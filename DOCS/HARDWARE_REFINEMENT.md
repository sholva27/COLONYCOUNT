# Raffinement Matériel (Hardware) - Guide Avancé

Pour transformer le prototype actuel en un outil de laboratoire robuste et précis, voici les améliorations matérielles proposées par ordre d'importance :

## 1. Amélioration de l'Optique (Crucial pour l'IA)
*   **Objectif Grand Angle vs Macro :** Remplacer l'objectif standard par une lentille **M12 macro** ou ajouter une lentille de lecture (+2 ou +4 dioptries) pour permettre une mise au point nette à 5-10 cm.
*   **Filtre Polarisant :** Placer un petit film polarisant sur les LEDs et un autre (croisé à 90°) sur la caméra. Cela élimine radicalement les reflets brillants sur l'agar humide et le plastique.

## 2. Contrôle de l'Éclairage
*   **Éclairage Variable (PWM) :** Utiliser un transistor MOSFET (ex: 2N7000) pour contrôler l'intensité des LEDs depuis l'ESP32. Cela permet d'ajuster la luminosité selon l'opacité du milieu MRS.
*   **Ring Light RGB :** Utiliser un anneau de LEDs NeoPixel (WS2812B). En changeant la couleur (ex: lumière verte ou bleue), on peut augmenter le contraste de certaines colonies lactiques translucides.

## 3. Capteurs Environnementaux
*   **Température et Humidité (BME280) :** Si l'appareil reste dans l'incubateur, suivre ces paramètres permet de corréler la vitesse de croissance détectée par l'IA avec les conditions réelles.
*   **Capteur de Poids (HX711 + Jauge de contrainte) :** Intégrer une petite balance sous le support de la boîte. Cela permet de vérifier la perte d'eau (dessiccation) de l'agar pendant les incubations longues.

## 4. Stérilisation et Hygiène
*   **LED UV-C :** Intégrer une LED UV-C (275nm) pointée vers le support (utilisable uniquement quand aucune boîte n'est présente) pour décontaminer la zone de travail entre deux manipulations.
*   **Boîtier Antimicrobien :** Imprimer les pièces du support en utilisant du filament PLA chargé en ions argent (antibactérien).

## 5. Autonomie et Portabilité
*   **Gestion de Batterie (TP4056) :** Ajouter un circuit de charge pour batterie LiPo 18650 afin de rendre l'appareil totalement autonome pour les prélèvements sur le terrain.
*   **Écran OLED (0.96") :** Connecter un petit écran en I2C pour afficher le compte de colonies en temps réel sans avoir besoin de regarder la console série ou le PC.

## 6. Structure Mécanique
*   **Support Centreur :** Un guide physique imprimé en 3D pour que la boîte de Pétri soit *toujours* exactement au même endroit par rapport à la caméra, ce qui simplifie énormément le travail de l'IA.
