# Raffinement Logiciel (Software)

## 1. Prétraitement d'image
- Implémenter un algorithme de correction de vignettage (les bords sont souvent plus sombres).
- Conversion en niveaux de gris ou en espace couleur LAB pour mieux isoler les colonies du fond.

## 2. Optimisation IA
- Utilisation de la quantification 8-bit (INT8) pour diviser par 4 la taille du modèle.
- Élagage (Pruning) des neurones inutiles pour augmenter la vitesse d'inférence.

## 3. Système de Fichiers
- Utiliser LittleFS pour les petits fichiers de configuration et SD_MMC pour les images.
- Implémenter un système de rotation de logs pour ne pas saturer la SD.

## 4. Mode Sommeil
- Mettre l'ESP32 en Deep Sleep entre les captures pour économiser la batterie (si utilisation portable).
- Réveil (Wake-up) via le bouton tactile sur le GPIO 13.
