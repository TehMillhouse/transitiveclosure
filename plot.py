#!/usr/bin/python3

import io
import os
import sys
import subprocess
import time
from matplotlib import pyplot
import numpy

import gendag

def gen_dag(n, m):
    f = io.StringIO()
    gendag.gengnm(n, m, f)
    return f.getvalue()

def init():
    global algorithms, sizes, graphs
    algorithms = [("TLS", "count"), ("TLS", "array"), ("TLS", "matrix")]
    sizes = [(int(2**i),2**17) for i in numpy.arange(10, 16, 0.5)]
    #sizes = [(2**i,2**(i+1)) for i in range(8, 20)]
    graphs = [(n, gen_dag(n, m)) for n, m in sizes]

def execute(algo, g):
    start = time.time()
    p = subprocess.Popen(["./closure", algo[0], algo[1]], stdin=subprocess.PIPE, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    p.stdin.write(g.encode('ascii'))
    p.stdin.flush()
    return_code = p.wait()
    if return_code:
        raise Exception("{} returned code {}".format(algo, return_code))
    output = p.stderr.read()
    return output

plot_debug = False
def bake_files(algorithms, graphs):
    for algo in algorithms:
        if plot_debug:
            r = """1024  0.107248
            1448  0.165722
            2048  0.263774
            2896  0.424517
            EOF
            1024  0.114945
            1448  0.174251
            2048  0.279616
            2896  0.468908
            EOF
            1024  0.10936
            1448  0.167379
            2048  0.265336
            2896  0.42262
            EOF
            """
        else:
            r = [execute(algo, g[1]).decode('utf-8') for g in graphs] + ['EOF\n']
        with open('out/' + str(algo) + '_' + str(time.time()), 'w') as out:
            out.write(''.join(r))

def plot_files(files, titles):
    cmd = []
    output_png = False
    for i in range(len(files)):
        cmd.append('"%s" using ($1):($2 / $1) title "%s" with linespoints' % (files[i], titles[i]))
    plot = subprocess.Popen(['gnuplot'], stdin=subprocess.PIPE)
    real_cmd =  ('set xtics (1024, 2048, 4096, 8192, 16384, 32768, 65536)\n'
                 'set ylabel "Time / Node [s]"\n'
                 'set grid\n'
                 'set xlabel "Nodes"\n'
                 + ('set term pngcairo\nset output "graph.png"\n' if output_png else '')
                 + 'plot '
                 + ', '.join(cmd) + '\n'
                )
    print(real_cmd)
    plot.stdin.write(real_cmd.encode('ascii'))
    plot.stdin.flush()
    plot.wait()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("""Usage: `plot.py bake` to create gnuplot files.
       `plot.py FILE1 [FILE2 [FILE3...]]` to plot files.""")
        exit()
    init()
    if sys.argv[1] == 'bake':
        print('creating gnuplot files...')
        bake_files(algorithms, graphs)
    else:
        plot_files(sys.argv[1:], sys.argv[1:])
