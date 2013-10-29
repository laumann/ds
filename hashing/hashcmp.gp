set encoding utf8
#iso_8859_1
set term postscript landscape enhanced "Times" 24
set autoscale
set xtic auto
set ytic auto
set xrange [150000:1600000]
set key left top Left reverse samplen 4 width 10 spacing 0.95 title ""
set output "hashcmp.eps"
set title "Running times [s]"
set xlabel "size (bytes)" 
set ylabel "time"
set format x '10^{%L}'
plot "std.new.log" using 1:2 title "std" smooth unique with linespoints pt 3, \
     "dshash.new.log" using 1:2 title "DPSH" smooth unique w linespoints pt 4, \
     "dshash_wrapped.new.log" using 1:2 title "WPSH" smooth unique w linespoints pt 5
 
