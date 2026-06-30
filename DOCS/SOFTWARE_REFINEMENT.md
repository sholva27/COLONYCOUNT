# Raffinement Logiciel (Software) - V2

## 1. Gestion des données (Journalisation)
Le système enregistre désormais un fichier `data.csv` à la racine de la carte SD à chaque capture.
- **Champs :** ID de l'image, Température (°C), Humidité (%), Pression (hPa), Intensité Flash (0-255).
- **Utilité :** Permet de corréler l'aspect des colonies avec les conditions de croissance.

## 2. Retour visuel par NeoPixel
L'anneau de LED sert d'interface utilisateur :
- **Orange fixe :** Démarrage et initialisation des périphériques.
- **Vert fixe (1s) :** Prêt pour la capture.
- **Blanc brillant :** Capture en cours (éclairage d'appoint).
- **Bleu flash :** Écriture sur carte SD réussie.
- **Rouge fixe :** Erreur critique (Carte SD absente ou Caméra HS).

## 3. Optimisations PWM
L'intensité du flash est réglée par défaut à 128/255 pour éviter l'éblouissement du capteur OV2640 sur le milieu MRS clair. Cette valeur peut être ajustée dans la variable `flashBrightness` du code.

## 4. Stabilité et Durcissement (Hardening)
Le code intègre plusieurs mécanismes de sécurité :
- **Watchdog (WDT) :** Si le système se bloque (ex: I2C ou SD), l'ESP32 redémarre automatiquement après 10 secondes.
- **Gestion I2C isolée :** Le bus I2C est ouvert (`Wire.begin`) uniquement lors de la lecture des capteurs et fermé immédiatement après (`Wire.end`) pour ne pas interférer avec le driver SCCB de la caméra.
- **Stabilisation AEC/AWB :** Avant la capture finale, 3 images sont capturées et jetées pour laisser le temps au capteur de stabiliser l'exposition lumineuse sous le flash.
- **Flag isProcessing :** Empêche tout chevauchement de processus (bouton pressé plusieurs fois trop vite).

## 5. Optimisation IA (TFLite Micro)
Pour l'inférence sur un ESP32 classique (sans accélération matérielle SIMD) :
- **Quantification INT8 :** Indispensable pour réduire la taille du modèle et accélérer les calculs.
- **Résolution d'entrée :** Limiter la taille à **96x96** ou **128x128** pixels.
- **Approche par tuiles :** Au lieu de traiter l'image UXGA entière, l'ESP32 doit d'abord localiser les colonies (via vision classique) puis soumettre chaque petite vignette (tuile) au modèle d'IA pour classification.

## 6. Algorithmes de Vision (Inspiration ImageJ)
Pour le comptage sur ESP32, nous privilégierons une approche hybride :
1.  **Soustraction de fond locale :** Pour compenser l'opacité variable du milieu MRS.
2.  **Seuillage d'Otsu :** Auto-adaptation à la luminosité de la capture.
3.  **Filtrage par taille/circularité :** Comme le fait l'outil "Analyze Particles" d'ImageJ pour éliminer les poussières et les bulles d'air.

## 7. Note sur le langage (C++ vs MicroPython)
Bien que MicroPython soit excellent pour le prototypage rapide, le déploiement final de l'IA (Phase 4) doit se faire en **C++/ESP-IDF**. MicroPython manque de support natif performant pour TensorFlow Lite Micro et les opérations de traitement d'image pixel par pixel y sont trop lentes pour cette application.
