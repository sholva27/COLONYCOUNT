/**
 * @file ColonyCounterCapture.ino
 * @author Jules
 * @brief Système de capture d'images pour compteur de colonies bactériennes.
 *        Utilise un ESP32-CAM, une carte SD et un bouton tactile TTP223.
 */

#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"
#include "SD_MMC.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <Preferences.h>

// Instance Preferences pour stocker l'index de l'image
Preferences preferences;

// Pin du bouton TTP223
const int BUTTON_PIN = 13;

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

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Désactiver le brownout detector

  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);

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
    config.frame_size = FRAMESIZE_UXGA; // Haute résolution pour la capture
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Initialisation de la caméra
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Initialisation de la carte SD (mode 1-bit)
  if(!SD_MMC.begin("/sdcard", true)){
    Serial.println("SD Card Mount Failed");
    return;
  }

  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }

  // Création du dossier /img s'il n'existe pas
  if(!SD_MMC.exists("/img")){
      SD_MMC.mkdir("/img");
  }

  // Lecture du numéro d'image depuis la mémoire Flash (NVS)
  preferences.begin("colony-counter", false);
  pictureNumber = preferences.getUInt("num", 0);

  Serial.println("Système prêt. Appuyez sur le bouton pour capturer une image.");
}

void takePicture() {
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Chemin du fichier
  String path = "/img/colony_" + String(pictureNumber) + ".jpg";

  fs::FS &fs = SD_MMC;
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
  } else {
    file.write(fb->buf, fb->len);
    Serial.printf("Saved file to path: %s\n", path.c_str());
    pictureNumber++;
    preferences.putUInt("num", pictureNumber);
  }
  file.close();
  esp_camera_fb_return(fb);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println("Bouton pressé ! Capture en cours...");
    takePicture();

    // Feedback visuel (flash rapide si disponible ou simple pause)
    delay(1000); // Anti-rebond et temps de latence entre captures
    while(digitalRead(BUTTON_PIN) == HIGH); // Attendre que le bouton soit relâché
  }
}
