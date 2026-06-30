# Raffinement Matériel (Hardware) - Guide Avancé

Pour transformer le prototype actuel en un outil de laboratoire robuste et précis, voici les améliorations matérielles proposées par ordre d'importance :

## 1. Amélioration de l'Optique (Crucial pour l'IA)
*   **Objectif Grand Angle vs Macro :** Remplacer l'objectif standard par une lentille **M12 macro** ou ajouter une lentille de lecture (+2 ou +4 dioptries) pour permettre une mise au point nette à 5-10 cm.
*   **Filtre Polarisant :** Placer un petit film polarisant sur les LEDs et un autre (croisé à 90°) sur la caméra. Cela élimine radicalement les reflets brillants sur l'agar humide et le plastique.

## 2. Architecture d'Éclairage à Deux Composants
Pour garantir à la fois une interface utilisateur claire et des images scientifiquement exploitables, le projet sépare les fonctions :

### A. Éclairage de Status (NeoPixel Ring 16 LEDs)
- **Modèle :** Diamètre 44.5mm.
- **Rôle :** Retour visuel sur l'état du système (Prêt, Erreur, WiFi...).
- **Justification :** Le format 16 LEDs offre une meilleure densité visuelle que le 12 LEDs pour un anneau continu.

### B. Éclairage d'Imagerie (High-CRI 95+)
- **Source :** Bande LED COB Blanc Froid (6000K) avec un **CRI > 95**.
- **Contrôle :** PWM via MOSFET Canal-N (GPIO 4).
- **Montage :** Disposé en cercle à l'intérieur du boîtier (diamètre ~100mm) pour un éclairage latéral homogène.

### MOSFETs Recommandés (Compatibles 3.3V) :
| Modèle | Format | Courant Max | Avantages |
| :--- | :--- | :--- | :--- |
| **2N7000** | TO-92 | 200mA | Idéal pour petits segments COB. |
| **AO3400** | SOT-23 | 5.8A | Meilleur choix pour de longs rubans LEDs. |
| **IRLZ44N** | TO-220 | 47A | Très robuste, facile à souder à la main. |

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
