reset
set xlabel 'F(n)'
set ylabel 'time(ns)'
set title 'Fibonacci runtime in userspace'
set term png
set output "uclient_picture.png"
set grid
plot [0:92][0:] \
"uclient_time" using 1:2 with linespoints linewidth 2  pointtype 7 pointsize 0.5 title "fast doubling", \
"" using 1:3 with linespoints linewidth 2 pointtype 7 pointsize 0.5 title "bitwise hack", 