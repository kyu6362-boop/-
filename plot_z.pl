# 潜在空間 z の2次元散布図
#
# 使い方:
#   gnuplot plot_z.pl
#
# 事前に以下の変数を環境に合わせて変更してください。

# --- 設定 ---
zfile      = "256_64_z.dat"   # z データファイル (sample z_idx value)
hidden_n   = 64               # 潜在空間の次元数
num_samples = 25              # サンプル数
outfile    = "z_scatter.png"  # 出力画像ファイル

width = int(sqrt(hidden_n + 0.5))
height = int(sqrt(hidden_n + 0.5))

# --- 出力設定 ---
set terminal pngcairo size 1600,1200 font "Arial,10"
set output outfile

set palette defined (0 "white", 0.5 "royalblue", 1 "red")
set cbrange [0:1]
set cblabel "z value"

cols = 5
rows = int((num_samples + cols - 1) / cols)

set multiplot layout rows,cols title "Latent Space z  (".zfile.")" font ",14"

do for [s=0:num_samples-1] {
    set title sprintf("Sample %d", s) font ",10"
    set xrange [-0.5:width-0.5]
    set yrange [-0.5:height-0.5]
    set xtics 0, 1
    set ytics 0, 1
    set size ratio 1
    plot zfile using (int($1)==s ? int($2) % width : 1/0) \
                    :(int($2) / width) \
                    :3 \
         with points pt 5 ps 1.5 palette notitle
}

unset multiplot
set output

print sprintf("saved: %s  (%d samples, %dx%d grid)", outfile, num_samples, width, height)
