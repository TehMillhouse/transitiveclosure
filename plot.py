#!/usr/bin/python3

import io
import os
import sys
import subprocess
import time
from matplotlib import pyplot
import numpy

import gendag

def plot_files(files, titles):
    cmd = []
    output_png = False
    for i in range(len(files)):
        cmd.append('"%s" using 1:4 title "%s" with linespoints' % (files[i], titles[i]))
    plot = subprocess.Popen(['gnuplot'], stdin=subprocess.PIPE)
    real_cmd =  ('set xtics (1024, 2048, 4096, 8192, 16384, 32768, 65536)\n'
                 'set ylabel "Time / Node [s]"\n'
                 'set grid\n'
                 'set xlabel "Nodes"\n'
                 + ('set term pngcairo size 1024, 768\nset output "graph.png"\n' if output_png else '')
                 + 'plot '
                 + ', '.join(cmd) + '\n'
                )
    print(real_cmd)
    plot.stdin.write(real_cmd.encode('ascii'))
    plot.stdin.flush()
    plot.wait()

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: plot.py <scenario>')
        exit()
    base = 'out/' + sys.argv[1]
    files = os.listdir(base)
    plot_files([os.path.join(base, f) for f in files], files)
