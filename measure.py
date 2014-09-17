#!/usr/bin/python3

import io
import os
import sys
import subprocess
import numpy
import gendag

sizes = {
    'density': [(int(2**i),2**16) for i in numpy.arange(10, 15, 0.5)],  # graph density comparison
    'sparse': [(2**i,2**(i+1)) for i in range(8, 17)]  # graph size comparison
}

def execute(algo, output_format, in_file):
    with open(in_file) as f:
        p = subprocess.Popen(["./closure", algo, output_format, "-seconds", "3", "-no-output"], stdin=f, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    return_code = p.wait()
    if return_code:
        raise Exception("{} returned code {}".format(algo, return_code))
    output = float(p.stderr.read().decode('utf-8').strip().split()[3])
    return output

def measure(scenario, algo):
    for n, m in sizes[scenario]:
        in_file = 'data/rand%sx%s.gra' % (n, m)
        if not os.path.exists(in_file):
            with open(in_file, 'w') as f:
                gendag.gengnm(n, m, f)
        time = execute(algo, "list", in_file)
        print('%s %s %s %s' % (n, m, time, time / n))

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage: measure.py <scenario> <algorithm>', file=sys.stderr)
        sys.exit(1)
    measure(sys.argv[1], sys.argv[2])
