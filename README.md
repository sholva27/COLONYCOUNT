# ColonyCounter ESP32-CAM 🔬🧫

Système intelligent et low-cost de comptage et de classification de colonies bactériennes (Lactiques vs Contaminants) sur milieu MRS, basé sur ESP32-CAM.

## 🌟 Points Forts
- **Edge AI :** Détection et classification directement sur le microcontrôleur (Phase 3+).
- **Dataset Ready :** Système de capture optimisé avec stabilisation lumineuse pour créer vos propres bases de données.
- **Journalisation Scientifique :** Enregistrement automatique de la température, humidité et pression (BME280) pour chaque capture.
- **Éclairage Adaptatif :** Support PWM pour le flash et anneau NeoPixel pour un éclairage latéral sans reflets.
- **Résilience :** Watchdog matériel et gestion sécurisée de la carte SD (mode 1-bit).

## 🛠️ Matériel Requis
- **ESP32-CAM** (AI-Thinker avec PSRAM)
- **Capteur BME280** (I2C)
- **Module RTC DS3231** (I2C) + Pile CR2032 (Indispensable)
- **Bouton Tactile TTP223**
- **Anneau NeoPixel (16 LEDs)** + Bande LED High-CRI (Imagerie)
- **Carte Micro SD** (FAT32, < 32GB)
- **Condensateur 470µF** (Indispensable entre 5V et GND)
 - **Résistance 10kΩ** (Pull-down pour GPIO 12)
 - **MOSFET Canal-N** (ex: 2N7000, pour l'imagerie High-CRI)

## 🚀 Installation

### Via PlatformIO (Recommandé)
1. Clonez le dépôt.
2. Ouvrez le dossier dans VS Code avec PlatformIO.
3. Les dépendances s'installeront automatiquement.
4. Téléversez sur votre ESP32-CAM.

### Via Arduino IDE
Installez les bibliothèques suivantes via le Library Manager :
- `Adafruit NeoPixel`
- `Adafruit BME280 Library`
- `Adafruit Unified Sensor`
 - `RTClib` (Adafruit)

## 📖 Utilisation
1. Insérez une carte SD.
2. Alimentez le système (5V stable).
3. **LED Orange :** Initialisation.
4. **LED Verte :** Système prêt.
5. Touchez le bouton TTP223 :
   - L'anneau devient blanc.
   - 3 frames sont jetées pour stabiliser l'exposition.
   - La photo est enregistrée dans `/img/`.
   - Les données météo sont ajoutées dans `data.csv`.
   - **LED Bleue :** Succès.

## 📂 Structure du Projet
- `ColonyCounterCapture/` : Code source Arduino/C++.
- `DOCS/` : Documentation approfondie (Recherche, Hardware, Roadmap).
- `tools/` : Scripts Python pour la préparation des données et l'entraînement.

## 📝 Licence
Ce projet est sous licence MIT.
