# 潜在空間 z の2次元画像表示（サンプルごとに個別EPS出力）
#
# z データ (sample z_idx value) を hidden_n 次元から
# width x height の2次元にリシェイプしてヒートマップ表示する。
#
# 使い方:
#   gnuplot -e "zfile='256_64_z.dat'; hidden_n=64; num_samples=25; prefix='z_pretrain_256_64'; outdir='results/images/z/'" plot_z_img.pl

# --- デフォルト値 (run_experiment.c から -e で上書きされる) ---
if (!exists("outdir")) outdir = ""

# --- 次元からリシェイプサイズを計算 ---
width  = int(sqrt(hidden_n + 0.5))
height = int(hidden_n / width + 0.5)

# --- 描画設定 ---
set palette defined (0 "white", 1 "black")
set cbrange [0:1]
unset colorbox
unset key

do for [s=0:num_samples-1] {
    outfile = sprintf("%s%s_s%d.eps", outdir, prefix, s)
    set terminal postscript eps enhanced color font "Arial,10" size 3,3
    set output outfile
    set title sprintf("%s  sample %d  (%dx%d)", prefix, s, width, height) font ",12"
    set xrange [-0.5:width-0.5]
    set yrange [height-0.5:-0.5]
    plot zfile using (int($1)==s ? int($2) % width : 1/0) \
                    :(int($2) / width) \
                    :3 \
         with image notitle
    set output
    print sprintf("saved: %s", outfile)
}
