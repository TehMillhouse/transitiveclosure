#!/usr/bin/python3

import io
import os
import sys
import subprocess
import time
from matplotlib import pyplot

import gendag

algorithms = ["BFS", "DFS", "TLS"]
nodes = [2**i for i in range(10, 17)]
densities = [0.2*i for i in range(1, 11)]

def gen_dag(n, m):
    f = io.StringIO()
    gendag.gengnm(n, m, f)
    return f.getvalue()

graphs = [(n, gen_dag(n, 2*n)) for n in nodes]

def execute(algo, g):
    start = time.time()
    p = subprocess.Popen(["./closure", algo], stdin=subprocess.PIPE, stdout=subprocess.DEVNULL)
    p.stdin.write(g.encode('ascii'))
    p.wait()
    return time.time() - start

for algo in algorithms:
    n = 30000
    pyplot.plot(nodes, [execute(algo, g)/n for n, g in graphs], label=algo)
pyplot.legend()
pyplot.show()
