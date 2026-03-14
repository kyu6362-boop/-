#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_SAMPLE 50
#define MAX_INPUT  4096

static int ih, iw;
static int num_samples;
static int input_n;
static double X_in [MAX_SAMPLE * MAX_INPUT];
static double Y_out[MAX_SAMPLE * MAX_INPUT];

int main(void)
{
    char infile[256], yfile[256];

    printf("画像の高さ: ");   scanf("%d", &ih);
    printf("画像の幅: ");     scanf("%d", &iw);
    printf("サンプル数: ");   scanf("%d", &num_samples);
    printf("入力データファイル (X): "); scanf("%s", infile);
    printf("出力データファイル (y): "); scanf("%s", yfile);

    input_n = ih * iw;

    if (num_samples > MAX_SAMPLE) {
        fprintf(stderr, "サンプル数 %d が最大値 %d を超えています\n",
                num_samples, MAX_SAMPLE);
        return 1;
    }
    if (input_n > MAX_INPUT) {
        fprintf(stderr, "入力サイズ %d が最大値 %d を超えています\n",
                input_n, MAX_INPUT);
        return 1;
    }

    /* 配列を 0 初期化 */
    for (int i = 0; i < num_samples * input_n; i++) {
        X_in[i]  = 0.0;
        Y_out[i] = 0.0;
    }

    /* --- 入力データ読み込み (s y x value : 4列形式) --- */
    FILE *fp = fopen(infile, "r");
    if (!fp) { fprintf(stderr, "Cannot open %s\n", infile); return 1; }

    char line[512], tmp[512];
    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return 1; }
    int tok = 0;
    strcpy(tmp, line);
    for (char *p = strtok(tmp, " \t\n"); p; p = strtok(NULL, " \t\n")) tok++;
    rewind(fp);

    if (tok >= 4) {
        int s_r, yr, xr; double v;
        while (fscanf(fp, "%d %d %d %lf", &s_r, &yr, &xr, &v) == 4) {
            int idx = yr * iw + xr;
            if (s_r >= 0 && s_r < num_samples && idx >= 0 && idx < input_n)
                X_in[s_r * input_n + idx] = v;
        }
    } else {
        int s_r, idx_r; double v;
        while (fscanf(fp, "%d %d %lf", &s_r, &idx_r, &v) == 3)
            if (s_r >= 0 && s_r < num_samples && idx_r >= 0 && idx_r < input_n)
                X_in[s_r * input_n + idx_r] = v;
    }
    fclose(fp);
    printf("入力データ読み込み完了: %s\n", infile);

    /* --- 出力 y 読み込み (s idx value : 3列形式) --- */
    fp = fopen(yfile, "r");
    if (!fp) { fprintf(stderr, "Cannot open %s\n", yfile); return 1; }

    if (!fgets(line, sizeof(line), fp)) { fclose(fp); return 1; }
    tok = 0;
    strcpy(tmp, line);
    for (char *p = strtok(tmp, " \t\n"); p; p = strtok(NULL, " \t\n")) tok++;
    rewind(fp);

    if (tok >= 4) {
        int s_r, yr, xr; double v;
        while (fscanf(fp, "%d %d %d %lf", &s_r, &yr, &xr, &v) == 4) {
            int idx = yr * iw + xr;
            if (s_r >= 0 && s_r < num_samples && idx >= 0 && idx < input_n)
                Y_out[s_r * input_n + idx] = v;
        }
    } else {
        int s_r, idx_r; double v;
        while (fscanf(fp, "%d %d %lf", &s_r, &idx_r, &v) == 3)
            if (s_r >= 0 && s_r < num_samples && idx_r >= 0 && idx_r < input_n)
                Y_out[s_r * input_n + idx_r] = v;
    }
    fclose(fp);
    printf("出力データ読み込み完了: %s\n\n", yfile);

    /* --- 評価 --- */
    char fname[256];

    /* SSE per sample → eval_sse.dat */
    fp = fopen("eval_sse.dat", "w");
    if (!fp) { fprintf(stderr, "Cannot open eval_sse.dat\n"); return 1; }

    double total_sse = 0.0;
    for (int s = 0; s < num_samples; s++) {
        double sse = 0.0;
        for (int i = 0; i < input_n; i++) {
            double d = Y_out[s * input_n + i] - X_in[s * input_n + i];
            sse += d * d;
        }
        fprintf(fp, "%d %f\n", s, sse);
        printf("sample %d  SSE = %f\n", s, sse);
        total_sse += sse;
    }
    fclose(fp);
    printf("\n平均 SSE = %f\n", total_sse / num_samples);
    printf("eval_sse.dat saved\n\n");

    /* 閾値 0.5 で二値化した出力をサンプルごとに保存 */
    for (int s = 0; s < num_samples; s++) {
        snprintf(fname, sizeof(fname), "eval_bin_s%d.dat", s);
        fp = fopen(fname, "w");
        if (!fp) continue;
        for (int i = 0; i < input_n; i++) {
            int bval = (Y_out[s * input_n + i] >= 0.5) ? 1 : 0;
            fprintf(fp, "%d %d %d\n", i % iw, i / iw, bval);
        }
        fclose(fp);
    }
    printf("eval_bin_s{0-%d}.dat saved\n\n", num_samples - 1);

    /* 二値化後の一致率を計算 → eval_accuracy.dat */
    fp = fopen("eval_accuracy.dat", "w");
    if (!fp) { fprintf(stderr, "Cannot open eval_accuracy.dat\n"); return 1; }

    double total_acc = 0.0;
    for (int s = 0; s < num_samples; s++) {
        int match = 0;
        for (int i = 0; i < input_n; i++) {
            int bin_y = (Y_out[s * input_n + i] >= 0.5) ? 1 : 0;
            int bin_x = (X_in [s * input_n + i] >= 0.5) ? 1 : 0;
            if (bin_y == bin_x) match++;
        }
        double acc = (double)match / input_n * 100.0;
        fprintf(fp, "%d %f\n", s, acc);
        printf("sample %d  accuracy = %.2f%% (%d/%d)\n", s, acc, match, input_n);
        total_acc += acc;
    }
    fclose(fp);
    printf("\n平均 accuracy = %.2f%%\n", total_acc / num_samples);
    printf("eval_accuracy.dat saved\n");

    return 0;
}
