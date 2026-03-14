#include <stdio.h>
#include <stdlib.h>

#define TRIALS 10

/* データ生成パラメータ */
#define IMG_H         16
#define IMG_W         16
#define CHANNELS      1
#define BASE_PATTERNS 5
#define VARIATIONS    10
#define BLOCKS        3

/* オートエンコーダ / 推論パラメータ */
#define INPUT_FILE  "X_study.dat"
#define NUM_LAYERS  3
#define LAYER_SIZES "256 64 2"

/* 評価パラメータ */
#define EVAL_INPUT  "X_study.dat"
#define EVAL_Y      "256_64_y.dat"
#define NUM_SAMPLES 25

static int run(const char *cmd)
{
    int ret = system(cmd);
    if (ret != 0)
        fprintf(stderr, "コマンド失敗: %s\n", cmd);
    return ret;
}

int main(void)
{
    char cmd[1024];

    /* 前回の結果をクリア */
    remove("eval_sse.dat");
    remove("eval_accuracy.dat");

    printf("=== 実験開始: %d 回繰り返し ===\n\n", TRIALS);

    for (int t = 1; t <= TRIALS; t++) {
        printf("===== Trial %d / %d =====\n", t, TRIALS);

        /* データ生成 */
        snprintf(cmd, sizeof(cmd),
            "echo '%d\n%d\n%d\n%d\n%d\n%d' | ./hierarchical_datagen",
            IMG_H, IMG_W, CHANNELS, BASE_PATTERNS, VARIATIONS, BLOCKS);
        if (run(cmd)) return 1;

        /* 事前学習 */
        snprintf(cmd, sizeof(cmd),
            "echo '%s\n%d\n%s' | ./hierarchical_autoencoder",
            INPUT_FILE, NUM_LAYERS, LAYER_SIZES);
        if (run(cmd)) return 1;

        /* 推論 */
        snprintf(cmd, sizeof(cmd),
            "echo '%s\n%d\n%s' | ./hierarchical_inference",
            INPUT_FILE, NUM_LAYERS, LAYER_SIZES);
        if (run(cmd)) return 1;

        /* 評価 */
        snprintf(cmd, sizeof(cmd),
            "echo '%d\n%d\n%d\n%s\n%s\n%d' | ./hierarchical_eval",
            IMG_H, IMG_W, NUM_SAMPLES, EVAL_INPUT, EVAL_Y, t);
        if (run(cmd)) return 1;

        printf("\n");
    }

    printf("=== 実験完了 ===\n");
    printf("eval_sse.dat      : 各試行の平均SSE\n");
    printf("eval_accuracy.dat : 各試行の平均一致率(%%)\n");

    return 0;
}
