/**
 * @file ColonyCounterCapture.ino
 * @author Jules
 * @brief Système de capture d'images avec contrôle d'éclairage et capteurs.
 *        - ESP32-CAM + Carte SD (1-bit)
 *        - Bouton TTP223 (GPIO 13)
 *        - Éclairage PWM (GPIO 4)
 *        - NeoPixel Ring (GPIO 12)
 *        - Capteur BME280 (I2C partagé sur GPIO 26/27)
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

// Pins
const int BUTTON_PIN = 13; // GPIO 13 has external pull-up on AI-Thinker
const int FLASH_PWM_PIN = 4;
const int NEOPIXEL_PIN = 12;
const int NUMPIXELS = 12; // Ajuster selon votre anneau

// Configuration I2C pour BME280 (partagé avec la caméra)
#define I2C_SDA 26
#define I2C_SCL 27

// Instances
Preferences preferences;
Adafruit_BME280 bme;
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Pins de la caméra (AI-Thinker)
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

// Configuration PWM pour le flash
const int pwmChannel = 7;
const int pwmFreq = 5000;
const int pwmResolution = 8;
int flashBrightness = 128; // 0-255

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);

  // GPIO 13 a une résistance de pull-up physique.
  // Le TTP223 doit être configuré pour tirer vers GND (Active LOW)
  // OU on change la logique de détection.
  pinMode(BUTTON_PIN, INPUT);

  // Initialisation NeoPixel
  pixels.begin();
  pixels.setBrightness(50);
  showColor(pixels.Color(255, 165, 0)); // Orange : Initialisation

  // Initialisation PWM Flash
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(FLASH_PWM_PIN, pwmChannel);
  ledcWrite(pwmChannel, 0);

  // Configuration de la caméra
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
    Serial.printf("Camera init failed with error 0x%x", err);
    showColor(pixels.Color(255, 0, 0)); // Rouge : Erreur
    return;
  }

  // Initialisation I2C explicite pour BME280
  Wire.begin(I2C_SDA, I2C_SCL);
  if (bme.begin(0x76, &Wire)) { // Adresse 0x76 ou 0x77
    bmeFound = true;
    Serial.println("BME280 trouvé !");
  } else {
    Serial.println("BME280 non trouvé. Vérifiez le câblage.");
  }

  // Initialisation Carte SD (mode 1-bit)
  if(!SD_MMC.begin("/sdcard", true)){
    Serial.println("SD Card Mount Failed");
    showColor(pixels.Color(255, 0, 0));
    return;
  }

  if(!SD_MMC.exists("/img")) SD_MMC.mkdir("/img");

  // Initialisation CSV si inexistant
  if(!SD_MMC.exists("/data.csv")){
    File file = SD_MMC.open("/data.csv", FILE_WRITE);
    if(file) {
      file.println("ID,Temp,Humidite,Pression,LuminositePWM");
      file.close();
    }
  }

  preferences.begin("colony-counter", false);
  pictureNumber = preferences.getUInt("num", 0);

  showColor(pixels.Color(0, 255, 0)); // Vert : Prêt
  delay(1000);
  pixels.clear();
  pixels.show();

  Serial.println("Système prêt.");
}

void showColor(uint32_t color) {
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}

void logData(float t, float h, float p) {
  File file = SD_MMC.open("/data.csv", FILE_APPEND);
  if(file) {
    file.printf("%d,%.2f,%.2f,%.2f,%d\n", pictureNumber, t, h, p, flashBrightness);
    file.close();
  }
}

void takePicture() {
  // Allumage éclairage pour capture
  ledcWrite(pwmChannel, flashBrightness);
  showColor(pixels.Color(255, 255, 255)); // Blanc pour capture
  delay(200); // Temps de stabilisation lumineuse

  camera_fb_t * fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Capture échouée");
    ledcWrite(pwmChannel, 0);
    pixels.clear();
    pixels.show();
    return;
  }

  String path = "/img/colony_" + String(pictureNumber) + ".jpg";
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if(file){
    file.write(fb->buf, fb->len);
    file.close();
    Serial.printf("Image sauvegardée : %s\n", path.c_str());

    // Lecture capteurs et log
    float t = bmeFound ? bme.readTemperature() : 0;
    float h = bmeFound ? bme.readHumidity() : 0;
    float p = bmeFound ? bme.readPressure() / 100.0F : 0;
    logData(t, h, p);

    pictureNumber++;
    preferences.putUInt("num", pictureNumber);
    showColor(pixels.Color(0, 0, 255)); // Bleu : Succès
  }

  esp_camera_fb_return(fb);
  delay(500);
  ledcWrite(pwmChannel, 0);
  pixels.clear();
  pixels.show();
}

void loop() {
  // Détection du bouton : On attend une transition LOW (appui) car GPIO 13 est HIGH par défaut
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // Debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      takePicture();
      while(digitalRead(BUTTON_PIN) == LOW); // Attendre relâchement
    }
  }
}
