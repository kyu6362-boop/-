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
#define EVAL_STUDY_X  "X_study.dat"
#define EVAL_UNSEEN_X "X_unseen.dat"
#define EVAL_Y_STUDY  "study_256_64_y.dat"
#define EVAL_Y_UNSEEN "unseen_256_64_y.dat"
#define NUM_SAMPLES   25

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

static void save_trials(const char *path, double *list, int n)
{
    FILE *fp = fopen(path, "w");
    if (!fp) { fprintf(stderr, "Cannot open %s\n", path); return; }
    for (int t = 0; t < n; t++)
        fprintf(fp, "%d %f\n", t + 1, list[t]);
    fclose(fp);
}

static void save_avg(const char *path, double *list, int n)
{
    double sum = 0.0;
    for (int t = 0; t < n; t++) sum += list[t];
    FILE *fp = fopen(path, "w");
    if (!fp) { fprintf(stderr, "Cannot open %s\n", path); return; }
    fprintf(fp, "%f\n", sum / n);
    fclose(fp);
}

int main(void)
{
    char cmd[1024];
    double study_sse[TRIALS],  study_acc[TRIALS];
    double unseen_sse[TRIALS], unseen_acc[TRIALS];

    printf("=== データ生成 ===\n");

    /* データ生成（1回だけ、ループ外で固定） */
    snprintf(cmd, sizeof(cmd),
        "echo '%d\n%d\n%d\n%d\n%d\n%d' | ./hierarchical_datagen",
        IMG_H, IMG_W, CHANNELS, BASE_PATTERNS, VARIATIONS, BLOCKS);
    if (run(cmd)) return 1;

    printf("\n=== 実験開始: %d 回繰り返し（データ固定・初期値のみ変更） ===\n\n",
           TRIALS);

    for (int t = 0; t < TRIALS; t++) {
        printf("===== Trial %d / %d =====\n", t + 1, TRIALS);

        /* 事前学習（毎回ランダム初期値で再学習） */
        snprintf(cmd, sizeof(cmd),
            "echo '%s\n%d\n%d\n%s' | ./hierarchical_autoencoder",
            INPUT_FILE, NUM_SAMPLES, NUM_LAYERS, LAYER_SIZES);
        if (run(cmd)) return 1;

        /* 最終試行: 事前学習後の潜在空間をプロット */
        if (t == TRIALS - 1) {
            run("gnuplot -e \"zfile='64_2_z.dat'; outfile='z_scatter_pretrain.eps'\" plot_z.pl");
            printf("  -> z_scatter_pretrain.eps を出力\n");
        }

        /* 学習データで推論 (プレフィックス: study) */
        snprintf(cmd, sizeof(cmd),
            "echo '%s\nstudy\n%d\n%d\n%s' | ./hierarchical_inference",
            EVAL_STUDY_X, NUM_SAMPLES, NUM_LAYERS, LAYER_SIZES);
        if (run(cmd)) return 1;

        /* 最終試行: 推論後の潜在空間をプロット */
        if (t == TRIALS - 1) {
            run("gnuplot -e \"zfile='study_64_2_z.dat'; outfile='z_scatter_inference.eps'\" plot_z.pl");
            printf("  -> z_scatter_inference.eps を出力\n");
        }

        /* 学習データの評価 */
        snprintf(cmd, sizeof(cmd),
            "echo '%d\n%d\n%d\n%s\n%s' | ./hierarchical_eval",
            IMG_H, IMG_W, NUM_SAMPLES, EVAL_STUDY_X, EVAL_Y_STUDY);
        if (run(cmd)) return 1;

        study_sse[t] = read_value("eval_sse.dat");
        study_acc[t] = read_value("eval_accuracy.dat");
        printf("  [学習]   SSE = %f, accuracy = %.2f%%\n", study_sse[t], study_acc[t]);

        /* 未学習データで推論 (プレフィックス: unseen) */
        snprintf(cmd, sizeof(cmd),
            "echo '%s\nunseen\n%d\n%d\n%s' | ./hierarchical_inference",
            EVAL_UNSEEN_X, NUM_SAMPLES, NUM_LAYERS, LAYER_SIZES);
        if (run(cmd)) return 1;

        /* 最終試行: 入力・復元画像をプロット */
        if (t == TRIALS - 1) {
            run("gnuplot plot_study.pl");
            printf("  -> X_study_images.eps を出力\n");
            run("gnuplot plot_unseen.pl");
            printf("  -> X_unseen_images.eps を出力\n");
            run("gnuplot plot_y_study.pl");
            printf("  -> y_study_images.eps を出力\n");
            run("gnuplot plot_y_unseen.pl");
            printf("  -> y_unseen_images.eps を出力\n");
        }

        /* 未学習データの評価 */
        snprintf(cmd, sizeof(cmd),
            "echo '%d\n%d\n%d\n%s\n%s' | ./hierarchical_eval",
            IMG_H, IMG_W, NUM_SAMPLES, EVAL_UNSEEN_X, EVAL_Y_UNSEEN);
        if (run(cmd)) return 1;

        unseen_sse[t] = read_value("eval_sse.dat");
        unseen_acc[t] = read_value("eval_accuracy.dat");
        printf("  [未学習] SSE = %f, accuracy = %.2f%%\n\n", unseen_sse[t], unseen_acc[t]);
    }

    /* 各試行の値を保存 */
    save_trials("eval_study_sse.dat",    study_sse,  TRIALS);
    save_trials("eval_study_acc.dat",    study_acc,  TRIALS);
    save_trials("eval_unseen_sse.dat",   unseen_sse, TRIALS);
    save_trials("eval_unseen_acc.dat",   unseen_acc, TRIALS);

    /* 平均を保存 */
    save_avg("eval_study_sse_avg.dat",   study_sse,  TRIALS);
    save_avg("eval_study_acc_avg.dat",   study_acc,  TRIALS);
    save_avg("eval_unseen_sse_avg.dat",  unseen_sse, TRIALS);
    save_avg("eval_unseen_acc_avg.dat",  unseen_acc, TRIALS);

    printf("=== 実験完了 ===\n");
    printf("eval_study_sse.dat  / eval_study_acc.dat  : 学習データ各試行\n");
    printf("eval_unseen_sse.dat / eval_unseen_acc.dat : 未学習データ各試行\n");
    printf("*_avg.dat : それぞれの%d回平均\n", TRIALS);
    printf("study_256_64_y.dat  : 最終試行の学習データ復元結果\n");
    printf("unseen_256_64_y.dat : 最終試行の未学習データ復元結果\n");

    return 0;
}
