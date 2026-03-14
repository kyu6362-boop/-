#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_IH      128
#define MAX_IW      128
#define MAX_IN      4
#define MAX_SAMPLE  50
#define MAX_SAMPLE1 10
#define MAX_M1      20
#define MAX_N_BLOCK 50

/* ランタイムで設定される実際のサイズ */
static int ih, iw, in_ch;
static int bh, bw;
static int sample_n;
static int sample1;
static int m1;
static int n_block;

/* 固定サイズグローバル配列 */
static double X      [MAX_SAMPLE ][MAX_IH][MAX_IW][MAX_IN];
static double X1     [MAX_SAMPLE ][MAX_IH][MAX_IW][MAX_IN];
static double Xchange[MAX_SAMPLE1][MAX_M1][MAX_IH][MAX_IW][MAX_IN];

/* ブロック左上座標: BP[base][block][0=y, 1=x] */
static int BP[MAX_SAMPLE1][MAX_N_BLOCK][2];

/* ================================================================
 * ブロック生成・変換関数
 * ================================================================ */
/*
 * n 番目のベースパターンを生成:
 * n_block 個の bh×bw ブロックをランダムに配置して Xin に書き込む。
 * ブロック位置は BP[n] に記録する。
 */
static void block(int n, double Xin[MAX_IH][MAX_IW][MAX_IN])
{
    for (int s = 0; s < n_block; s++) {
        int by = rand() % (ih - bh + 1);
        int bx = rand() % (iw - bw + 1);
        for (int y = 0; y < bh; y++)
            for (int x = 0; x < bw; x++)
                for (int a = 0; a < in_ch; a++)
                    Xin[by + y][bx + x][a] = 1.0;
        BP[n][s][0] = by;
        BP[n][s][1] = bx;
    }
}

/*
 * 全ブロックを同じ方向に最大 s ピクセルシフトした画像を Xout に生成。
 */
static void block1(double Xout[MAX_IH][MAX_IW][MAX_IN], int s, int n)
{
    for (int y = 0; y < ih; y++)
        for (int x = 0; x < iw; x++)
            for (int a = 0; a < in_ch; a++)
                Xout[y][x][a] = 0.0;

    int dy = (rand() % 2 == 0) ? ((rand() % 2 ? 1 : -1) * s) : 0;
    int dx = (rand() % 2 == 0) ? ((rand() % 2 ? 1 : -1) * s) : 0;

    for (int b = 0; b < n_block; b++) {
        int by = BP[n][b][0];
        int bx = BP[n][b][1];
        for (int y = 0; y < bh; y++)
            for (int x = 0; x < bw; x++)
                for (int a = 0; a < in_ch; a++) {
                    int ny = by + y + dy;
                    int nx = bx + x + dx;
                    if (ny >= 0 && ny < ih && nx >= 0 && nx < iw)
                        Xout[ny][nx][a] = 1.0;
                }
    }
}

/*
 * 各ブロックを個別に最大 s ピクセルシフトした画像を Xout に生成。
 */
static void block2(double Xout[MAX_IH][MAX_IW][MAX_IN], int s, int n)
{
    for (int y = 0; y < ih; y++)
        for (int x = 0; x < iw; x++)
            for (int a = 0; a < in_ch; a++)
                Xout[y][x][a] = 0.0;

    for (int b = 0; b < n_block; b++) {
        int by = BP[n][b][0];
        int bx = BP[n][b][1];
        int dy = (rand() % 2 == 0) ? ((rand() % 2 ? 1 : -1) * s) : 0;
        int dx = (rand() % 2 == 0) ? ((rand() % 2 ? 1 : -1) * s) : 0;
        for (int y = 0; y < bh; y++)
            for (int x = 0; x < bw; x++)
                for (int a = 0; a < in_ch; a++) {
                    int ny = by + y + dy;
                    int nx = bx + x + dx;
                    if (ny >= 0 && ny < ih && nx >= 0 && nx < iw)
                        Xout[ny][nx][a] = 1.0;
                }
    }
}

/*
 * ランダムに p 点のピクセルを 1.0 に設定するノイズを付加する。
 */
static void plusnoise(double Xout[MAX_IH][MAX_IW][MAX_IN], int p)
{
    for (int i = 0; i < p; i++) {
        int py = rand() % ih;
        int px = rand() % iw;
        int pa = rand() % in_ch;
        Xout[py][px][pa] = 1.0;
    }
}

/* ================================================================
 * main
 * ================================================================ */
int main(void)
{
    srand((unsigned int)time(NULL));

    printf("画像の高さ: ");
    scanf("%d", &ih);
    printf("画像の幅: ");
    scanf("%d", &iw);
    printf("チャンネル数: ");
    scanf("%d", &in_ch);
    printf("ベースパターン数: ");
    scanf("%d", &sample1);
    printf("バリエーション数: ");
    scanf("%d", &m1);
    printf("ブロック数: ");
    scanf("%d", &n_block);

    if (ih > MAX_IH || iw > MAX_IW || in_ch > MAX_IN) {
        fprintf(stderr, "画像サイズが最大値を超えています (最大 %dx%dx%d)\n",
                MAX_IH, MAX_IW, MAX_IN);
        return 1;
    }
    if (sample1 > MAX_SAMPLE1 || m1 > MAX_M1 || n_block > MAX_N_BLOCK) {
        fprintf(stderr, "パラメータが最大値を超えています\n");
        return 1;
    }

    bh = ih / 4;
    bw = iw / 4;
    sample_n = sample1 * m1 / 2;

    if (sample_n > MAX_SAMPLE) {
        fprintf(stderr, "サンプル数 %d が最大値 %d を超えています\n",
                sample_n, MAX_SAMPLE);
        return 1;
    }

    printf("\n=== Data Generator ===\n");
    printf("Image size : %d x %d x %d  (H x W x C)\n", ih, iw, in_ch);
    printf("Block size : %d x %d\n", bh, bw);
    printf("N_BLOCK    : %d\n", n_block);
    printf("SAMPLE     : %d (study) + %d (unseen)\n", sample_n, sample_n);
    printf("SAMPLE1    : %d base patterns,  M1=%d variations each\n\n",
           sample1, m1);

    /* Xchange を 0 初期化 */
    for (int n = 0; n < sample1; n++)
        for (int i = 0; i < m1; i++)
            for (int y = 0; y < ih; y++)
                for (int x = 0; x < iw; x++)
                    for (int a = 0; a < in_ch; a++)
                        Xchange[n][i][y][x][a] = 0.0;

    /* 各ベースパターンを生成 */
    for (int n = 0; n < sample1; n++)
        block(n, Xchange[n][0]);

    /* ベースパターンを全バリエーションにコピー */
    for (int n = 0; n < sample1; n++)
        for (int i = 1; i < m1; i++)
            for (int y = 0; y < ih; y++)
                for (int x = 0; x < iw; x++)
                    for (int a = 0; a < in_ch; a++)
                        Xchange[n][i][y][x][a] = Xchange[n][0][y][x][a];

    /* 各バリエーションにランダム変換を適用 */
    for (int n = 0; n < sample1; n++) {
        for (int i = 0; i < m1; i++) {
            int s = rand() % 3;
            if (s == 0) block1(Xchange[n][i], 1, n);
            if (s == 1) block2(Xchange[n][i], 1, n);
            if (s == 2) {
                int p = (int)(ih * iw * in_ch * 0.02);
                plusnoise(Xchange[n][i], p);
            }
        }
    }

    /* 前半バリエーション → 学習データ X */
    for (int n = 0; n < sample1; n++)
        for (int i = 0; i < m1 / 2; i++)
            for (int y = 0; y < ih; y++)
                for (int x = 0; x < iw; x++)
                    for (int a = 0; a < in_ch; a++)
                        X[n * (m1 / 2) + i][y][x][a] = Xchange[n][i][y][x][a];

    /* 後半バリエーション → 未学習データ X1 */
    for (int n = 0; n < sample1; n++)
        for (int i = m1 / 2; i < m1; i++)
            for (int y = 0; y < ih; y++)
                for (int x = 0; x < iw; x++)
                    for (int a = 0; a < in_ch; a++)
                        X1[n * (m1 / 2) + (i - m1 / 2)][y][x][a] =
                            Xchange[n][i][y][x][a];

    /* ----------------------------------------------------------------
     * X_study.dat: フォーマット "s y x value"
     *              hae の4列形式読み込みに対応
     * ---------------------------------------------------------------- */
    FILE *fp = fopen("X_study.dat", "w");
    if (!fp) { fprintf(stderr, "Error: cannot open X_study.dat\n"); return 1; }
    for (int n = 0; n < sample_n; n++)
        for (int y = 0; y < ih; y++)
            for (int x = 0; x < iw; x++)
                fprintf(fp, "%d %d %d %f\n", n, y, x, X[n][y][x][0]);
    fclose(fp);
    printf("X_study.dat  saved  (%d samples)\n", sample_n);

    /* X_unseen.dat */
    fp = fopen("X_unseen.dat", "w");
    if (!fp) { fprintf(stderr, "Error: cannot open X_unseen.dat\n"); return 1; }
    for (int n = 0; n < sample_n; n++)
        for (int y = 0; y < ih; y++)
            for (int x = 0; x < iw; x++)
                fprintf(fp, "%d %d %d %f\n", n, y, x, X1[n][y][x][0]);
    fclose(fp);
    printf("X_unseen.dat saved  (%d samples)\n", sample_n);

    /* 各ベースパターンの可視化ファイル */
    char fname[256];
    for (int n = 0; n < sample1; n++) {
        sprintf(fname, "X_input%d.dat", n);
        fp = fopen(fname, "w");
        if (!fp) continue;
        for (int y = 0; y < ih; y++)
            for (int x = 0; x < iw; x++)
                fprintf(fp, "%d %d %f\n",
                        x, y, X[n * (m1 / 2)][y][x][0]);
        fclose(fp);
    }
    printf("X_input{0-%d}.dat saved\n\n", sample1 - 1);

    printf("hae との接続:\n");
    printf("  入力ファイル名: X_study.dat\n");
    printf("  入力層サイズ  : %d (= %d x %d)\n", ih * iw, ih, iw);

    return 0;
}
