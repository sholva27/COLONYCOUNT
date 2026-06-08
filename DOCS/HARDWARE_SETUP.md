# Configuration Matérielle - Compteur de Colonies ESP32-CAM

## 1. Brochage (Pinout)

| Composant | Pin ESP32 | Description |
| :--- | :--- | :--- |
| **Bouton TTP223** | GPIO 13 | Signal d'entrée (Digital Input). |
| **Carte SD (Mode 1-bit)** | GPIO 2, 14, 15 | Utilisé pour l'enregistrement des images. |
| **Flash Intégré** | GPIO 4 | Peut être utilisé pour l'éclairage de capture. |
| **Caméra OV2640** | (Standard) | Pins standard de l'ESP32-CAM AI-Thinker. |

## 2. Note sur le mode SD 1-bit
Pour libérer le **GPIO 13** (souvent utilisé par la carte SD en mode 4-bit), nous utilisons le mode **1-bit**.
- **Impact sur la qualité :** Aucun. La qualité de l'image dépend uniquement du capteur OV2640 et des paramètres de compression JPEG.
- **Impact sur la vitesse :** L'enregistrement d'une photo prendra environ 100-200ms de plus qu'en 4-bit, ce qui est négligeable pour cette application.

## 3. Montage du Bouton TTP223
- **VCC :** 3.3V ou 5V (selon le module)
- **GND :** GND
- **SIG :** GPIO 13
- **Configuration :** Le cavalier (jumper) sur le TTP223 doit être configuré pour une sortie active au repos (ou gérer l'inversion dans le code Arduino).
