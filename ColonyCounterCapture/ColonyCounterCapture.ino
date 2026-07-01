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
const uint32_t COLOR_AI_BUSY = 0xFFFF00;  // Jaune

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

const String FIRMWARE_VERSION = "1.7.0";
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
  config.pixel_format = PIXFORMAT_JPEG; // JPEG pour stockage, besoin de conversion RGB pour IA

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

  sensor_t * s = esp_camera_sensor_get();
  s->set_whitebal(s, 0);       // Désactive l'AWB automatique
  s->set_awb_gain(s, 0);       // Désactive explicitement le gain AWB
  s->set_wb_mode(s, 1);        // Preset "Sunny" (Fixe, adapté ~6000K)
  s->set_exposure_ctrl(s, 0);  // Désactive l'AEC automatique
  s->set_aec_value(s, 300);    // Valeur d'exposition fixe
  s->set_gain_ctrl(s, 0);      // Désactive l'AGC automatique
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

  // Vérification espace SD initial
  if (SD_MMC.totalBytes() - SD_MMC.usedBytes() < 50 * 1024 * 1024) {
    showColor(COLOR_WARN_SPACE);
    delay(2000);
  }

  if(!SD_MMC.exists("/data.csv")){
    File file = SD_MMC.open("/data.csv", FILE_WRITE);
    if(file) {
      file.println("ID,Timestamp,Temp,Humidite,Pression,LuminositePWM,Version");
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

void takePicture() {
  if(isProcessing) return;
  isProcessing = true;
  esp_task_wdt_reset();

  ledcWrite(pwmChannel, flashBrightness);
  showColor(COLOR_CAPTURE);

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

  // Correction Race Condition RTC: Lecture UNIQUE de l'heure
  String ts_filename = "00000000_000000";
  String ts_csv = "0000-00-00 00:00:00";

  if (rtcFound) {
    DateTime now = rtc.now();
    char buf_fn[] = "YYYYMMDD_hhmmss";
    ts_filename = String(now.toString(buf_fn));
    char buf_csv[] = "YYYY-MM-DD hh:mm:ss";
    ts_csv = String(now.toString(buf_csv));
  }

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
      csv.printf("%d,%s,%.2f,%.2f,%.2f,%d,%s\n",
                 pictureNumber, ts_csv.c_str(), t, h, p, flashBrightness, FIRMWARE_VERSION.c_str());
      csv.close();
    }

    pictureNumber++;
    preferences.putUInt("num", pictureNumber);
    showColor(COLOR_SUCCESS);
  } else {
    showColor(COLOR_ERR_SD);
  }

  esp_camera_fb_return(fb);
  delay(500);
  ledcWrite(pwmChannel, 0);
  pixels.clear();
  pixels.show();

  isProcessing = false;

  // Attente du relâchement du bouton pour éviter les déclenchements multiples
  while(digitalRead(BUTTON_PIN) == LOW) {
    delay(10);
    esp_task_wdt_reset();
  }

  // Vérification de l'espace SD restant (< 50 Mo)
  if (SD_MMC.totalBytes() - SD_MMC.usedBytes() < 50 * 1024 * 1024) {
    showColor(COLOR_WARN_SPACE);
  }
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
