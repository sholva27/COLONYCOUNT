import tensorflow as tf
from tensorflow.keras import layers, models
from tensorflow.keras.preprocessing.image import ImageDataGenerator
import os
import argparse
import subprocess

"""
Script d'entraînement pour ColonyCounter.
Usage: python tools/train_colony_model.py --data dataset_v1/cropped --epochs 20
"""

def build_model(input_shape=(128, 128, 3)):
    model = models.Sequential([
        layers.Conv2D(16, (3, 3), activation='relu', input_shape=input_shape),
        layers.MaxPooling2D((2, 2)),
        layers.Conv2D(32, (3, 3), activation='relu'),
        layers.MaxPooling2D((2, 2)),
        layers.Conv2D(64, (3, 3), activation='relu'),
        layers.Flatten(),
        layers.Dense(64, activation='relu'),
        layers.Dropout(0.5),
        layers.Dense(1, activation='sigmoid') # Binaire: LAB ou Autre
    ])

    model.compile(optimizer='adam',
                  loss='binary_crossentropy',
                  metrics=['accuracy'])
    return model

def train_model(data_dir, epochs=20, batch_size=32):
    # Data Augmentation (Crucial pour le petit nombre d'images initiales)
    train_datagen = ImageDataGenerator(
        rescale=1./255,
        rotation_range=180,
        width_shift_range=0.1,
        height_shift_range=0.1,
        horizontal_flip=True,
        vertical_flip=True,
        validation_split=0.2
    )

    train_generator = train_datagen.flow_from_directory(
        data_dir,
        target_size=(128, 128),
        batch_size=batch_size,
        class_mode='binary',
        subset='training'
    )

    validation_generator = train_datagen.flow_from_directory(
        data_dir,
        target_size=(128, 128),
        batch_size=batch_size,
        class_mode='binary',
        subset='validation'
    )

    model = build_model()
    model.summary()

    history = model.fit(
        train_generator,
        epochs=epochs,
        validation_data=validation_generator
    )

    # Sauvegarde du modèle Keras
    model.save('colony_model.h5')
    print("Modèle Keras sauvegardé sous 'colony_model.h5'")

    return model

def convert_to_tflite(keras_model_path, output_tflite_path):
    # Conversion vers TFLite
    model = tf.keras.models.load_model(keras_model_path)
    converter = tf.lite.TFLiteConverter.from_keras_model(model)

    # Optimisation pour la taille (quantization)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]

    tflite_model = converter.convert()

    with open(output_tflite_path, 'wb') as f:
        f.write(tflite_model)
    print(f"Modèle TFLite sauvegardé sous '{output_tflite_path}'")

    # Conversion vers C header pour ESP32
    try:
        # On utilise xxd pour générer le header C
        # Note: Cette commande suppose que xxd est installé (standard sur Linux/Mac)
        cmd = f"xxd -i {output_tflite_path} > model_data.h"
        subprocess.run(cmd, shell=True, check=True)
        print("Fichier header 'model_data.h' généré pour l'ESP32.")
    except Exception as e:
        print(f"Erreur lors de la génération du header C : {e}")
        print("Vous pouvez utiliser 'xxd -i colony_model.tflite > model_data.h' manuellement.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Entraînement de l'IA ColonyCounter")
    parser.add_argument("--data", required=True, help="Dossier contenant les classes 'LAB' et 'Other'")
    parser.add_argument("--epochs", type=int, default=20, help="Nombre d'époques d'entraînement")
    args = parser.parse_args()

    if not os.path.exists(args.data):
        print(f"Erreur: Dossier {args.data} introuvable.")
    else:
        model = train_model(args.data, epochs=args.epochs)
        convert_to_tflite('colony_model.h5', 'colony_model.tflite')
