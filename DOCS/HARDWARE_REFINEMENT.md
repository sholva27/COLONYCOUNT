# Raffinement Matériel (Hardware) - V3

## 1. Structure et Gabarit de Positionnement
Le succès de l'analyse repose sur la reproductibilité.
- **Gabarit 3D :** Un support imprimé en 3D avec un logement de **90mm** pour la boîte de Pétri est indispensable.
- **Distance Fixe :** La caméra doit être maintenue à une distance focale constante pour que la calibration spatiale reste valide.

## 2. Architecture de l'ESP32-S3 (Solution Structurelle)
La migration vers l'**ESP32-S3** règle les problèmes de bus I2C :
- **Bus Indépendants :** Assignation de bus I2C distincts pour la Caméra (SCCB) et les Périphériques (BME280/RTC).
- **Inférence SIMD :** Performance IA décuplée (+15x).

## 3. Gestion d'Alimentation et Portabilité
Pour un fonctionnement sur batterie en incubateur (Phase 5) :
- **Interrupteur de charge :** Utilisation d'un MOSFET P-Channel pour couper l'alimentation du NeoPixel, du BME280 et du RTC pendant le Deep Sleep.
- **Circuit de charge :** Intégration d'un module TP4056 et d'une batterie LiPo 18650.

## 4. Synchronisation Multi-Appareils
- **Référence Temporelle :** Tous les modules RTC DS3231 doivent être réglés sur la même source (PC) avant le déploiement pour garantir la cohérence des timestamps dans le dataset fusionné.
- **Uniformité Lumineuse :** Calibration de l'intensité PWM (GPIO 4) pour que le rendu colorimétrique soit identique entre les unités.
