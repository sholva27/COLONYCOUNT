# Debugging et Résolution de Problèmes

## 1. Problèmes Caméra
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
    - **Solution :** Ajouter un condensateur de 100uF ou 470uF entre 5V et GND.
    - **Contournement :** `WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);` (déjà dans le code).

## 4. Problèmes Bouton Tactile (TTP223)
- **Déclenchements intempestifs :** Sensibilité trop élevée ou câbles trop longs.
- **Bouton ne répond pas :** Vérifiez que le GPIO 13 ne touche pas un autre composant de la carte SD.
