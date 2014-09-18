#! /usr/bin/env python
"""Uses the out/sample/ files to generate latex tables with the results."""

import json
import os
import pprint
import sys
import collections

# table format:

algos = sorted(os.listdir('out/sample'))
fmts = ['matrix','array','list']

table_start = """\\adjustbox{max width=\columnwidth}{
\\begin{tabular}{ l c c | %s }
  Graph & $n$ & $m$ & %s \\\\
    \hline
    \hline
    """ % (' '.join(['c' for _ in algos]), ' & '.join(algos))

table_end = """  \end{tabular}
}
"""

class Graph(object):
    def __init__(self):
        self.n = 0
        self.m = 0
        self.samples = {}

def parse_graph(path):
    g = Graph()
    with open(path, 'r') as f:
        f.readline()
        f.readline()
        for line in f:
            g.n += 1
            g.m += len(line.split()) - 2
    return g

# {string: Graph}
graphs = {}

def parse_data(algo, lines):
    for line in lines:
        [graph, fmt, res] = line.split()
        if graph not in graphs:
            graphs[graph] = parse_graph(graph)
        g = graphs[graph]
        try:
            g.samples[(algo, fmt)] = float(res)
        except ValueError:
            g.samples[(algo, fmt)] = res

    for g in graphs.values():
        g.fastest = min([1000] + [time for time in g.samples.values() if type(time) is float])

def get_cell_text(graph, algo, fmt):
    res = graphs[graph].samples.get((algo, fmt), '-')
    if type(res) is float:
        string = '%.5f' % res
        if graphs[graph].fastest == res:
            return '\\textbf{%s}' % string
        else:
            return string
    else:
        return '\\verb|%s|' % res

def get_tables(graphs_per_table=None):
    print(table_start)
    graph_type = ''
    i = 0
    for name in sorted(list(graphs)):
#       X | n | m | <algorithms>
# <graph> |   |   |
        g = graphs[name]

        if graphs_per_table and i >= graphs_per_table:
            print(table_end)
            print(table_start)
            graph_type = ''
            i = 0
        graph = name.split('/')[-1][:-4]
        newtype = name.split('/')[-2]
        if newtype != graph_type:
            # emit a section header
            print('\\multicolumn{%s}{l}{\\textbf{%s}} \\\\' % (len(algos) + 3, newtype.replace('_',' ')))
            print('\hline')
        graph_type = newtype

        print(r'\verb|%s| & %s \\' % (graph, ' & '.join([str(g.n), str(g.m)] + [get_cell_text(name, algo, fmts[0]) for algo in algos])))
        for fmt in fmts[1:]:
            print(r'%s \\' % ' & '.join(3 * [''] + [get_cell_text(name, algo, fmt) for algo in algos]))
        print('\hline')
        i += 1
    print(table_end)

def main():
    for f in algos:
        parse_data(f, open('out/sample/' + f, 'r').readlines())
    get_tables(graphs_per_table=16)

if __name__ == '__main__':
    main()
