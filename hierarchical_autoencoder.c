#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define R          0.001
#define LOSS_LIMIT 0.01
#define sample     25
#define MAX_INPUT  4096
#define MAX_HIDDEN 1024
#define MAX_LAYERS 10

/* Adam ハイパーパラメータ */
#define BETA1   0.9
#define BETA2   0.999
#define EPSILON 1e-8
#define WD      1e-4    /* weight decay */

/* ランタイムで設定される実際のサイズ */
static int input_n;
static int hidden_n;

/* 固定サイズグローバル配列 */
static double X_data[sample * MAX_INPUT];
static double F1[MAX_INPUT], F2[MAX_INPUT];
static double EF[MAX_HIDDEN], z[MAX_HIDDEN];
static double y[MAX_INPUT];
static double D1[MAX_INPUT * MAX_HIDDEN];
static double D2[MAX_HIDDEN * MAX_INPUT];
static double DB1[MAX_HIDDEN];
static double DB2[MAX_INPUT];
static double dL[MAX_INPUT], dy_g[MAX_INPUT], dL_output[MAX_INPUT];
static double dz[MAX_HIDDEN], dL_hidden[MAX_HIDDEN];
static double dD1[MAX_INPUT * MAX_HIDDEN];
static double dD2[MAX_HIDDEN * MAX_INPUT];
static double Zn[sample * MAX_HIDDEN];
static double Zy[sample * MAX_INPUT];

/* Adam 状態変数 (m: 1次モーメント, v: 2次モーメント) */
static double mD1[MAX_INPUT * MAX_HIDDEN], vD1[MAX_INPUT * MAX_HIDDEN];
static double mD2[MAX_HIDDEN * MAX_INPUT], vD2[MAX_HIDDEN * MAX_INPUT];
static double mDB1[MAX_HIDDEN], vDB1[MAX_HIDDEN];
static double mDB2[MAX_INPUT],  vDB2[MAX_INPUT];
static int adam_t;

static void clear_adam_state(void)
{
    adam_t = 0;
    for(int i = 0; i < input_n * hidden_n; i++){
        mD1[i] = 0.0;  vD1[i] = 0.0;
    }
    for(int i = 0; i < hidden_n * input_n; i++){
        mD2[i] = 0.0;  vD2[i] = 0.0;
    }
    for(int i = 0; i < hidden_n; i++){
        mDB1[i] = 0.0; vDB1[i] = 0.0;
    }
    for(int i = 0; i < input_n; i++){
        mDB2[i] = 0.0; vDB2[i] = 0.0;
    }
}

/* Xavier初期化: 重みを sqrt(6 / (fan_in + fan_out)) の範囲で一様乱数 */
static void xavier_init_WB(void)
{
    double limit1 = sqrt(6.0 / (double)(input_n + hidden_n));
    double limit2 = sqrt(6.0 / (double)(hidden_n + input_n));

    for(int i = 0; i < hidden_n; i++)
        DB1[i] = 0.0;
    for(int i = 0; i < input_n; i++)
        DB2[i] = 0.0;
    for(int i = 0; i < input_n * hidden_n; i++)
        D1[i] = limit1 * (2.0 * (double)rand()/RAND_MAX - 1.0);
    for(int i = 0; i < hidden_n * input_n; i++)
        D2[i] = limit2 * (2.0 * (double)rand()/RAND_MAX - 1.0);
}

static void flatten_1(int n)
{
    for(int i = 0; i < input_n; i++)
        F1[i] = X_data[n * input_n + i];
}

static void encoder_output(void)
{
    for(int j = 0; j < hidden_n; j++)
        EF[j] = DB1[j];
    for(int i = 0; i < input_n; i++)
        for(int j = 0; j < hidden_n; j++)
            EF[j] += F1[i] * D1[i * hidden_n + j];
    for(int j = 0; j < hidden_n; j++)
        EF[j] /= (double)input_n;
    for(int i = 0; i < hidden_n; i++)
        z[i] = 1.0 / (1.0 + exp(-EF[i]));
}

static void dense_1(void)
{
    for(int i = 0; i < input_n; i++)
        F2[i] = DB2[i];
    for(int i = 0; i < input_n; i++)
        for(int j = 0; j < hidden_n; j++)
            F2[i] += z[j] * D2[j * input_n + i];
    for(int i = 0; i < input_n; i++)
        F2[i] /= (double)hidden_n;
    for(int i = 0; i < input_n; i++)
        y[i] = 1.0 / (1.0 + exp(-F2[i]));
}

static void dense_forward(void)
{
    encoder_output();
    dense_1();
}

static double sum_of_squared_error(void)
{
    double loss = 0.0;
    for(int i = 0; i < input_n; i++)
        loss += 0.5 * (y[i] - F1[i]) * (y[i] - F1[i]);
    return loss / (double)input_n;
}

static void grad_output(void)
{
    for(int i = 0; i < input_n; i++){
        dL[i]        = y[i] - F1[i];
        dy_g[i]      = y[i] * (1.0 - y[i]);
        dL_output[i] = dL[i] * dy_g[i];
    }
    for(int i = 0; i < input_n; i++)
        for(int j = 0; j < hidden_n; j++)
            dD2[j * input_n + i] = z[j] * dL_output[i];
    /* dL_output[i] がDB2[i]の勾配 */
}

static void grad_hidden(void)
{
    for(int i = 0; i < hidden_n; i++){
        dL_hidden[i] = 0.0;
        for(int j = 0; j < input_n; j++)
            dD1[j * hidden_n + i] = 0.0;
    }
    for(int i = 0; i < hidden_n; i++){
        dz[i] = z[i] * (1.0 - z[i]);
        for(int j = 0; j < input_n; j++)
            dL_hidden[i] += D2[i * input_n + j] * dL_output[j];
        dL_hidden[i] *= dz[i];
    }
    for(int i = 0; i < input_n; i++)
        for(int j = 0; j < hidden_n; j++)
            dD1[i * hidden_n + j] += F1[i] * dL_hidden[j];
    /* dL_hidden[i] がDB1[i]の勾配 */
}

/* AdamW: 重みにweight decay、バイアスにはweight decayなし */
static void adam_update(void)
{
    adam_t++;
    double bc1 = 1.0 - pow(BETA1, adam_t);
    double bc2 = 1.0 - pow(BETA2, adam_t);

    /* D1 (weight decay あり) */
    for(int i = 0; i < input_n * hidden_n; i++){
        double g = dD1[i];
        mD1[i] = BETA1 * mD1[i] + (1.0 - BETA1) * g;
        vD1[i] = BETA2 * vD1[i] + (1.0 - BETA2) * g * g;
        double mh = mD1[i] / bc1;
        double vh = vD1[i] / bc2;
        D1[i] -= R * (mh / (sqrt(vh) + EPSILON) + WD * D1[i]);
    }
    /* D2 (weight decay あり) */
    for(int i = 0; i < hidden_n * input_n; i++){
        double g = dD2[i];
        mD2[i] = BETA1 * mD2[i] + (1.0 - BETA1) * g;
        vD2[i] = BETA2 * vD2[i] + (1.0 - BETA2) * g * g;
        double mh = mD2[i] / bc1;
        double vh = vD2[i] / bc2;
        D2[i] -= R * (mh / (sqrt(vh) + EPSILON) + WD * D2[i]);
    }
    /* DB1 (weight decay なし) */
    for(int i = 0; i < hidden_n; i++){
        double g = dL_hidden[i];
        mDB1[i] = BETA1 * mDB1[i] + (1.0 - BETA1) * g;
        vDB1[i] = BETA2 * vDB1[i] + (1.0 - BETA2) * g * g;
        double mh = mDB1[i] / bc1;
        double vh = vDB1[i] / bc2;
        DB1[i] -= R * mh / (sqrt(vh) + EPSILON);
    }
    /* DB2 (weight decay なし) */
    for(int i = 0; i < input_n; i++){
        double g = dL_output[i];
        mDB2[i] = BETA1 * mDB2[i] + (1.0 - BETA1) * g;
        vDB2[i] = BETA2 * vDB2[i] + (1.0 - BETA2) * g * g;
        double mh = mDB2[i] / bc1;
        double vh = vDB2[i] / bc2;
        DB2[i] -= R * mh / (sqrt(vh) + EPSILON);
    }
}

static void train_autoencoder(void)
{
    int epoch = 0;
    double loss_sum;
    do {
        loss_sum = 0.0;
        for(int n = 0; n < sample; n++){
            flatten_1(n);
            dense_forward();
            loss_sum += sum_of_squared_error();
            grad_output();
            grad_hidden();
            adam_update();
        }
        loss_sum /= (double)sample;
        epoch++;
        if(epoch % 100 == 0)
            printf("Epoch %d  LOSS %lf\n", epoch, loss_sum);
    } while(loss_sum > LOSS_LIMIT);
    printf("収束: Epoch %d  LOSS %lf\n", epoch, loss_sum);
}

static void save_stage_results(void)
{
    char fname[256];
    FILE *fp;

    /* 順伝播して z, y を記録 */
    for(int n = 0; n < sample; n++){
        flatten_1(n);
        dense_forward();
        for(int i = 0; i < hidden_n; i++)
            Zn[n * hidden_n + i] = z[i];
        for(int j = 0; j < input_n; j++)
            Zy[n * input_n + j] = y[j];
    }

    snprintf(fname, sizeof(fname), "%d_%d_D1.dat", input_n, hidden_n);
    fp = fopen(fname, "w");
    for(int i = 0; i < input_n; i++)
        for(int j = 0; j < hidden_n; j++)
            fprintf(fp, "%d %d %f\n", i, j, D1[i * hidden_n + j]);
    fclose(fp);

    snprintf(fname, sizeof(fname), "%d_%d_DB1.dat", input_n, hidden_n);
    fp = fopen(fname, "w");
    for(int i = 0; i < hidden_n; i++)
        fprintf(fp, "%d %f\n", i, DB1[i]);
    fclose(fp);

    snprintf(fname, sizeof(fname), "%d_%d_D2.dat", input_n, hidden_n);
    fp = fopen(fname, "w");
    for(int i = 0; i < hidden_n; i++)
        for(int j = 0; j < input_n; j++)
            fprintf(fp, "%d %d %f\n", i, j, D2[i * input_n + j]);
    fclose(fp);

    snprintf(fname, sizeof(fname), "%d_%d_DB2.dat", input_n, hidden_n);
    fp = fopen(fname, "w");
    for(int i = 0; i < input_n; i++)
        fprintf(fp, "%d %f\n", i, DB2[i]);
    fclose(fp);

    snprintf(fname, sizeof(fname), "%d_%d_z.dat", input_n, hidden_n);
    fp = fopen(fname, "w");
    for(int s = 0; s < sample; s++)
        for(int j = 0; j < hidden_n; j++)
            fprintf(fp, "%d %d %f\n", s, j, Zn[s * hidden_n + j]);
    fclose(fp);

    snprintf(fname, sizeof(fname), "%d_%d_y.dat", input_n, hidden_n);
    fp = fopen(fname, "w");
    for(int s = 0; s < sample; s++)
        for(int j = 0; j < input_n; j++)
            fprintf(fp, "%d %d %f\n", s, j, Zy[s * input_n + j]);
    fclose(fp);
}

int main(void)
{
    char infile[256];
    int num_layers;
    int layer_sizes[MAX_LAYERS];

    printf("入力ファイル名: ");
    scanf("%s", infile);
    printf("階層数: ");
    scanf("%d", &num_layers);
    if(num_layers < 2 || num_layers > MAX_LAYERS){
        fprintf(stderr, "階層数は2以上%d以下で指定してください\n", MAX_LAYERS);
        return 1;
    }
    printf("各階層のサイズ: ");
    for(int i = 0; i < num_layers; i++){
        scanf("%d", &layer_sizes[i]);
        if(layer_sizes[i] > MAX_INPUT){
            fprintf(stderr, "サイズ %d は最大値 %d を超えています\n",
                    layer_sizes[i], MAX_INPUT);
            return 1;
        }
    }

    srand((unsigned int)time(NULL));

    /* --- Stage 1: ファイルからデータを読み込み --- */
    input_n  = layer_sizes[0];
    hidden_n = layer_sizes[1];

    FILE *fp = fopen(infile, "r");
    if(!fp){ fprintf(stderr, "Cannot open %s\n", infile); return 1; }

    /* 先頭行のトークン数でフォーマット判定 */
    char line[512], tmp[512];
    if(!fgets(line, sizeof(line), fp)){ fclose(fp); return 1; }
    int tok = 0;
    strcpy(tmp, line);
    for(char *p = strtok(tmp, " \t\n"); p; p = strtok(NULL, " \t\n")) tok++;
    rewind(fp);

    if(tok >= 4){
        /* 4列形式: s y x val */
        int width = (int)round(sqrt((double)input_n));
        int s_r, yr, xr; double v;
        while(fscanf(fp, "%d %d %d %lf", &s_r, &yr, &xr, &v) == 4){
            int idx = yr * width + xr;
            if(s_r >= 0 && s_r < sample && idx >= 0 && idx < input_n)
                X_data[s_r * input_n + idx] = v;
        }
    } else {
        /* 3列形式: s idx val */
        int s_r, idx_r; double v;
        while(fscanf(fp, "%d %d %lf", &s_r, &idx_r, &v) == 3)
            if(s_r >= 0 && s_r < sample && idx_r >= 0 && idx_r < input_n)
                X_data[s_r * input_n + idx_r] = v;
    }
    fclose(fp);

    /* --- 各ステージの学習 --- */
    for(int stage = 0; stage < num_layers - 1; stage++){
        input_n  = layer_sizes[stage];
        hidden_n = layer_sizes[stage + 1];

        if(hidden_n > MAX_HIDDEN){
            fprintf(stderr, "中間層サイズ %d は最大値 %d を超えています\n",
                    hidden_n, MAX_HIDDEN);
            return 1;
        }

        printf("\n--- Stage %d: %d -> %d ---\n", stage + 1, input_n, hidden_n);

        xavier_init_WB();
        clear_adam_state();
        train_autoencoder();
        save_stage_results();

        /* 次のステージがあれば、z を X_data にコピー */
        if(stage + 2 < num_layers){
            for(int i = 0; i < sample * MAX_INPUT; i++)
                X_data[i] = 0.0;
            for(int s = 0; s < sample; s++)
                for(int j = 0; j < hidden_n; j++)
                    X_data[s * hidden_n + j] = Zn[s * hidden_n + j];
        }
    }

    return 0;
}
