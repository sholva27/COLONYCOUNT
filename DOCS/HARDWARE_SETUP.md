# Configuration Matérielle - Compteur de Colonies ESP32-CAM (V2)

## 1. Brochage Mis à Jour (Advanced Pinout)

| Composant | Pin ESP32 | Description |
| :--- | :--- | :--- |
| **Bouton TTP223** | GPIO 13 | Déclenchement capture (Signal Actif à l'état BAS / Active LOW). |
| **Flash Intégré** | GPIO 4 | Contrôlé en **PWM** pour ajuster la luminosité. |
| **NeoPixel Ring** | GPIO 12 | Éclairage annulaire RGB et retour d'état visuel. |
| **Capteur BME280** | GPIO 26 (SDA) / 27 (SCL) | Température, Humidité, Pression (I2C). |
| **Carte SD (1-bit)** | GPIO 2, 14, 15 | Stockage images et logs CSV. |

## 2. Détails techniques
- **I2C :** Le bus I2C est partagé avec les signaux SIOD/SIOC de la caméra. Cela fonctionne car les adresses sont distinctes.
- **PWM Flash :** Fréquence de 5kHz, résolution 8-bit. Permet d'éviter la surexposition des colonies blanches.
- **NeoPixel :** Utilisé pour le retour d'état (Vert: Prêt, Orange: Init, Bleu: Enregistrement, Rouge: Erreur).

## 3. Schéma de connexion
- **NeoPixel VCC :** 5V (recommandé pour la luminosité).
- **BME280 VCC :** 3.3V.
- **Bouton SIG :** GPIO 13 (Le TTP223 doit être configuré en mode 'Active LOW' car GPIO 13 possède une résistance de pull-up interne sur l'ESP32-CAM).
