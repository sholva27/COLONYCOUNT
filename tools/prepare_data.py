import os
import shutil
import pandas as pd
from datetime import datetime
import argparse
from PIL import Image

def verify_and_prepare(sd_path, output_path):
    """
    Vérifie l'intégrité des images, fusionne avec les métadonnées CSV
    et organise le dataset en préservant la traçabilité temporelle et environnementale.
    """
    img_dir = os.path.join(sd_path, "img")
    csv_path = os.path.join(sd_path, "data.csv")

    if not os.path.exists(img_dir):
        print(f"❌ Erreur: Dossier d'images non trouvé dans {sd_path}")
        return

    # Chargement des métadonnées (ID -> Timestamp, Temp, etc.)
    metadata = {}
    if os.path.exists(csv_path):
        try:
            df = pd.read_csv(csv_path)
            # Conversion de l'ID en string pour correspondre au nom de fichier
            df['ID'] = df['ID'].astype(str)
            metadata = df.set_index('ID').to_dict('index')
            print(f"📊 {len(df)} entrées de métadonnées chargées depuis data.csv.")
        except Exception as e:
            print(f"⚠️ Impossible de lire data.csv: {e}")

    # Création des dossiers de sortie
    categories = ['lactique', 'contaminant', 'a_trier']
    for cat in categories:
        os.makedirs(os.path.join(output_path, cat), exist_ok=True)

    count = 0
    corrupted = 0

    files = [f for f in os.listdir(img_dir) if f.endswith('.jpg')]
    print(f"🔍 Analyse de {len(files)} images...")

    processed_data = []

    for filename in files:
        # Extraction de l'ID depuis 'colony_X.jpg'
        file_id = filename.replace('colony_', '').replace('.jpg', '')

        try:
            full_path = os.path.join(img_dir, filename)

            # 1. Vérification d'intégrité JPEG
            with Image.open(full_path) as img:
                img.verify()

            # 2. Récupération des métadonnées réelles du capteur
            img_meta = metadata.get(file_id, {})
            sensor_ts = img_meta.get('Timestamp', '0000-00-00_00-00-00')
            # Nettoyage du timestamp pour le nom de fichier
            clean_ts = str(sensor_ts).replace(' ', '_').replace(':', '-')

            new_name = f"COL_{file_id}_{clean_ts}.jpg"

            # 3. Copie vers a_trier par défaut
            dest_path = os.path.join(output_path, 'a_trier', new_name)
            shutil.copy2(full_path, dest_path)

            # 4. Enregistrement pour le CSV récapitulatif
            img_meta['Original_Filename'] = filename
            img_meta['New_Filename'] = new_name
            processed_data.append(img_meta)

            count += 1

        except (IOError, SyntaxError) as e:
            print(f"💀 Image corrompue détectée : {filename}")
            corrupted += 1

    # Sauvegarde d'un index propre du dataset
    if processed_data:
        dataset_index = pd.DataFrame(processed_data)
        dataset_index.to_csv(os.path.join(output_path, "dataset_index.csv"), index=False)
        print(f"📝 Index du dataset créé : {output_path}/dataset_index.csv")

    print(f"\n✅ Terminé !")
    print(f"   - Images traitées : {count}")
    print(f"   - Images corrompues : {corrupted}")
    print(f"   - Destination : {output_path}/a_trier/")
    print("\nTraçabilité : Le nom de fichier contient maintenant le timestamp RÉEL du capteur.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="ColonyCounter Dataset Builder")
    parser.add_argument("--sd", required=True, help="Chemin vers la racine de la carte SD")
    parser.add_argument("--out", default="dataset_v1", help="Dossier de sortie pour le dataset")

    args = parser.parse_args()
    verify_and_prepare(args.sd, args.out)
