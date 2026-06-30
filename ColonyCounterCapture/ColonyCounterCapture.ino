/**
 * @file ColonyCounterCapture.ino
 * @author Jules
 * @brief Système de capture d'images durci (v1.3).
 *        - Watchdog protégé pendant le boot (WiFi/NTP)
 *        - Anti-rebond et anti-multi-déclenchement pour TTP223
 *        - Gestion isolée du bus I2C
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
#include <esp_task_wdt.h>
#include <WiFi.h>
#include "time.h"

// Configuration WiFi pour NTP
const char* ssid     = "VOTRE_SSID";
const char* password = "VOTRE_PASSWORD";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// Pins
const int BUTTON_PIN = 13;
const int IMAGING_LIGHT_PIN = 4;
const int NEOPIXEL_PIN = 12;
const int NUMPIXELS = 16;

#define I2C_SDA 26
#define I2C_SCL 27
#define WDT_TIMEOUT 15 // Augmenté à 15s pour plus de marge

Preferences preferences;
Adafruit_BME280 bme;
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
bool isInitialized = false;
volatile bool isProcessing = false;
int flashBrightness = 128;

const int pwmChannel = 7;
const int pwmFreq = 5000;
const int pwmResolution = 8;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);

  // Initialisation Watchdog
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  pixels.begin();
  pixels.setBrightness(50);
  showColor(pixels.Color(255, 165, 0));

  // WiFi et NTP avec Reset du Watchdog
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
    esp_task_wdt_reset(); // Empêcher le reset WDT pendant la connexion WiFi
  }

  if(WiFi.status() == WL_CONNECTED) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.print("Synchro NTP");
    int retry = 0;
    while (time(nullptr) < 1000000 && retry < 20) {
      delay(500);
      Serial.print(".");
      esp_task_wdt_reset(); // Empêcher le reset WDT pendant l'attente NTP
      retry++;
    }
    Serial.println(time(nullptr) > 1000000 ? " OK" : " Échec");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }

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
    showColor(pixels.Color(255, 0, 0));
    return;
  }

  // I2C Management
  Wire.begin(I2C_SDA, I2C_SCL);
  if (bme.begin(0x76, &Wire)) bmeFound = true;
  Wire.end();

  if(!SD_MMC.begin("/sdcard", true)){
    showColor(pixels.Color(255, 0, 0));
    return;
  }

  if(!SD_MMC.exists("/img")) SD_MMC.mkdir("/img");

  if(!SD_MMC.exists("/data.csv")){
    File file = SD_MMC.open("/data.csv", FILE_WRITE);
    if(file) {
      file.println("ID,Timestamp,Temp,Humidite,Pression,LuminositePWM");
      file.close();
    }
  }

  preferences.begin("colony-counter", false);
  pictureNumber = preferences.getUInt("num", 0);

  showColor(pixels.Color(0, 255, 0));
  delay(1000);
  pixels.clear();
  pixels.show();

  isInitialized = true;
  esp_task_wdt_reset();
}

void showColor(uint32_t color) {
  for(int i=0; i<NUMPIXELS; i++) pixels.setPixelColor(i, color);
  pixels.show();
}

String getTimestamp() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return "0000-00-00 00:00:00";
  char timeString[20];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeString);
}

void takePicture() {
  if(isProcessing) return;
  isProcessing = true;
  esp_task_wdt_reset();

  ledcWrite(pwmChannel, flashBrightness);
  showColor(pixels.Color(255, 255, 255));

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

  String timestamp = getTimestamp();
  String path = "/img/colony_" + String(pictureNumber) + ".jpg";
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if(file){
    file.write(fb->buf, fb->len);
    file.close();

    Wire.begin(I2C_SDA, I2C_SCL);
    float t = bmeFound ? bme.readTemperature() : 0;
    float h = bmeFound ? bme.readHumidity() : 0;
    float p = bmeFound ? bme.readPressure() / 100.0F : 0;
    Wire.end();
    delay(10); // Laisse le bus se stabiliser avant tout usage SCCB par AEC/AGC

    File csv = SD_MMC.open("/data.csv", FILE_APPEND);
    if(csv) {
      csv.printf("%d,%s,%.2f,%.2f,%.2f,%d\n", pictureNumber, timestamp.c_str(), t, h, p, flashBrightness);
      csv.close();
    }

    pictureNumber++;
    preferences.putUInt("num", pictureNumber);
    showColor(pixels.Color(0, 0, 255));
  }

  esp_camera_fb_return(fb);
  delay(200);
  ledcWrite(pwmChannel, 0);
  pixels.clear();
  pixels.show();

  // Attendre le relâchement du bouton (TTP223 a un temps de décharge)
  while(digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
      esp_task_wdt_reset();
  }

  esp_task_wdt_reset();
  isProcessing = false;
}

void loop() {
  esp_task_wdt_reset();
  if (!isInitialized) return;

  // Déclenchement sur front descendant (Active LOW) avec validation de durée (200ms)
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
