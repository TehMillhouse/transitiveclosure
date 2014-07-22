#!/usr/bin/python3

import io
import os
import sys
import subprocess
import numpy
import gendag

sizes = [(int(2**i),2**16) for i in numpy.arange(10, 15, 0.5)]  # graph density comparison
#sizes = [(2**i,2**(i+1)) for i in range(8, 20)]  # graph size comparison

def execute(algo, output_format, in_file):
    with open(in_file) as f:
        p = subprocess.Popen(["./closure", algo, output_format], stdin=f, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    return_code = p.wait()
    if return_code:
        raise Exception("{} returned code {}".format(algo, return_code))
    output = float(p.stderr.read().decode('utf-8').strip().split()[1][:-1])
    return output

def measure(algo):
    for n, m in sizes:
        in_file = 'data/rand%sx%s.gra' % (n, m)
        if not os.path.exists(in_file):
            with open(in_file, 'w') as f:
                gendag.gengnm(n, m, f)
        time = execute(algo, "matrix", in_file)
        print('%s %s %s %s' % (n, m, time, time / n))

if __name__ == '__main__':
    measure(sys.argv[1])
