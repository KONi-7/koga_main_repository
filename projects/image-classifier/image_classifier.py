"""Train and use a small vegetable image classifier."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers


IMAGE_SIZE = (128, 128)
BATCH_SIZE = 32
SEED = 42
DEFAULT_MODEL_PATH = Path("vegetable_classifier.keras")
DEFAULT_LABELS_PATH = Path("labels.json")
DISPLAY_NAMES = {"ha": "白菜", "kya": "キャベツ", "re": "レタス"}


def load_dataset(directory: Path, *, shuffle: bool) -> tf.data.Dataset:
    """Load images from class-named subdirectories."""
    if not directory.is_dir():
        raise FileNotFoundError(f"データディレクトリが見つかりません: {directory}")

    return keras.utils.image_dataset_from_directory(
        directory,
        image_size=IMAGE_SIZE,
        batch_size=BATCH_SIZE,
        label_mode="int",
        shuffle=shuffle,
        seed=SEED,
    )


def build_model(class_count: int) -> keras.Model:
    """Build a compact CNN suitable for a small dataset."""
    augmentation = keras.Sequential(
        [
            layers.RandomFlip("horizontal"),
            layers.RandomRotation(0.08),
            layers.RandomZoom(0.1),
        ],
        name="augmentation",
    )

    model = keras.Sequential(
        [
            layers.Input(shape=(*IMAGE_SIZE, 3)),
            augmentation,
            layers.Rescaling(1.0 / 255),
            layers.Conv2D(32, 3, padding="same", activation="relu"),
            layers.MaxPooling2D(),
            layers.Conv2D(64, 3, padding="same", activation="relu"),
            layers.MaxPooling2D(),
            layers.Conv2D(128, 3, padding="same", activation="relu"),
            layers.MaxPooling2D(),
            layers.GlobalAveragePooling2D(),
            layers.Dropout(0.35),
            layers.Dense(class_count, activation="softmax"),
        ],
        name="vegetable_classifier",
    )
    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=1e-3),
        loss="sparse_categorical_crossentropy",
        metrics=["accuracy"],
    )
    return model


def train(args: argparse.Namespace) -> None:
    train_dataset = load_dataset(args.train, shuffle=True)
    valid_dataset = load_dataset(args.valid, shuffle=False)
    class_names = list(train_dataset.class_names)

    if list(valid_dataset.class_names) != class_names:
        raise ValueError("学習データと検証データのクラス構成が一致しません。")

    autotune = tf.data.AUTOTUNE
    train_dataset = train_dataset.prefetch(autotune)
    valid_dataset = valid_dataset.prefetch(autotune)

    model = build_model(len(class_names))
    args.model.parent.mkdir(parents=True, exist_ok=True)
    args.labels.parent.mkdir(parents=True, exist_ok=True)
    callbacks = [
        keras.callbacks.EarlyStopping(
            monitor="val_loss", patience=8, restore_best_weights=True
        ),
        keras.callbacks.ModelCheckpoint(
            str(args.model), monitor="val_loss", save_best_only=True
        ),
        keras.callbacks.ReduceLROnPlateau(
            monitor="val_loss", factor=0.5, patience=3, min_lr=1e-6
        ),
    ]

    model.fit(
        train_dataset,
        validation_data=valid_dataset,
        epochs=args.epochs,
        callbacks=callbacks,
    )

    args.labels.write_text(
        json.dumps(class_names, ensure_ascii=False, indent=2), encoding="utf-8"
    )

    if args.test is not None:
        test_dataset = load_dataset(args.test, shuffle=False)
        if list(test_dataset.class_names) != class_names:
            raise ValueError("学習データとテストデータのクラス構成が一致しません。")
        loss, accuracy = model.evaluate(test_dataset.prefetch(autotune), verbose=0)
        print(f"test loss: {loss:.4f}")
        print(f"test accuracy: {accuracy:.2%}")


def predict(args: argparse.Namespace) -> None:
    if not args.image.is_file():
        raise FileNotFoundError(f"画像が見つかりません: {args.image}")

    class_names = json.loads(args.labels.read_text(encoding="utf-8"))
    model = keras.models.load_model(args.model)
    image = keras.utils.load_img(args.image, target_size=IMAGE_SIZE)
    image_array = keras.utils.img_to_array(image)
    probabilities = model.predict(np.expand_dims(image_array, axis=0), verbose=0)[0]

    for class_name, probability in zip(class_names, probabilities, strict=True):
        display_name = DISPLAY_NAMES.get(class_name, class_name)
        print(f"{display_name}: {probability:.2%}")

    result_index = int(np.argmax(probabilities))
    result_name = DISPLAY_NAMES.get(class_names[result_index], class_names[result_index])
    print(f"予測結果: {result_name}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    train_parser = subparsers.add_parser("train", help="モデルを学習する")
    train_parser.add_argument("--train", type=Path, required=True)
    train_parser.add_argument("--valid", type=Path, required=True)
    train_parser.add_argument("--test", type=Path)
    train_parser.add_argument("--epochs", type=int, default=60)
    train_parser.add_argument("--model", type=Path, default=DEFAULT_MODEL_PATH)
    train_parser.add_argument("--labels", type=Path, default=DEFAULT_LABELS_PATH)
    train_parser.set_defaults(handler=train)

    predict_parser = subparsers.add_parser("predict", help="1枚の画像を分類する")
    predict_parser.add_argument("image", type=Path)
    predict_parser.add_argument("--model", type=Path, default=DEFAULT_MODEL_PATH)
    predict_parser.add_argument("--labels", type=Path, default=DEFAULT_LABELS_PATH)
    predict_parser.set_defaults(handler=predict)

    return parser.parse_args()


if __name__ == "__main__":
    arguments = parse_args()
    arguments.handler(arguments)
