# 学習用入力データ X_study の2次元画像表示
#
# 使い方:
#   gnuplot plot_study.pl

# --- 設定 ---
sample1    = 5    # ベースパターン数
variations = 5    # バリエーション数 (m1/2)
ih         = 16   # 画像の高さ
iw         = 16   # 画像の幅
outfile    = "X_study_images.eps"

# --- 出力設定 ---
set terminal postscript eps enhanced color font "Arial,10" size 10,10
set output outfile

set palette defined (0 "white", 1 "black")
set cbrange [0:1]
unset colorbox
unset key

set multiplot layout sample1, variations title "X study" font ",14"

do for [n=0:sample1-1] {
    do for [v=0:variations-1] {
        fname = sprintf("X_study_s%d_v%d.dat", n, v)
        set title sprintf("s%d v%d", n, v) font ",9"
        set xrange [-0.5:iw-0.5]
        set yrange [ih-0.5:-0.5]
        plot fname using 1:2:3 with image notitle
    }
}

unset multiplot
set output
print sprintf("saved: %s", outfile)
