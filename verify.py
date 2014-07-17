#!/usr/bin/python3

import os
import sys
import subprocess

algorithms = ["BFS", "DFS", "TLS", "TLS64", "WAR"]

def exec(algo, in_file):
    with open(in_file) as f:
        return subprocess.check_output(["./closure", algo, "matrix"], stdin=f).decode('ascii').split("\n")

output = [exec(algo, sys.argv[1]) for algo in algorithms]
lengths = list(map(len, output))
if len(set(lengths)) != 1:
    print("Differing output line counts: {}".format(lengths))
for lines in zip(*output):
    lines = [sorted(line.split()) for line in lines]
    for line in lines[1:]:
        if line != lines[0]:
            print(line)
            print(lines[0])
            os.exit(42)
