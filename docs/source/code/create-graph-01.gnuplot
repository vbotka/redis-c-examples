#!/usr/bin/gnuplot
set terminal png 
set output 'graph-01.png'
set style data points
set datafile separator ";"
set xdata time
set timefmt "%Y-%m-%d %H:%M"
set xlabel "date"
set ylabel "score"
set format x "%d.%m"
set autoscale
set key top right
plot "install.csv" using 1:2 title "Install" lt rgbcolor "red", \
     "upgrade.csv" using 1:2 title "Upgrade" lt rgbcolor "green", \
     "remove.csv" using 1:2 title "Remove" lt rgbcolor "blue"
