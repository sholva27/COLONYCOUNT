import os
import shutil
import argparse

def prepare_data(source_dir, output_dir):
    """
    Organise les images capturées par l'ESP32-CAM pour l'entraînement.
    Crée une structure de dossiers compatible avec la plupart des frameworks d'IA.
    """
    categories = ['lactique', 'contaminant', 'a_trier']

    for cat in categories:
        os.makedirs(os.path.join(output_dir, cat), exist_ok=True)

    print(f"Structure de dossiers créée dans : {output_dir}")
    print("Veuillez déplacer vos images de '/img' vers les dossiers correspondants.")

    if os.path.exists(source_dir):
        files = [f for f in os.listdir(source_dir) if f.endswith('.jpg')]
        for f in files:
            # Par défaut on met tout dans 'a_trier'
            shutil.copy(os.path.join(source_dir, f), os.path.join(output_dir, 'a_trier', f))
        print(f"{len(files)} images copiées dans 'a_trier'.")
    else:
        print(f"Note: Le dossier source {source_dir} n'a pas été trouvé. Branchez votre carte SD.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Préparation des données pour l'IA ColonyCounter")
    parser.add_argument("--source", default="img", help="Dossier contenant les images de la carte SD")
    parser.add_argument("--output", default="dataset", help="Dossier de destination pour le dataset")

    args = parser.parse_args()
    prepare_data(args.source, args.output)
