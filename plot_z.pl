# 潜在空間 z の2次元散布図
#
# hidden_n=2 の z データファイルを読み込み、
# z[0] を x 座標、z[1] を y 座標として全サンプルを散布図にプロットする。
#
# 使い方:
#   gnuplot plot_z.pl

# --- 設定 ---
zfile   = "256_2_z.dat"    # z データファイル (sample z_idx value)
outfile = "z_scatter.png"  # 出力画像ファイル

# --- 出力設定 ---
set terminal pngcairo size 800,800 font "Arial,12"
set output outfile

set title "Latent Space z  (".zfile.")" font ",14"
set xlabel "z[0]"
set ylabel "z[1]"
set size ratio 1
set grid

set palette defined (0 "royalblue", 12 "green", 24 "red")
set cblabel "sample"

# z_idx=0 の行で x を取得、z_idx=1 の行で y を取得し、1行にまとめる
plot "< awk 'NR%2==1{s=$1;x=$3} NR%2==0{print s,x,$3}' ".zfile \
     using 2:3:1 with points pt 7 ps 1.5 palette notitle

set output
print sprintf("saved: %s", outfile)
