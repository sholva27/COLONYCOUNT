# Configuration Matérielle - Compteur de Colonies ESP32-CAM (V2)

## 1. Brochage Mis à Jour (Advanced Pinout)

| Composant | Pin ESP32 | Description |
| :--- | :--- | :--- |
| **Bouton TTP223** | GPIO 13 | Déclenchement capture (Signal Actif à l'état BAS / Active LOW). |
| **Flash Intégré** | GPIO 4 | Contrôlé en **PWM** pour ajuster la luminosité. |
| **NeoPixel Ring** | GPIO 12 | Éclairage annulaire RGB (⚠️ Pin de boot critique). |
| **Capteur BME280** | GPIO 26 (SDA) / 27 (SCL) | Température, Humidité, Pression (I2C). |
| **Carte SD (1-bit)** | GPIO 2, 14, 15 | Stockage images et logs CSV. |

## 2. Détails techniques et Subtilités Critiques

### A. GPIO 12 (MTDI) - Risque de Boot Loop
Le **GPIO 12** est une "strapping pin". S'il est tiré vers le haut (HIGH) au démarrage, l'ESP32 configure le voltage de sa mémoire flash interne à **1.8V** au lieu de 3.3V, provoquant un échec du boot (Crash/Reboot loop).
- **Solution :** Ajoutez une résistance de **pull-down de 10kΩ** entre GPIO 12 et GND pour garantir un boot sécurisé.

### B. GPIO 4 (Flash) et Carte SD
Le flash intégré est sur le **GPIO 4**. Sur l'ESP32-CAM, cette broche est partagée avec le slot SD (Data 1).
- **Condition de succès :** Le code force le mode **SD 1-bit** (`SD_MMC.begin("/sdcard", true)`), ce qui libère le GPIO 4 pour un usage PWM sans interférer avec la carte SD.

### C. Logique 3.3V vers 5V (NeoPixel)
Les NeoPixels (WS2812B) attendent un signal logique à 5V, mais l'ESP32 sort du 3.3V. Bien que cela "tombe en marche" souvent, c'est hors spécification.
- **Solution recommandée :** Utiliser un convertisseur de niveau logique (Level Shifter) ou alimenter la première LED via une diode pour abaisser son seuil de détection.

### D. I2C Partagé
Le bus I2C (BME280) partage les pins **GPIO 26/27** avec le protocole SCCB de la caméra. Le code gère cela en ouvrant/fermant le bus `Wire` dynamiquement.

## 3. Schéma de connexion
- **NeoPixel VCC :** 5V.
- **BME280 VCC :** 3.3V.
- **Bouton SIG :** GPIO 13 (Mode 'Active LOW' impératif).
