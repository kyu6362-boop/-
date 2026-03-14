# 未学習データの出力 y の2次元画像表示
#
# 使い方:
#   gnuplot plot_y_unseen.pl

# --- 設定 ---
yfile       = "256_64_y_unseen.dat"  # 未学習データの出力yファイル (s idx value)
input_n     = 256   # 入力層サイズ
hidden_n    = 64    # 中間層サイズ
num_samples = 25    # サンプル数
outfile     = "y_unseen_images.eps"

width  = int(sqrt(input_n + 0.5))
height = int(sqrt(input_n + 0.5))

# --- 出力設定 ---
set terminal postscript eps enhanced color font "Arial,10" size 12,10
set output outfile

set palette defined (0 "white", 1 "black")
set cbrange [0:1]
unset colorbox
unset key

cols = 5
rows = int((num_samples + cols - 1) / cols)

set multiplot layout rows, cols \
    title sprintf("Output y (unseen)  (%d \\rightarrow %d)", input_n, hidden_n) font ",14"

do for [s=0:num_samples-1] {
    set title sprintf("sample %d", s) font ",9"
    set xrange [-0.5:width-0.5]
    set yrange [height-0.5:-0.5]
    plot yfile using (int($1)==s ? int($2) % width : 1/0) \
                    :(int($2) / width) \
                    :3 \
         with image notitle
}

unset multiplot
set output
print sprintf("saved: %s", outfile)
