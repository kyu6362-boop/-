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

group_size = 5  # グループあたりのサンプル数

# z_idx=0 の行で x を取得、z_idx=1 の行で y を取得し、1行にまとめる
# 出力: sample x y group
cmd = "< awk 'NR%2==1{s=$1;x=$3} NR%2==0{print s,x,$3,int(s/".sprintf("%d", group_size).")}' ".zfile

set key outside right

plot cmd using 2:3:(int($4)==0 ? 1 : 1/0) with points pt 7 ps 1.5 lc rgb "royalblue"  title "sample 0-4", \
     cmd using 2:3:(int($4)==1 ? 1 : 1/0) with points pt 7 ps 1.5 lc rgb "forest-green" title "sample 5-9", \
     cmd using 2:3:(int($4)==2 ? 1 : 1/0) with points pt 7 ps 1.5 lc rgb "red"         title "sample 10-14", \
     cmd using 2:3:(int($4)==3 ? 1 : 1/0) with points pt 7 ps 1.5 lc rgb "orange"      title "sample 15-19", \
     cmd using 2:3:(int($4)==4 ? 1 : 1/0) with points pt 7 ps 1.5 lc rgb "dark-violet" title "sample 20-24"

set output
print sprintf("saved: %s", outfile)
