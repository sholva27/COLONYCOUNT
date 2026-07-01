# Calibration Spatiale et Calcul UFC

Pour calculer le nombre d'Unités Formant Colonie par millilitre (UFC/ml), le système doit convertir les pixels en millimètres réels.

## 1. Détermination de la constante PIXELS_PER_MM
La distance focale étant fixe dans le boîtier 3D, le rapport pixel/mm est constant.
- **Méthode :** Prenez une photo d'une règle graduée à la distance focale choisie.
- **Calcul :** `Nombre de pixels / Longueur en mm = PIXELS_PER_MM`.
- *Exemple :* Si 1000 pixels correspondent à 50mm, alors `PIXELS_PER_MM = 20`.

## 2. Calibration de la Distorsion (Méthode OpenCV)
Les lentilles de l'OV2640 introduisent une distorsion "en barillet".
- **Action :** Capturer une image d'un damier (Chessboard) OpenCV.
- **Correction :** Utiliser `cv2.calibrateCamera()` pour obtenir les paramètres intrinsèques et les coefficients de distorsion.
- **Impact :** Garantit que la taille mesurée d'une colonie au centre est la même que sur les bords de la boîte de Pétri.

## 3. Calcul de la surface d'une colonie
`Surface (mm²) = (Surface en pixels) / (PIXELS_PER_MM²)` (après correction de distorsion).

## 4. Calcul UFC/ml
1.  **Comptage (N) :** Nombre total de colonies détectées sur la boîte.
2.  **Facteur de Dilution (D) :** Exemple 10⁻⁶.
3.  **Volume Inoculé (V) :** Généralement 0.1 ml ou 1 ml.
4.  **Formule :** `UFC/ml = N / (V * D)`

## 5. Mire de Calibration de Référence
Il est recommandé d'inclure une mire de calibration (un carré noir de 10x10mm) sur le bord du support de la boîte de Pétri. Cela permet à l'IA de recalibrer automatiquement le rapport `PIXELS_PER_MM` à chaque capture pour compenser les légères variations mécaniques.
