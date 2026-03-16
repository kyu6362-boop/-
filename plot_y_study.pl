# 学習データの出力 y の2次元画像表示（サンプルごとに個別EPS出力）
#
# 使い方:
#   gnuplot -e "yfile='study_256_64_y.dat'; input_n=256; hidden_n=64; num_samples=25; outdir='results/images/output/'" plot_y_study.pl

# --- デフォルト値 (run_experiment.c から -e で上書きされる) ---
if (!exists("yfile"))       yfile       = "study_256_64_y.dat"
if (!exists("input_n"))     input_n     = 256
if (!exists("hidden_n"))    hidden_n    = 64
if (!exists("num_samples")) num_samples = 25
if (!exists("outdir"))      outdir      = ""

width  = int(sqrt(input_n + 0.5))
height = int(sqrt(input_n + 0.5))

set palette defined (0 "white", 1 "black")
set cbrange [0:1]
unset colorbox
unset key

do for [s=0:num_samples-1] {
    outfile = sprintf("%sy_study_s%d.eps", outdir, s)
    set terminal postscript eps enhanced color font "Arial,10" size 3,3
    set output outfile
    set title sprintf("y study  sample %d", s) font ",12"
    set xrange [-0.5:width-0.5]
    set yrange [height-0.5:-0.5]
    plot yfile using (int($1)==s ? int($2) % width : 1/0) \
                    :(int($2) / width) \
                    :3 \
         with image notitle
    set output
    print sprintf("saved: %s", outfile)
}
