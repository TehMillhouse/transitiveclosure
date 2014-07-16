#!/usr/bin/python3

import io
import os
import sys
import subprocess
import time
from matplotlib import pyplot
import numpy

import gendag

algorithms = [("TLS", "count"), ("TLS", "array"), ("TLS", "matrix")]
sizes = [(int(2**i),2**17) for i in numpy.arange(10, 16, 0.5)]
#sizes = [(2**i,2**(i+1)) for i in range(8, 20)]

def gen_dag(n, m):
    f = io.StringIO()
    gendag.gengnm(n, m, f)
    return f.getvalue()

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
    print(output)
    return float(output[:-2])

for algo in algorithms:
    pyplot.plot([n for n, g in graphs], [execute(algo, g)/n for n, g in graphs], label=str(algo))
pyplot.ylabel('Time / Number of nodes')
pyplot.xlabel('Number of nodes')
pyplot.legend()
pyplot.show()
