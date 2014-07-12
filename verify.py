#!/usr/bin/python3

import os
import sys
import subprocess

algorithms = ["BFS", "DFS", "TLS"]

in_file = sys.argv[1]
output = [subprocess.check_output(["./hull", algo, in_file]).decode('ascii').split("\n") for algo in algorithms]
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
