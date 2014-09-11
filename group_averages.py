#! /usr/bin/env python
"""collapses file format left behind by average.py so latexify.py doesn't need to hold as much state."""


with open('average_grouped','w') as outf:
    with open('average', 'r') as inf:
        lastgraph = ''
        line = inf.readline()
        while line:
            parts = line.split()
            if lastgraph != parts[0]:
                outf.write('[' + parts[0] + '|' + parts[3] + '|' + parts[4] + ']\n')
                lastgraph = parts[0]
            outf.write(' '.join([parts[1],parts[2], parts[5]]) + '\n')
            line = inf.readline()


