#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
#define NUM_SAMPLES   25

/* 出力先ディレクトリ (事前に存在すること) */
#define DIR_Z      "results/images/z/"
#define DIR_INPUT  "results/images/input/"
#define DIR_OUTPUT "results/images/output/"
#define DIR_DATA   "results/data/"

#define MAX_LAYERS 16

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

/* LAYER_SIZES 文字列をパースして配列に格納 */
static int parse_layers(const char *str, int *layers, int num_layers)
{
    const char *s = str;
    for (int i = 0; i < num_layers; i++) {
        layers[i] = atoi(s);
        while (*s && *s != ' ') s++;
        while (*s == ' ') s++;
    }
    return 0;
}

/* 各層のz画像を gnuplot で生成するヘルパー */
static void plot_z_images(const char *prefix, const char *file_prefix,
                          int *layers, int num_layers, int num_samples)
{
    char cmd[1024];
    for (int i = 0; i < num_layers - 1; i++) {
        char zfile[256];
        if (file_prefix[0] == '\0')
            snprintf(zfile, sizeof(zfile), "%d_%d_z.dat", layers[i], layers[i+1]);
        else
            snprintf(zfile, sizeof(zfile), "%s_%d_%d_z.dat", file_prefix, layers[i], layers[i+1]);

        int w = (int)sqrt((double)layers[i+1] + 0.5);
        int h = (layers[i+1] + w - 1) / w;

        snprintf(cmd, sizeof(cmd),
            "gnuplot -e \"zfile='%s'; hidden_n=%d; num_samples=%d; prefix='%s_%d_%d'; outdir='%s'\" plot_z_img.pl",
            zfile, layers[i+1], num_samples, prefix, layers[i], layers[i+1], DIR_Z);
        run(cmd);
        printf("  -> %s%s_%d_%d_s{0..%d}.eps (%d枚, %dx%d)\n",
               DIR_Z, prefix, layers[i], layers[i+1], num_samples - 1, num_samples, w, h);
    }
}

int main(void)
{
    char cmd[1024];
    double study_sse[TRIALS],  study_acc[TRIALS];
    double unseen_sse[TRIALS], unseen_acc[TRIALS];

    /* LAYER_SIZES をパースして配列に格納 */
    int layers[MAX_LAYERS];
    parse_layers(LAYER_SIZES, layers, NUM_LAYERS);

    /* 評価用yファイル名を動的生成 (第1層ペアの出力) */
    char eval_y_study[256], eval_y_unseen[256];
    snprintf(eval_y_study,  sizeof(eval_y_study),  "study_%d_%d_y.dat",  layers[0], layers[1]);
    snprintf(eval_y_unseen, sizeof(eval_y_unseen), "unseen_%d_%d_y.dat", layers[0], layers[1]);

    /* 最終層ペアのzファイル名 (散布図用) */
    char last_zfile[256], study_last_zfile[256];
    snprintf(last_zfile, sizeof(last_zfile),
             "%d_%d_z.dat", layers[NUM_LAYERS-2], layers[NUM_LAYERS-1]);
    snprintf(study_last_zfile, sizeof(study_last_zfile),
             "study_%d_%d_z.dat", layers[NUM_LAYERS-2], layers[NUM_LAYERS-1]);

    int plot_variations = NUM_SAMPLES / BASE_PATTERNS;

    printf("=== データ生成 ===\n");

    /* データ生成（1回だけ、ループ外で固定） */
    snprintf(cmd, sizeof(cmd),
        "echo '%d\n%d\n%d\n%d\n%d\n%d' | ./hierarchical_datagen",
        IMG_H, IMG_W, CHANNELS, BASE_PATTERNS, VARIATIONS, BLOCKS);
    if (run(cmd)) return 1;

    printf("\n=== 実験開始: %d 回繰り返し（データ固定・初期値のみ変更） ===\n\n",
           TRIALS);
    printf("  階層構成: ");
    for (int i = 0; i < NUM_LAYERS; i++)
        printf("%s%d", i ? " -> " : "", layers[i]);
    printf(" (%dステージ)\n\n", NUM_LAYERS - 1);

    for (int t = 0; t < TRIALS; t++) {
        printf("===== Trial %d / %d =====\n", t + 1, TRIALS);

        /* 事前学習（毎回ランダム初期値で再学習） */
        snprintf(cmd, sizeof(cmd),
            "echo '%s\n%d\n%d\n%s' | ./hierarchical_autoencoder",
            INPUT_FILE, NUM_SAMPLES, NUM_LAYERS, LAYER_SIZES);
        if (run(cmd)) return 1;

        /* 最終試行: 事前学習後の潜在空間をプロット */
        if (t == TRIALS - 1) {
            snprintf(cmd, sizeof(cmd),
                "gnuplot -e \"zfile='%s'; outfile='z_scatter_pretrain.eps'; group_size=%d; outdir='%s'\" plot_z.pl",
                last_zfile, plot_variations, DIR_Z);
            run(cmd);
            printf("  -> %sz_scatter_pretrain.eps\n", DIR_Z);

            plot_z_images("z_pretrain", "", layers, NUM_LAYERS, NUM_SAMPLES);
        }

        /* 学習データで推論 (プレフィックス: study) */
        snprintf(cmd, sizeof(cmd),
            "echo '%s\nstudy\n%d\n%d\n%s' | ./hierarchical_inference",
            EVAL_STUDY_X, NUM_SAMPLES, NUM_LAYERS, LAYER_SIZES);
        if (run(cmd)) return 1;

        /* 最終試行: 推論後の潜在空間をプロット */
        if (t == TRIALS - 1) {
            snprintf(cmd, sizeof(cmd),
                "gnuplot -e \"zfile='%s'; outfile='z_scatter_inference.eps'; group_size=%d; outdir='%s'\" plot_z.pl",
                study_last_zfile, plot_variations, DIR_Z);
            run(cmd);
            printf("  -> %sz_scatter_inference.eps\n", DIR_Z);

            plot_z_images("z_study", "study", layers, NUM_LAYERS, NUM_SAMPLES);
        }

        /* 学習データの評価 */
        snprintf(cmd, sizeof(cmd),
            "echo '%d\n%d\n%d\n%s\n%s' | ./hierarchical_eval",
            IMG_H, IMG_W, NUM_SAMPLES, EVAL_STUDY_X, eval_y_study);
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
            plot_z_images("z_unseen", "unseen", layers, NUM_LAYERS, NUM_SAMPLES);

            snprintf(cmd, sizeof(cmd),
                "gnuplot -e \"sample1=%d; variations=%d; ih=%d; iw=%d; outdir='%s'\" plot_study.pl",
                BASE_PATTERNS, plot_variations, IMG_H, IMG_W, DIR_INPUT);
            run(cmd);
            printf("  -> %sX_study_s{0..%d}.eps (%d枚)\n", DIR_INPUT, NUM_SAMPLES - 1, NUM_SAMPLES);

            snprintf(cmd, sizeof(cmd),
                "gnuplot -e \"sample1=%d; variations=%d; ih=%d; iw=%d; outdir='%s'\" plot_unseen.pl",
                BASE_PATTERNS, plot_variations, IMG_H, IMG_W, DIR_INPUT);
            run(cmd);
            printf("  -> %sX_unseen_s{0..%d}.eps (%d枚)\n", DIR_INPUT, NUM_SAMPLES - 1, NUM_SAMPLES);

            snprintf(cmd, sizeof(cmd),
                "gnuplot -e \"yfile='%s'; input_n=%d; hidden_n=%d; num_samples=%d; outdir='%s'\" plot_y_study.pl",
                eval_y_study, layers[0], layers[1], NUM_SAMPLES, DIR_OUTPUT);
            run(cmd);
            printf("  -> %sy_study_s{0..%d}.eps (%d枚)\n", DIR_OUTPUT, NUM_SAMPLES - 1, NUM_SAMPLES);

            snprintf(cmd, sizeof(cmd),
                "gnuplot -e \"yfile='%s'; input_n=%d; hidden_n=%d; num_samples=%d; outdir='%s'\" plot_y_unseen.pl",
                eval_y_unseen, layers[0], layers[1], NUM_SAMPLES, DIR_OUTPUT);
            run(cmd);
            printf("  -> %sy_unseen_s{0..%d}.eps (%d枚)\n", DIR_OUTPUT, NUM_SAMPLES - 1, NUM_SAMPLES);
        }

        /* 未学習データの評価 */
        snprintf(cmd, sizeof(cmd),
            "echo '%d\n%d\n%d\n%s\n%s' | ./hierarchical_eval",
            IMG_H, IMG_W, NUM_SAMPLES, EVAL_UNSEEN_X, eval_y_unseen);
        if (run(cmd)) return 1;

        unseen_sse[t] = read_value("eval_sse.dat");
        unseen_acc[t] = read_value("eval_accuracy.dat");
        printf("  [未学習] SSE = %f, accuracy = %.2f%%\n\n", unseen_sse[t], unseen_acc[t]);
    }

    /* 各試行の値を直接 results/data/ に保存 */
    save_trials(DIR_DATA "eval_study_sse.dat",    study_sse,  TRIALS);
    save_trials(DIR_DATA "eval_study_acc.dat",    study_acc,  TRIALS);
    save_trials(DIR_DATA "eval_unseen_sse.dat",   unseen_sse, TRIALS);
    save_trials(DIR_DATA "eval_unseen_acc.dat",   unseen_acc, TRIALS);

    /* 平均を直接 results/data/ に保存 */
    save_avg(DIR_DATA "eval_study_sse_avg.dat",   study_sse,  TRIALS);
    save_avg(DIR_DATA "eval_study_acc_avg.dat",   study_acc,  TRIALS);
    save_avg(DIR_DATA "eval_unseen_sse_avg.dat",  unseen_sse, TRIALS);
    save_avg(DIR_DATA "eval_unseen_acc_avg.dat",  unseen_acc, TRIALS);

    /* サマリー表示 */
    printf("=== 実験完了 ===\n");
    printf("  階層構成: ");
    for (int i = 0; i < NUM_LAYERS; i++)
        printf("%s%d", i ? " -> " : "", layers[i]);
    printf(" (%dステージ)\n\n", NUM_LAYERS - 1);

    printf("%s : 潜在空間z画像\n", DIR_Z);
    printf("  z_scatter_pretrain.eps  - 事前学習後の潜在空間散布図\n");
    printf("  z_scatter_inference.eps - 推論後の潜在空間散布図\n");
    for (int i = 0; i < NUM_LAYERS - 1; i++) {
        int w = (int)sqrt((double)layers[i+1] + 0.5);
        int h = (layers[i+1] + w - 1) / w;
        printf("  z_pretrain_%d_%d_s{n}.eps  - 事前学習z 第%d層 %dx%d (%d枚)\n",
               layers[i], layers[i+1], i+1, w, h, NUM_SAMPLES);
        printf("  z_study_%d_%d_s{n}.eps    - 学習推論z 第%d層 %dx%d (%d枚)\n",
               layers[i], layers[i+1], i+1, w, h, NUM_SAMPLES);
        printf("  z_unseen_%d_%d_s{n}.eps   - 未学習推論z 第%d層 %dx%d (%d枚)\n",
               layers[i], layers[i+1], i+1, w, h, NUM_SAMPLES);
    }
    printf("%s : 入力画像\n", DIR_INPUT);
    printf("  X_study_s{n}.eps    - 学習用入力画像 (%d枚)\n", NUM_SAMPLES);
    printf("  X_unseen_s{n}.eps   - 未学習入力画像 (%d枚)\n", NUM_SAMPLES);
    printf("%s : 出力(復元)画像\n", DIR_OUTPUT);
    printf("  y_study_s{n}.eps    - 学習データ復元画像 (%d枚)\n", NUM_SAMPLES);
    printf("  y_unseen_s{n}.eps   - 未学習データ復元画像 (%d枚)\n", NUM_SAMPLES);
    printf("%s : 評価データ\n", DIR_DATA);
    printf("  eval_*_sse.dat / eval_*_acc.dat : 各試行の値\n");
    printf("  *_avg.dat : %d回平均\n", TRIALS);

    return 0;
}
