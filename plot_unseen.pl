# 未学習入力データ X_unseen の2次元画像表示（サンプルごとに個別EPS出力）
#
# 使い方:
#   gnuplot -e "sample1=5; variations=5; ih=16; iw=16; outdir='results/images/input/'" plot_unseen.pl

# --- デフォルト値 (run_experiment.c から -e で上書きされる) ---
if (!exists("sample1"))    sample1    = 5
if (!exists("variations")) variations = 5
if (!exists("ih"))         ih         = 16
if (!exists("iw"))         iw         = 16
if (!exists("outdir"))     outdir     = ""

set palette defined (0 "white", 1 "black")
set cbrange [0:1]
unset colorbox
unset key

do for [n=0:sample1-1] {
    do for [v=0:variations-1] {
        s       = n * variations + v
        fname   = sprintf("X_unseen_s%d_v%d.dat", n, v)
        outfile = sprintf("%sX_unseen_s%d.eps", outdir, s)
        set terminal postscript eps enhanced color font "Arial,10" size 3,3
        set output outfile
        set title sprintf("X unseen  s%d", s) font ",12"
        set xrange [-0.5:iw-0.5]
        set yrange [ih-0.5:-0.5]
        plot fname using 1:2:3 with image notitle
        set output
        print sprintf("saved: %s", outfile)
    }
}
