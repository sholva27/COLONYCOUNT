# Raffinement Logiciel (Software) - V3

## 1. Gestion des données et Versioning
Le fichier `data.csv` inclut désormais une colonne **Version** pour assurer la traçabilité du schéma de données.
- **Header actuel :** `ID,Timestamp,Temp,Humidite,Pression,LuminositePWM,Version`
- **Firmware Version :** Le système enregistre `1.7.0` (ou supérieur).

## 2. Calibration Spatiale et Optique
Pour convertir les pixels en millimètres (UFC/mL), une calibration est indispensable.
- **Gabarit :** Utilisez un damier de calibration OpenCV standard (Chessboard).
- **Stockage :** Une capture du damier doit être effectuée pour chaque configuration (distance/focale).
- **Prochaines étapes :** Intégration d'un script `tools/calibrate_camera.py` utilisant OpenCV `findChessboardCorners` pour générer une matrice de transformation.

## 3. Fusion Multi-Cartes (Aggregation)
Lorsque la collecte utilise plusieurs appareils ou cartes SD :
- Le nommage par **Timestamp RTC** empêche les collisions de fichiers.
- L'outil `tools/merge_datasets.py` (à venir) permettra de fusionner plusieurs `dataset_index.csv` en un index maître unique.

## 4. Annotation et Étiquetage (Phase 2+)
L'outil `tools/colony_labeler.py` est le premier pas vers l'étiquetage manuel.
- **Amélioration prévue :** Une interface plus fluide (Streamlit/Tkinter) pour valider les labels en vrac plutôt que par clic individuel.

## 5. Mise à jour Firmware Simplifiée (OTA-SD)
Pour les déploiements multi-unités :
- Utilisation de la bibliothèque `Update.h` pour vérifier au boot la présence d'un `firmware.bin` sur la carte SD.
- Mise à jour automatique sans connexion USB.

## 6. Analyse Algorithmique Avancée
- **Détection de flou :** Rejeter les images où la condensation sur le couvercle de la boîte de Pétri empêche la vision.
- **Soustraction de fond locale :** Pour compenser l'hétérogénéité lumineuse du MRS.
