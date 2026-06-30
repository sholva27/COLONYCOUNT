/**
 * @file ColonyCounterCapture.ino
 * @author Jules
 * @brief Système de capture durci pour Dataset (v1.6).
 *        - Paramètres caméra FIGÉS (AEC/AWB) pour cohérence IA
 *        - Nommage fichiers par horodatage RTC
 *        - Diagnostic visuel avancé et gestion espace SD
 */

#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"
#include "SD_MMC.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#include <esp_task_wdt.h>

// Pins
const int BUTTON_PIN = 13;
const int IMAGING_LIGHT_PIN = 4;
const int NEOPIXEL_PIN = 12;
const int NUMPIXELS = 16;

#define I2C_SDA 26
#define I2C_SCL 27
#define WDT_TIMEOUT 15

// Couleurs Status
const uint32_t COLOR_INIT = 0xFFA500;    // Orange
const uint32_t COLOR_READY = 0x00FF00;   // Vert
const uint32_t COLOR_CAPTURE = 0xFFFFFF; // Blanc
const uint32_t COLOR_SUCCESS = 0x0000FF; // Bleu
const uint32_t COLOR_ERR_CAM = 0xFF0000; // Rouge
const uint32_t COLOR_ERR_SD = 0xFF00FF;  // Magenta
const uint32_t COLOR_WARN_SPACE = 0x800080; // Violet

Preferences preferences;
Adafruit_BME280 bme;
RTC_DS3231 rtc;
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Camera Pins AI-Thinker
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

int pictureNumber = 0;
bool bmeFound = false;
bool rtcFound = false;
bool isInitialized = false;
volatile bool isProcessing = false;
int flashBrightness = 128;

const int pwmChannel = 7;
const int pwmFreq = 5000;
const int pwmResolution = 8;

void showColor(uint32_t color) {
  for(int i=0; i<NUMPIXELS; i++) pixels.setPixelColor(i, color);
  pixels.show();
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);

  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  pixels.begin();
  pixels.setBrightness(50);
  showColor(COLOR_INIT);

  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(IMAGING_LIGHT_PIN, pwmChannel);
  ledcWrite(pwmChannel, 0);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    showColor(COLOR_ERR_CAM);
    return;
  }

  // FIGER LES RÉGLAGES CAPTEUR (Après init)
  sensor_t * s = esp_camera_sensor_get();
  s->set_whitebal(s, 0);       // Désactiver AWB
  s->set_wb_mode(s, 0);        // Mode fixe (Daylight/Manual)
  s->set_exposure_ctrl(s, 0);  // Désactiver AEC
  s->set_aec_value(s, 300);    // Valeur fixe à ajuster selon éclairage
  s->set_gain_ctrl(s, 0);      // Désactiver AGC
  s->set_agc_gain(s, 0);       // Gain fixe

  Wire.begin(I2C_SDA, I2C_SCL);
  if (bme.begin(0x76, &Wire)) bmeFound = true;
  if (rtc.begin(&Wire)) {
    rtcFound = true;
    if (rtc.lostPower()) {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }
  Wire.end();

  if(!SD_MMC.begin("/sdcard", true)){
    showColor(COLOR_ERR_SD);
    return;
  }

  if(!SD_MMC.exists("/img")) SD_MMC.mkdir("/img");

  // Vérification espace SD
  uint64_t freeBytes = SD_MMC.totalBytes() - SD_MMC.usedBytes();
  if (freeBytes < 50 * 1024 * 1024) {
      showColor(COLOR_WARN_SPACE);
      delay(2000);
  }

  if(!SD_MMC.exists("/data.csv")){
    File file = SD_MMC.open("/data.csv", FILE_WRITE);
    if(file) {
      file.println("ID,Timestamp,Temp,Humidite,Pression,LuminositePWM");
      file.close();
    }
  }

  preferences.begin("colony-counter", false);
  pictureNumber = preferences.getUInt("num", 0);

  showColor(COLOR_READY);
  delay(1000);
  pixels.clear();
  pixels.show();

  isInitialized = true;
  esp_task_wdt_reset();
}

String getTimestampInternal() {
  if (!rtcFound) return "00000000_000000";
  DateTime now = rtc.now();
  char buf[] = "YYYYMMDD_hhmmss";
  return String(now.toString(buf));
}

void takePicture() {
  if(isProcessing) return;
  isProcessing = true;
  esp_task_wdt_reset();

  ledcWrite(pwmChannel, flashBrightness);
  showColor(COLOR_CAPTURE);

  // Stabilisation AEC/AWB (même si désactivés, le capteur a besoin de frames pour se caler)
  for(int i = 0; i < 3; i++) {
    camera_fb_t * dummy_fb = esp_camera_fb_get();
    if(dummy_fb) esp_camera_fb_return(dummy_fb);
    delay(50);
  }

  camera_fb_t * fb = esp_camera_fb_get();
  if(!fb) {
    ledcWrite(pwmChannel, 0);
    pixels.clear();
    pixels.show();
    isProcessing = false;
    return;
  }

  Wire.begin(I2C_SDA, I2C_SCL);
  String ts_filename = getTimestampInternal();
  DateTime now = rtc.now();
  char ts_csv_buf[] = "YYYY-MM-DD hh:mm:ss";
  String ts_csv = String(now.toString(ts_csv_buf));

  float t = bmeFound ? bme.readTemperature() : -999.0;
  float h = bmeFound ? bme.readHumidity() : -999.0;
  float p = bmeFound ? bme.readPressure() / 100.0F : -999.0;
  Wire.end();

  String path = "/img/colony_" + ts_filename + ".jpg";

  esp_task_wdt_reset();
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if(file){
    file.write(fb->buf, fb->len);
    file.close();
    esp_task_wdt_reset();

    File csv = SD_MMC.open("/data.csv", FILE_APPEND);
    if(csv) {
      csv.printf("%d,%s,%.2f,%.2f,%.2f,%d\n", pictureNumber, ts_csv.c_str(), t, h, p, flashBrightness);
      csv.close();
    }

    pictureNumber++;
    preferences.putUInt("num", pictureNumber);
    showColor(COLOR_SUCCESS);
  } else {
    showColor(COLOR_ERR_SD);
  }

  esp_camera_fb_return(fb);
  delay(200);
  ledcWrite(pwmChannel, 0);
  pixels.clear();
  pixels.show();

  while(digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
      esp_task_wdt_reset();
  }

  // Vérification espace périodique
  if (SD_MMC.totalBytes() - SD_MMC.usedBytes() < 50 * 1024 * 1024) {
      showColor(COLOR_WARN_SPACE);
  }

  esp_task_wdt_reset();
  isProcessing = false;
}

void loop() {
  esp_task_wdt_reset();
  if (!isInitialized) return;

  if (digitalRead(BUTTON_PIN) == LOW && !isProcessing) {
    unsigned long pressTime = millis();
    bool confirmed = false;
    while(digitalRead(BUTTON_PIN) == LOW) {
      if (millis() - pressTime > 200) {
        confirmed = true;
        break;
      }
      esp_task_wdt_reset();
    }

    if (confirmed) {
        takePicture();
    }
  }
}
