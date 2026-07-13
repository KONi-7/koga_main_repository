# 野菜画像分類

## 作品概要

白菜・キャベツ・レタスの3種類を分類する画像認識作品です。CNNで画像の特徴を学習し、入力画像について各クラスの予測確率と判定結果を出力します。元の実験結果は `vegetable_classifier_experiment.ipynb` に残し、再実行しやすい学習・推論処理を `image_classifier.py` に整理しています。

## 使用技術

- **Python**: データ読み込み、学習、評価、推論処理
- **TensorFlow / Keras**: CNNの構築、学習、モデル保存
- **NumPy**: 画像配列と予測結果の処理
- **OpenCV**: 元Notebookでの画像読込、色変換、リサイズ
- **Matplotlib**: 入力画像と学習結果の可視化
- **Jupyter Notebook / Google Colab**: モデルの実験と実行結果の記録

## 主な実装

- 128×128ピクセルのRGB画像を入力
- 回転、反転、ズームによるデータ拡張
- 畳み込み層による画像特徴の抽出
- 白菜・キャベツ・レタスの3クラス分類
- Early StoppingとModel Checkpointによる過学習対策
- 学習済みモデルを使った1枚画像の推論

## 実験結果

- 学習画像: 234枚
- 検証画像: 66枚
- テスト画像: 77枚
- 元ノートブックのテスト正解率: 約76.6%

小規模なデータセットのため、この数値だけで実環境での性能を保証するものではありません。

## 改善した点

- データのパスをコマンドライン引数で指定可能
- 回転・反転・ズームによるデータ拡張
- `GlobalAveragePooling2D` を使い、元モデルよりパラメータ数を削減
- Early Stoppingによる過学習対策
- 最良モデルとクラス名を保存
- 学習、評価、1枚の画像推論を同じスクリプトから実行可能

## データ配置

各ディレクトリの直下にクラス別フォルダを作成します。

```text
data/
  train/
    ha/
    kya/
    re/
  valid/
    ha/
    kya/
    re/
  test/
    ha/
    kya/
    re/
```

## 実行方法

```bash
python -m venv .venv
python -m pip install -r requirements.txt
python image_classifier.py train --train data/train --valid data/valid --test data/test
python image_classifier.py predict sample.jpg
```

学習後は `vegetable_classifier.keras` と `labels.json` が生成されます。データセットは容量と権利関係を確認した上で別途用意してください。
