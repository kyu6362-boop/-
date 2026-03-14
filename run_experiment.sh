#!/bin/bash
# 事前学習→推論→評価を10回繰り返す実験スクリプト
#
# 使い方:
#   bash run_experiment.sh
#
# 事前に以下の変数を環境に合わせて変更してください。

# --- 設定 ---
TRIALS=10

# データ生成パラメータ
IMG_H=16
IMG_W=16
CHANNELS=1
BASE_PATTERNS=5
VARIATIONS=10
BLOCKS=3

# オートエンコーダ / 推論パラメータ
INPUT_FILE="X_study.dat"
NUM_LAYERS=3
LAYER_SIZES="256 64 2"

# 評価パラメータ
EVAL_INPUT="X_study.dat"
EVAL_Y="256_64_y.dat"
NUM_SAMPLES=25

# --- 前回の結果をクリア ---
rm -f eval_sse.dat eval_accuracy.dat

echo "=== 実験開始: ${TRIALS} 回繰り返し ==="
echo ""

for t in $(seq 1 $TRIALS); do
    echo "===== Trial $t / $TRIALS ====="

    # データ生成
    echo "$IMG_H
$IMG_W
$CHANNELS
$BASE_PATTERNS
$VARIATIONS
$BLOCKS" | ./hierarchical_datagen

    # 事前学習
    echo "$INPUT_FILE
$NUM_LAYERS
$LAYER_SIZES" | ./hierarchical_autoencoder

    # 推論
    echo "$INPUT_FILE
$NUM_LAYERS
$LAYER_SIZES" | ./hierarchical_inference

    # 評価
    echo "$IMG_H
$IMG_W
$NUM_SAMPLES
$EVAL_INPUT
$EVAL_Y
$t" | ./hierarchical_eval

    echo ""
done

echo "=== 実験完了 ==="
echo "eval_sse.dat      : 各試行の平均SSE"
echo "eval_accuracy.dat : 各試行の平均一致率(%)"
