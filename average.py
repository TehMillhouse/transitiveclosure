#! /usr/bin/env python
"""Collects and averages all run results from files written by cleanup.py"""

import os

runs = len(list(filter(lambda x: x.startswith('o_run_'), os.listdir())))

fs = [open('o_run_%s' % i,'r') for i in range(1,runs + 1)]
buf = []

with open('average','w') as outfile:
    lines = [fs[i].readline().split() for i in range(len(fs))]
    while all([split != [] for split in lines]):
        if all([lines[i][0] != "<timeout>" for i in range(len(lines))]):
            line_start = ' '.join(lines[0][:-1])  # everything but the time
            times = [float(lines[i][-1]) for i in range(len(lines))]
            avg = str(sum(times) / len(times))
            print(avg)
            buf.append(line_start + ' ' + avg + '\n')
            # outfile.write(line_start + ' ' + avg + '\n')
        else:
            assert len([lines[i][0] == "<timeout>" for i in range(len(lines))]) == len(lines)
        lines = [fs[i].readline().split() for i in range(len(fs))]
    buf.sort()
    outfile.writelines(buf)
