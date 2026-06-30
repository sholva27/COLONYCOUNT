# Guide de Conception 3D

Ce dossier contiendra les modèles STL pour le boîtier du compteur de colonies. Voici les spécifications de conception suivies :

## 1. Chambre de Capture
- **Hauteur (Focale) :** 80mm entre l'objectif et la boîte de Pétri (ajustable selon l'objectif macro utilisé).
- **Isolation Lumineuse :** Boîtier opaque (noir ou gris foncé) pour éviter les lumières parasites externes.
- **Support de Boîte :** Diamètre de 92mm avec un épaulement de centrage pour les boîtes de Pétri standard.

## 2. Éclairage (Double Système)
- **Status (Haut) :** Emplacement pour anneau NeoPixel **16 LEDs (Diamètre ext. 44.5mm)** autour de l'objectif.
- **Imagerie (Bas) :** Logement circulaire de **diamètre 100mm** pour ruban LED COB blanc. Incliné à 30-45° pour un éclairage tangentiel (Darkfield-like).
- **Diffuseur :** Support pour anneau diffusant imprimé en PETG translucide (1 couche) ou papier calque.

## 3. Maintenance et Accès
- **Trappe SD :** Ouverture latérale pour retirer la carte Micro SD sans démonter le boîtier.
- **Passage Câble :** Orifice pour le câble USB (alimentation 5V).
- **Emplacement Bouton :** Support pour le capteur TTP223 sur le dessus pour un accès facile.

## 4. Gestion Thermique
- **Évents :** Petites fentes d'aération sur le dessus pour laisser s'échapper la chaleur de l'ESP32 sans laisser entrer la lumière (chicane).
