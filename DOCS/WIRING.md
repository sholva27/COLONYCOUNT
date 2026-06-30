# Schéma de Câblage (Wiring)

## Vue d'ensemble des connexions

```text
       +-----------------------+
       |       ESP32-CAM       |
       |                       |
  5V  <--[ 5V ]      [ GND ]-- GND
       |                       |
GPIO13 <--[ IO13 ]    [ IO12 ]--> NeoPixel DIN
       | (TTP223)              | (Pull-down 10k)
       |                       |
GPIO4  <--[ IO4 ]     [ IO2 ]-- (SD Data 0)
       | (Flash PWM)           |
       |                       |
GPIO26 <--[ IO26 ]    [ IO27 ]--> I2C SCL (BME280)
       | (I2C SDA)             |
       +-----------------------+
```

## Détails des Pins

1. **Bouton TTP223 :**
   - VCC -> 5V
   - GND -> GND
   - SIG -> **GPIO 13**
2. **Capteur BME280 :**
   - VCC -> 3.3V
   - GND -> GND
   - SDA -> **GPIO 26**
   - SCL -> **GPIO 27**
3. **NeoPixel Ring :**
   - VCC -> 5V
   - GND -> GND
   - DIN -> **GPIO 12** (⚠️ Ajouter résistance 10k vers GND)
4. **Condensateur :**
   - Connecter entre le pin **5V** et le pin **GND** de l'ESP32-CAM.
