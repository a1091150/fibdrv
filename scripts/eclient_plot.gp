reset
set xlabel 'ith read'
set ylabel 'time(ns)'
set title 'Fibonacci driver read'
set term png
set output "eclient_picture.png"
set grid
plot [0:100][0:] \
"eclient_time" with linespoints linewidth 2 pointtype 7 pointsize 0.5 title "fast doubling"