# Propositions Créatives et Innovantes

## 1. IA Multispectrale
- Utiliser des LEDs de différentes longueurs d'onde (UV, Bleu, Vert, IR) pour capturer une série d'images.
- Certaines bactéries sont fluorescentes sous UV, ce qui faciliterait énormément la distinction.

## 2. Analyse de Croissance Temporelle (Time-lapse)
- Laisser l'ESP32-CAM dans l'incubateur.
- Prendre une photo toutes les heures.
- L'IA analyse la *vitesse* de croissance : les contaminants poussent souvent plus vite que les LAB.

### ⚠️ Défi : Gestion de la Condensation
Dans un incubateur humide à 37°C, le couvercle de la boîte de Pétri se couvre de gouttelettes, rendant l'image inexploitable.
*   **Solutions Matérielles :**
    - Placer une petite résistance chauffante (12V/1W) sur le dessus du couvercle pour évaporer la condensation.
    - Utiliser des boîtes de Pétri avec traitement anti-buée.
*   **Solutions Logicielles :**
    - Algorithme de détection de flou (Laplacian variance) pour rejeter les images illisibles et attendre une fenêtre de clarté.

## 3. Réalité Augmentée Microbiologique
- Interface web qui affiche le flux vidéo en direct avec des boîtes de couleur dessinées autour des colonies détectées en temps réel.

## 4. Gamification du tri
- Créer une petite interface web mobile type "Tinder" pour que l'utilisateur puisse swiper (Lactique/Contaminant) ses images capturées pour accélérer l'étiquetage du dataset.
