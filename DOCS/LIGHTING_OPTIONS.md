# Options d'éclairage pour le comptage de colonies

Ce document répertorie les différentes stratégies d'éclairage pour la capture d'images de colonies bactériennes sur milieu MRS.

## 1. Éclairage Latéral (Side Lighting) - *Choisie pour la Phase 1*
**Description :** Des barrettes de LEDs ou des sources lumineuses placées sur les côtés de la boîte de Pétri.
- **Avantages :**
    - Crée du contraste sur la texture de la colonie.
    - Préserve les couleurs réelles.
    - Limite les reflets directs vers l'objectif si l'angle est bien choisi.
- **Inconvénients :**
    - Peut créer des ombres portées qui pourraient être confondues avec des colonies par l'IA.

## 2. Rétro-éclairage (Backlight)
**Description :** Source lumineuse diffuse placée sous la boîte de Pétri.
- **Avantages :**
    - Silhouette parfaite des colonies.
    - Idéal pour le simple comptage (détection de présence).
- **Inconvénients :**
    - Perte totale des informations de couleur et de relief.
    - Difficile de distinguer les contaminants des colonies lactiques si la différence est basée sur l'aspect de surface.

## 3. Éclairage Annulaire (Ring Light)
**Description :** Un anneau de LEDs entourant l'objectif de la caméra.
- **Avantages :**
    - Éclairage très uniforme.
    - Excellente reproduction des détails.
- **Inconvénients :**
    - Risque élevé de reflets (points blancs) sur l'agar ou le couvercle en plastique, ce qui perturbe l'IA.

## 4. Éclairage indirect (Dôme diffusant)
**Description :** La boîte est placée sous un dôme blanc éclairé de l'intérieur, la caméra regarde par un trou au sommet.
- **Avantages :**
    - Qualité professionnelle.
    - Aucune ombre portée ni reflet.
- **Inconvénients :**
    - Plus complexe et encombrant à fabriquer.
