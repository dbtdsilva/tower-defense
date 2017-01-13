#!/usr/bin/gnuplot

set key off
set terminal qt
set title "Histogram of the execution time"
set xlabel "Execution time (us)"
set ylabel "Occurrences (quantity)"
set autoscale
binwidth=5
bin(x,width)=width*floor(x/width)

plot 'wcet_god_values.txt' using (bin($1,binwidth)):(1.0) smooth freq with boxes

pause -1
