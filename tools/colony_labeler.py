import cv2
import os
import numpy as np
import argparse

"""
Outil d'étiquetage pour ColonyCounter.
Usage: python tools/colony_labeler.py --input dataset_v1/a_trier --output dataset_v1/cropped

Instructions:
- Clic GAUCHE : Marquer comme Colonie Lactique (LAB) -> Cadre Vert
- Clic DROIT : Marquer comme Autre/Contaminant (Other) -> Cadre Rouge
- Touche 'n' : Image Suivante
- Touche 'q' : Quitter
"""

class ColonyLabeler:
    def __init__(self, input_dir, output_dir, crop_size=128):
        self.input_dir = input_dir
        self.output_dir = output_dir
        self.crop_size = crop_size
        self.half_crop = crop_size // 2

        self.lab_dir = os.path.join(output_dir, 'LAB')
        self.other_dir = os.path.join(output_dir, 'Other')

        os.makedirs(self.lab_dir, exist_ok=True)
        os.makedirs(self.other_dir, exist_ok=True)

        self.image_files = [f for f in os.listdir(input_dir) if f.lower().endswith(('.jpg', '.jpeg', '.png'))]
        self.image_files.sort()
        self.current_idx = 0
        self.img = None
        self.display_img = None

    def save_crop(self, x, y, label):
        # Découpe une zone de 128x128 centrée sur le clic
        y1, y2 = y - self.half_crop, y + self.half_crop
        x1, x2 = x - self.half_crop, x + self.half_crop

        # Gestion des bords de l'image (padding noir si nécessaire)
        h, w = self.img.shape[:2]

        crop_y1, crop_y2 = max(0, y1), min(h, y2)
        crop_x1, crop_x2 = max(0, x1), min(w, x2)

        crop = self.img[crop_y1:crop_y2, crop_x1:crop_x2]

        if crop.shape[0] != self.crop_size or crop.shape[1] != self.crop_size:
            pad_y1 = max(0, -y1)
            pad_y2 = max(0, y2 - h)
            pad_x1 = max(0, -x1)
            pad_x2 = max(0, x2 - w)
            crop = cv2.copyMakeBorder(crop, pad_y1, pad_y2, pad_x1, pad_x2, cv2.BORDER_CONSTANT, value=[0, 0, 0])

        filename = f"{os.path.splitext(self.image_files[self.current_idx])[0]}_x{x}_y{y}.jpg"
        target_dir = self.lab_dir if label == 'LAB' else self.other_dir
        cv2.imwrite(os.path.join(target_dir, filename), crop)
        print(f"Enregistré {label} : {filename}")

    def mouse_callback(self, event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            self.save_crop(x, y, 'LAB')
            cv2.rectangle(self.display_img, (x-5, y-5), (x+5, y+5), (0, 255, 0), 2)
            cv2.imshow("Colony Labeler", self.display_img)
        elif event == cv2.EVENT_RBUTTONDOWN:
            self.save_crop(x, y, 'Other')
            cv2.rectangle(self.display_img, (x-5, y-5), (x+5, y+5), (0, 0, 255), 2)
            cv2.imshow("Colony Labeler", self.display_img)

    def run(self):
        if not self.image_files:
            print(f"Aucune image trouvée dans {self.input_dir}")
            return

        cv2.namedWindow("Colony Labeler")
        cv2.setMouseCallback("Colony Labeler", self.mouse_callback)

        print("\n--- Aide ---")
        print("Clic Gauche : LAB (Vert)")
        print("Clic Droit  : Autre (Rouge)")
        print("'n' : Image Suivante")
        print("'q' : Quitter")

        while self.current_idx < len(self.image_files):
            img_path = os.path.join(self.input_dir, self.image_files[self.current_idx])
            self.img = cv2.imread(img_path)
            if self.img is None:
                print(f"Erreur de lecture : {img_path}")
                self.current_idx += 1
                continue

            self.display_img = self.img.copy()
            # Affichage du nom de fichier sur l'image
            cv2.putText(self.display_img, f"{self.current_idx+1}/{len(self.image_files)}: {self.image_files[self.current_idx]}",
                        (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

            while True:
                cv2.imshow("Colony Labeler", self.display_img)
                key = cv2.waitKey(20) & 0xFF
                if key == ord('n'):
                    self.current_idx += 1
                    break
                elif key == ord('q'):
                    print("Sortie...")
                    cv2.destroyAllWindows()
                    return

        print("Fini ! Toutes les images ont été traitées.")
        cv2.destroyAllWindows()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Outil d'étiquetage pour ColonyCounter")
    parser.add_argument("--input", required=True, help="Dossier contenant les images à trier")
    parser.add_argument("--output", default="dataset_v1/cropped", help="Dossier de sortie pour les crops")
    args = parser.parse_args()

    labeler = ColonyLabeler(args.input, args.output)
    labeler.run()
