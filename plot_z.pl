# 潜在空間 z の2次元散布図
#
# hidden_n=2 の z データファイルを読み込み、
# z[0] を x 座標、z[1] を y 座標として全サンプルを散布図にプロットする。
#
# 使い方:
#   gnuplot -e "zfile='study_64_2_z.dat'; outfile='z_scatter.eps'; group_size=5; outdir='results/images/z/'" plot_z.pl

# --- デフォルト値 (run_experiment.c から -e で上書きされる) ---
if (!exists("zfile"))      zfile      = "study_64_2_z.dat"
if (!exists("outfile"))    outfile    = "z_scatter.eps"
if (!exists("group_size")) group_size = 5
if (!exists("outdir"))     outdir     = ""

# --- 出力設定 ---
set terminal postscript eps enhanced color font "Arial,16"
set output outdir.outfile

set title "Latent Space z  (".zfile.")" font ",14"
set xlabel "z[0]"
set ylabel "z[1]"
set size ratio 1
set grid

# z_idx=0 の行で x を取得、z_idx=1 の行で y を取得し、1行にまとめる
# 出力: sample x y group
cmd = "< awk 'NR%2==1{s=$1;x=$3} NR%2==0{print s,x,$3,int(s/".sprintf("%d", group_size).")}' ".zfile

set key outside right

plot cmd using (int($4)==0 ? $2 : 1/0):3 with points pt 7 ps 1.5 lc rgb "royalblue"    title "sample 0-4", \
     cmd using (int($4)==1 ? $2 : 1/0):3 with points pt 7 ps 1.5 lc rgb "forest-green" title "sample 5-9", \
     cmd using (int($4)==2 ? $2 : 1/0):3 with points pt 7 ps 1.5 lc rgb "red"          title "sample 10-14", \
     cmd using (int($4)==3 ? $2 : 1/0):3 with points pt 7 ps 1.5 lc rgb "orange"       title "sample 15-19", \
     cmd using (int($4)==4 ? $2 : 1/0):3 with points pt 7 ps 1.5 lc rgb "dark-violet"  title "sample 20-24"

set output
print sprintf("saved: %s%s", outdir, outfile)
