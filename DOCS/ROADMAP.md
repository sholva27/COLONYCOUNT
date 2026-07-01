# Roadmap du Projet

## Phase 1 : Système de Capture (Terminé) ✅
- [x] Initialisation ESP32-CAM optimisée (PSRAM).
- [x] Gestion Carte SD (mode 1-bit) et bouton tactile (anti-rebond).
- [x] Système de nommage basé sur RTC (Horodatage UNIQUE).
- [x] Contrôle d'éclairage PWM (High-CRI) et NeoPixel (Status).
- [x] Figeage des réglages capteur (AWB/AEC/AGC) pour dataset cohérent.
- [x] Intégration capteurs BME280 (Environnement) et DS3231 (RTC).
- [x] Journalisation CSV scientifique sur SD.
- [x] Gestion de l'espace disque et watchdog matériel.
- [x] Documentation complète (BOM, Câblage, Recherche).

## Phase 2 : Constitution du Dataset ⏳
- [ ] Capture de **2000 à 5000 images** (Dataset équilibré LAB / Contaminants).
- [x] **Outil d'étiquetage manuel** (`colony_labeler.py`) opérationnel.
- [x] **Script de préparation de données** (`prepare_data.py`) opérationnel.
- [ ] Analyse statistique des tailles et couleurs.

## Phase 3 : Entraînement de l'IA ⏳
- [x] **Script d'entraînement** (`train_colony_model.py`) prêt.
- [ ] Data Augmentation massive (Rotation, Flip, Zoom, Bruit, Variations de contraste).
- [ ] Choix de l'architecture : Modèle personnalisé ultra-léger (ex: 128x128 pixels).
- [ ] **Optionnel : Migration vers ESP32-S3** pour accélérer l'inférence par 15x.
- [ ] Entraînement sur PC (TensorFlow/Keras).
- [ ] Conversion vers TensorFlow Lite Micro (TFLM).

## Phase 4 : Déploiement Inférence ⏳
- [ ] Intégration du modèle .tflite sur l'ESP32.
- [ ] Optimisation de la RAM (utilisation de la PSRAM).
- [ ] Développement de l'interface de résultat (Série/Web/CSV).

## Phase 5 : Raffinement et Portabilité ⏳
- [ ] Optimisation de l'interface utilisateur (OLED/Web).
- [ ] Conception d'un boîtier imprimé en 3D avec gestion de la condensation.
- [ ] Intégration d'un écran OLED pour affichage direct du compte.

## Phase 6 : Évolution Professionnelle (Optionnelle) 🚀
- [ ] Migration vers l'**ESP32-P4** (WT9932P4-TINY).
- [ ] Passage aux capteurs MIPI-CSI (5MP+).
- [ ] Implémentation de l'accélération IA matérielle.
