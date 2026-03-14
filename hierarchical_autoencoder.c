#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define R          0.01
#define LOSS_LIMIT 0.01
#define sample     25
#define MAX_INPUT  4096
#define MAX_HIDDEN 1024
#define MAX_LAYERS 10

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

/* Xavier初期化: 重みを [-limit, +limit] の一様乱数 (limit = sqrt(6/(fan_in+fan_out))) */
static void random_WB(void)
{
    double limit = sqrt(6.0 / (double)(input_n + hidden_n));

    for(int i = 0; i < hidden_n; i++)
        DB1[i] = 0.0;
    for(int i = 0; i < input_n; i++)
        DB2[i] = 0.0;
    for(int i = 0; i < input_n * hidden_n; i++)
        D1[i] = limit * (2.0 * (double)rand()/RAND_MAX - 1.0);
    for(int i = 0; i < hidden_n * input_n; i++)
        D2[i] = limit * (2.0 * (double)rand()/RAND_MAX - 1.0);
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
}

static void update_weights(void)
{
    for(int i = 0; i < input_n; i++){
        for(int j = 0; j < hidden_n; j++){
            D2[j * input_n + i] -= R * dD2[j * input_n + i];
            D1[i * hidden_n + j] -= R * dD1[i * hidden_n + j];
        }
        DB2[i] -= R * dL_output[i];
    }
    for(int i = 0; i < hidden_n; i++)
        DB1[i] -= R * dL_hidden[i];
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
            update_weights();
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

    /* サンプルごとの個別ファイル */
    int y_width = (int)round(sqrt((double)input_n));
    int z_width = (int)round(sqrt((double)hidden_n));

    for(int s = 0; s < sample; s++){
        /* z: 潜在空間（2D reshape） */
        snprintf(fname, sizeof(fname), "%d_%d_z_s%d.dat", input_n, hidden_n, s);
        fp = fopen(fname, "w");
        if(fp){
            for(int j = 0; j < hidden_n; j++)
                fprintf(fp, "%d %d %f\n",
                        j % z_width, j / z_width, Zn[s * hidden_n + j]);
            fclose(fp);
        }
        /* y: 出力（2D reshape） */
        snprintf(fname, sizeof(fname), "%d_%d_y_s%d.dat", input_n, hidden_n, s);
        fp = fopen(fname, "w");
        if(fp){
            for(int j = 0; j < input_n; j++)
                fprintf(fp, "%d %d %f\n",
                        j % y_width, j / y_width, Zy[s * input_n + j]);
            fclose(fp);
        }
    }
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

        random_WB();
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
