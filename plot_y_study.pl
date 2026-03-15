# 学習データの出力 y の2次元画像表示（サンプルごとに個別EPS出力）
#
# 使い方:
#   gnuplot plot_y_study.pl

# --- 設定 ---
yfile       = "study_256_64_y.dat"  # 学習データの出力yファイル (s idx value)
input_n     = 256   # 入力層サイズ
hidden_n    = 64    # 中間層サイズ
num_samples = 25    # サンプル数

width  = int(sqrt(input_n + 0.5))
height = int(sqrt(input_n + 0.5))

set palette defined (0 "white", 1 "black")
set cbrange [0:1]
unset colorbox
unset key

do for [s=0:num_samples-1] {
    outfile = sprintf("y_study_s%d.eps", s)
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
