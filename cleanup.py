#! /usr/bin/env python
"""Cleans up the messy file format left behind by run_all.py"""

import os

run_files = filter(lambda x : x.startswith('run_'), os.listdir())
buf = []

for f in run_files:
    with open(f, 'r') as inf:
        with open('o_' + f, 'w') as outf:
            for line in inf:
                if line == '\n' or line.startswith('<return code') or line == 'Unknown algorithm\n':
                    continue
                # line contains actual data
                buf.append(line)
#            buf.sort()
            outf.writelines(buf)
