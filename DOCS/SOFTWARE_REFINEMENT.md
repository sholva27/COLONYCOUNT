# Raffinement Logiciel (Software) - V2

## 1. Gestion des données (Journalisation)
Le système enregistre désormais un fichier `data.csv` à la racine de la carte SD à chaque capture.
- **Champs :** ID de l'image, Température (°C), Humidité (%), Pression (hPa), Intensité Flash (0-255).
- **Utilité :** Permet de corréler l'aspect des colonies avec les conditions de croissance.

## 2. Retour visuel par NeoPixel
L'anneau de LED sert d'interface utilisateur :
- **Orange fixe :** Démarrage et initialisation des périphériques.
- **Vert fixe (1s) :** Prêt pour la capture.
- **Blanc brillant :** Capture en cours (éclairage d'appoint).
- **Bleu flash :** Écriture sur carte SD réussie.
- **Rouge fixe :** Erreur critique (Carte SD absente ou Caméra HS).

## 3. Optimisations PWM
L'intensité du flash est réglée par défaut à 128/255 pour éviter l'éblouissement du capteur OV2640 sur le milieu MRS clair. Cette valeur peut être ajustée dans la variable `flashBrightness` du code.

## 4. Algorithmes de Vision (Inspiration ImageJ)
Pour le comptage sur ESP32, nous privilégierons une approche hybride :
1.  **Soustraction de fond locale :** Pour compenser l'opacité variable du milieu MRS.
2.  **Seuillage d'Otsu :** Auto-adaptation à la luminosité de la capture.
3.  **Filtrage par taille/circularité :** Comme le fait l'outil "Analyze Particles" d'ImageJ pour éliminer les poussières et les bulles d'air.
