# Roadmap du Projet

## Phase 1 : Système de Capture (Actuel) ✅
- [x] Initialisation ESP32-CAM.
- [x] Gestion Carte SD et bouton tactile.
- [x] Système de nommage séquentiel des images.
- [x] Contrôle d'éclairage PWM et NeoPixel.
- [x] Intégration capteur BME280 et journalisation CSV.
- [x] Documentation initiale et avancée.

## Phase 2 : Constitution du Dataset ⏳
- [ ] Capture de 500+ images (250 lactiques, 250 contaminants).
- [ ] Étiquetage manuel (Annotation) des images sur PC.
- [ ] Analyse statistique des tailles et couleurs.

## Phase 3 : Entraînement de l'IA ⏳
- [ ] Choix de l'architecture (MobileNetV2, TinyYOLO ou CNN personnalisé).
- [ ] Entraînement sur PC (TensorFlow/Keras).
- [ ] Conversion vers TensorFlow Lite Micro (TFLM).

## Phase 4 : Déploiement Inférence ⏳
- [ ] Intégration du modèle .tflite sur l'ESP32.
- [ ] Optimisation de la RAM (utilisation de la PSRAM).
- [ ] Développement de l'interface de résultat (Série/Web).

## Phase 5 : Raffinement et Portabilité ⏳
- [ ] Passage à MicroPython.
- [ ] Conception d'un boîtier imprimé en 3D.
- [ ] Intégration d'un écran OLED pour affichage direct du compte.

## Phase 6 : Évolution Professionnelle (Optionnelle) 🚀
- [ ] Migration vers l'**ESP32-P4** (WT9932P4-TINY).
- [ ] Passage aux capteurs MIPI-CSI (5MP+).
- [ ] Implémentation de l'accélération IA matérielle.
