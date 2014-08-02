#!/usr/bin/python3

import collections
import sys
import subprocess

algorithms = ["BFS", "DFS", "TLS", "RTLS", "RecMerge"]

def exec(algo, in_file):
    with open(in_file) as f:
        return subprocess.check_output(["./closure", algo, "list"], stdin=f).decode('ascii').split("\n")

output = [exec(algo, sys.argv[1]) for algo in algorithms]
lengths = list(map(len, output))
if len(set(lengths)) != 1:
    print("Differing output line counts: {}".format(lengths))
for lines in zip(*output):
    lines = [tuple(sorted(line.split())) for line in lines]
    if len(set(lines)) > 1:
        for line in set(lines):
            print("%s: %s" % ([algo for algo, line2 in zip(algorithms, lines) if line2 == line], line))
        sys.exit(42)
