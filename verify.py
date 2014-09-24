#!/usr/bin/python3

import collections
import sys
import subprocess
import algorithms

def exec(algo, fmt, in_file):
    with open(in_file) as f:
        return [frozenset(line.split()) for line in subprocess.check_output(["./closure", algo, fmt, "-seconds", "1"], stdin=f).decode('ascii').split("\n")]

output = {(algo, fmt): exec(algo, fmt, sys.argv[1]) for algo, fmts in algorithms.algorithms.items() for fmt in fmts}
lengths = list(map(len, output.values()))
if len(set(lengths)) != 1:
    print("Differing output line counts: {}".format(lengths))
    sys.exit(42)
for i in range(lengths[0]):
    lines = collections.defaultdict(list)
    for key, out in output.items():
        lines[out[i]].append(key)
    if len(lines) > 1:
        for line, keys in lines.items():
            print("%s: %s" % (keys, line))
        sys.exit(42)
