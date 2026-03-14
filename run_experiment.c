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

static double read_value(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp) { fprintf(stderr, "Cannot open %s\n", path); return 0.0; }
    double v = 0.0;
    fscanf(fp, "%lf", &v);
    fclose(fp);
    return v;
}

int main(void)
{
    char cmd[1024];
    double sse_list[TRIALS];
    double acc_list[TRIALS];

    printf("=== 実験開始: %d 回繰り返し ===\n\n", TRIALS);

    for (int t = 0; t < TRIALS; t++) {
        printf("===== Trial %d / %d =====\n", t + 1, TRIALS);

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
            "echo '%d\n%d\n%d\n%s\n%s' | ./hierarchical_eval",
            IMG_H, IMG_W, NUM_SAMPLES, EVAL_INPUT, EVAL_Y);
        if (run(cmd)) return 1;

        /* 結果を読み取り */
        sse_list[t] = read_value("eval_sse.dat");
        acc_list[t] = read_value("eval_accuracy.dat");
        printf("  -> SSE = %f, accuracy = %.2f%%\n\n", sse_list[t], acc_list[t]);
    }

    /* 各試行の値を保存 */
    FILE *fp;

    fp = fopen("eval_sse.dat", "w");
    if (!fp) { fprintf(stderr, "Cannot open eval_sse.dat\n"); return 1; }
    for (int t = 0; t < TRIALS; t++)
        fprintf(fp, "%d %f\n", t + 1, sse_list[t]);
    fclose(fp);

    fp = fopen("eval_accuracy.dat", "w");
    if (!fp) { fprintf(stderr, "Cannot open eval_accuracy.dat\n"); return 1; }
    for (int t = 0; t < TRIALS; t++)
        fprintf(fp, "%d %f\n", t + 1, acc_list[t]);
    fclose(fp);

    /* 平均を計算して保存 */
    double sum_sse = 0.0, sum_acc = 0.0;
    for (int t = 0; t < TRIALS; t++) {
        sum_sse += sse_list[t];
        sum_acc += acc_list[t];
    }
    double avg_sse = sum_sse / TRIALS;
    double avg_acc = sum_acc / TRIALS;

    fp = fopen("eval_sse_avg.dat", "w");
    if (!fp) { fprintf(stderr, "Cannot open eval_sse_avg.dat\n"); return 1; }
    fprintf(fp, "%f\n", avg_sse);
    fclose(fp);

    fp = fopen("eval_accuracy_avg.dat", "w");
    if (!fp) { fprintf(stderr, "Cannot open eval_accuracy_avg.dat\n"); return 1; }
    fprintf(fp, "%f\n", avg_acc);
    fclose(fp);

    printf("=== 実験完了 ===\n");
    printf("eval_sse.dat          : 各試行のSSE (trial value)\n");
    printf("eval_accuracy.dat     : 各試行の一致率 (trial value)\n");
    printf("eval_sse_avg.dat      : %d回平均SSE = %f\n", TRIALS, avg_sse);
    printf("eval_accuracy_avg.dat : %d回平均一致率 = %.2f%%\n", TRIALS, avg_acc);

    return 0;
}
