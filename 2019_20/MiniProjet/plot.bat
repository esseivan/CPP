cd "C:\Program Files\gnuplot\bin\"
copy "C:\WorkSpace\ELE_BA1_INFO\CPP\2019_20\MiniProjet\output\output_c.txt" "out.dat"
gnuplot -e "plot 'out.dat' using 1:2 with lines; pause 4"