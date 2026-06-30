# Roadmap du Projet

## Phase 1 : Système de Capture (Actuel) ✅
- [x] Initialisation ESP32-CAM.
- [x] Gestion Carte SD et bouton tactile.
- [x] Système de nommage séquentiel des images.
- [x] Contrôle d'éclairage PWM et NeoPixel.
- [x] Intégration capteur BME280 et journalisation CSV.
- [x] Documentation initiale et avancée.

## Phase 2 : Constitution du Dataset ⏳
- [ ] Capture de **2000 à 5000 images** (Dataset équilibré LAB / Contaminants).
- [ ] Étiquetage manuel (Annotation) des images sur PC.
- [ ] Analyse statistique des tailles et couleurs.

## Phase 3 : Entraînement de l'IA ⏳
- [ ] **Data Augmentation massive** (Rotation, Flip, Zoom, Bruit, Variations de contraste).
- [ ] Choix de l'architecture : Modèle personnalisé ultra-léger (ex: 96x96 pixels).
- [ ] **Optionnel : Migration vers ESP32-S3** pour bénéficier des instructions SIMD et accélérer l'inférence par 15x.
- [ ] Entraînement sur PC (TensorFlow/Keras).
- [ ] Conversion vers TensorFlow Lite Micro (TFLM).

## Phase 4 : Déploiement Inférence ⏳
- [ ] Intégration du modèle .tflite sur l'ESP32.
- [ ] Optimisation de la RAM (utilisation de la PSRAM).
- [ ] Développement de l'interface de résultat (Série/Web).

## Phase 5 : Raffinement et Portabilité ⏳
- [ ] Optimisation de l'interface utilisateur (OLED/Web).
- [ ] Conception d'un boîtier imprimé en 3D avec gestion de la condensation.
- [ ] Intégration d'un écran OLED pour affichage direct du compte.

## Phase 6 : Évolution Professionnelle (Optionnelle) 🚀
- [ ] Migration vers l'**ESP32-P4** (WT9932P4-TINY).
- [ ] Passage aux capteurs MIPI-CSI (5MP+).
- [ ] Implémentation de l'accélération IA matérielle.
