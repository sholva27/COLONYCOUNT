# Debugging et Résolution de Problèmes

## 1. Problèmes Caméra et Boot
- **Boot Loop (Redémarrages en boucle) :** Si la LED rouge clignote sans cesse, vérifiez le **GPIO 12**. S'il est à l'état HAUT au démarrage (NeoPixel), le voltage flash passe à 1.8V. Débranchez le NeoPixel ou ajoutez une résistance de pull-down.
- **"Camera init failed with error 0x101" :** Souvent dû à un mauvais branchement ou un manque de puissance. Vérifiez le 5V.
- **Image violette ou bruitée :** Trop peu de lumière ou capteur défectueux.
- **Lignes horizontales :** Interférences électromagnétiques ou nappe de la caméra mal insérée.

## 2. Problèmes Carte SD
- **"SD Card Mount Failed" :**
    - Vérifiez que la carte est formatée en FAT32.
    - Assurez-vous que le mode 1-bit est bien activé dans le code.
    - GPIO 12 peut parfois causer des problèmes au boot s'il est tiré vers le haut (High).
- **Échec d'écriture :** Carte SD pleine ou protégée en écriture.

## 3. Problèmes d'Alimentation
- **Brownout Detector :** L'ESP32 redémarre lors de la capture ou du flash.
    - **DANGER :** Le Brownout Detector est activé dans le code par mesure de sécurité. Si vous n'avez pas une alimentation stable, la carte redémarrera pour protéger la carte SD de la corruption.
    - **SOLUTION IMPÉRATIVE :** Souder un condensateur électrolytique de **470 µF** (ou plus) directement entre les pins **5V** et **GND** de l'ESP32-CAM. Cela compense les pics de courant lors de l'activation du Flash et du Wi-Fi.

## 4. Problèmes NeoPixel (WS2812B)
- **Couleurs erronées ou clignotements :** Le signal 3.3V de l'ESP32 est limite pour les NeoPixels alimentés en 5V. Réduisez la tension d'alimentation des LEDs à 4.5V ou utilisez un level shifter.

## 5. Problèmes Bouton Tactile (TTP223)
- **Déclenchements intempestifs :** Sensibilité trop élevée ou câbles trop longs.
- **Multi-déclenchement (Double photo) :** Le TTP223 est un capteur capacitif qui peut mettre du temps à se "décharger" après un contact. Le code intègre une attente de relâchement explicite pour éviter cela.
- **Bouton ne répond pas :** Vérifiez que le GPIO 13 ne touche pas un autre composant de la carte SD.
