# Recherche Approfondie - Microbiologie et IA

## 1. Signatures Optiques des Colonies sur MRS
Le milieu MRS est riche en nutriments mais son opacité peut varier selon la concentration en agar et en extraits de levure.
- **Bactéries Lactiques (LAB) :** Elles ont tendance à être semi-translucides. Sous un éclairage latéral, elles créent une "diffraction de bord" caractéristique. L'IA doit apprendre à détecter ce halo.
- **Réfraction :** Les colonies bombées agissent comme des micro-lentilles. Un éclairage annulaire (Ring Light) crée souvent un point brillant central. C'est pourquoi le filtre polarisant (proposé en hardware) est crucial pour voir la *texture* réelle sous le reflet.

## 2. Cinétique de Croissance
- **LAB :** Croissance modérée. Temps de doublement typique : 40-90 min. Les colonies apparaissent généralement entre 24h et 48h à 30-37°C.
- **Contaminants (Moisissures/Bacillus) :** Croissance exponentielle beaucoup plus rapide. En 24h, une moisissure peut recouvrir 10% de la boîte.
- **Application IA :** Si on utilise le mode "Time-lapse" (photo toutes les heures), l'IA peut distinguer un contaminant d'une LAB simplement par sa **vitesse d'expansion radiale**.

## 3. Profils Colorimétriques
Sur MRS standard (brun clair) :
- **Lactobacilles :** Blanc crémeux à jaune très pâle.
- **Contaminants fongiques :** Souvent gris, verts ou noirs au centre (spores).
- **Indicateurs :** Si on ajoute du bleu de bromocrésol au milieu, les LAB (acidifiantes) feront virer le milieu au jaune. L'IA pourra alors segmenter les zones de changement de couleur.

## 4. État de l'Art (Automated Counting)
Les systèmes professionnels (type Scan1200) utilisent :
1.  Une caméra haute résolution (5MP+).
2.  Un éclairage sombre (Darkfield) pour faire ressortir les colonies claires sur fond noir.
*Notre projet avec ESP32-CAM (2MP) tente de reproduire cela à bas coût en optimisant l'angle d'éclairage latéral.*

## 5. Fidélité des Couleurs et Importance du CRI
La distinction entre les colonies lactiques (souvent blanc crémeux ou translucides) et les contaminants (gris, vert, jaune) repose sur une analyse colorimétrique fine.
- **Le problème du RGB :** Les LEDs NeoPixel (WS2812B) ont un spectre de lumière haché avec un **IRC (CRI) faible (~70)**. Cela signifie qu'elles ne restituent pas fidèlement les nuances de jaune et de blanc, ce qui peut induire l'IA en erreur.
- **La solution High-CRI :** L'utilisation d'une source lumineuse blanche avec un **CRI > 95** (spectre continu) garantit que les couleurs capturées par l'OV2640 sont conformes à la réalité biologique, facilitant grandement le travail de classification du réseau de neurones.

## 6. Défis de Segmentation
- **Colonies fusionnées (Clustering) :** Quand deux colonies se touchent, l'algorithme doit être capable d'identifier deux centres distincts (utilisation possible de l'algorithme de Watershed).
- **Artéfacts :** Les rayures sur le plastique de la boîte de Pétri ou les condensations d'eau sur le couvercle sont les principaux ennemis de l'IA.
