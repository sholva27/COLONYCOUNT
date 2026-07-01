# ColonyCounter ESP32-CAM 🔬🧫

Système intelligent et low-cost de comptage et de classification de colonies bactériennes (Lactiques vs Contaminants) sur milieu MRS, basé sur ESP32-CAM.

## 🌟 Points Forts
- **Edge AI Ready :** Conçu pour l'inférence locale via TensorFlow Lite Micro (Phase 4).
- **Dataset Ready :** Système de capture durci avec figeage des registres capteur (AWB/AEC) pour créer des bases de données scientifiques cohérentes.
- **Journalisation Scientifique :** Enregistrement de la température, humidité, pression (BME280) et horodatage précis (RTC DS3231) pour chaque capture.
- **Double Éclairage :** Gestion séparée du status (NeoPixel) et de l'imagerie (High-CRI LED via PWM).
- **Résilience :** Watchdog matériel (15s), gestion d'espace SD et protection contre les brownouts.

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

### Via Python (Pour les outils de données)
```bash
pip install -r tools/requirements.txt
```

## 📖 Utilisation & Codes Couleur
1. Insérez une carte SD.
2. Alimentez le système (5V stable).
3. **LED Orange :** Initialisation en cours.
4. **LED Verte :** Système prêt, en attente de capture.
5. Touchez le bouton TTP223 :
   - **LED Blanche :** Capture en cours (Flash High-CRI actif).
   - **LED Bleue :** Succès (Image enregistrée dans `/img/`, données dans `data.csv`).
6. **Codes d'erreur :**
   - **LED Rouge :** Erreur Initialisation Caméra.
   - **LED Magenta :** Erreur Carte SD (Absente ou défectueuse).
   - **LED Violette :** Espace SD faible (< 50 Mo).

## 📂 Structure du Projet
- `ColonyCounterCapture/` : Code source Arduino/C++ (PlatformIO).
- `DOCS/` : Documentation approfondie (BOM, WIRING, ROADMAP, RESEARCH).
- `tools/` : Scripts Python (Labeling, Training, Data Prep).

## 📝 Licence
Ce projet est sous licence MIT.
