set encoding iso_8859_1
set term postscript landscape enhanced "Times" 24
set key left top Left reverse samplen 4 width 10 spacing 0.95 title ""
    set output "time.eps"
    set title "Running times [ns]"
    set xlabel "{/Times-Italic n} [logarithmic scale]"
    set ylabel "time / ({/Times-Italic n} lg {/Times-Italic n})"
    set yrange [0.0:10.0]
    set xrange [1000.0:33554432.0]
    set logscale x
set format x '10^{%L}'
set pointsize 2.5
     plot 0 notitle, "std.log" using 1:2 title "introsort" with linespoints linetype 7 linewidth 3 pointtype 6
 
