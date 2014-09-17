#! /usr/bin/env python
"""Uses the out/sample/ files to generate latex tables with the results."""

import json
import os
import pprint
import sys
import collections

# table format:

algos = os.listdir('out/sample')
fmts = ['matrix','array','list']

table_start = """\\adjustbox{max width=\columnwidth}{
\\begin{tabular}{ l | c %s }
  Graph & $n$ & %s \\\\
    \hline
    \hline
    """ % (' '.join(['c' for _ in algos]), ' & '.join(algos))

table_end = """  \end{tabular}
}
"""

# {graph: (algo, format): time}
data = collections.defaultdict(dict)
# {graph: time}
fastest = {}
# {graph: int}
graph_n = {}

def parse_data(algo, lines):
    for line in lines:
        [graph, fmt, n, res] = line.split()
        graph_n[graph] = n
        try:
            data[graph][(algo, fmt)] = float(res)
        except ValueError:
            data[graph][(algo, fmt)] = res

    for graph in data:
        fastest[graph] = min(data[graph][(algo, fmt)]
                             for algo in algos
                             for fmt in fmts
                             if type(data[graph].get((algo, fmt))) is float)

def get_cell_text(graph, algo, fmt):
    res = data[graph].get((algo, fmt), '-')
    if type(res) is float:
        string = '%.5f' % res
        if fastest[graph] == res:
            return '\\textbf{%s}' % string
        else:
            return string
    else:
        return '\\verb|%s|' % res

def get_tables(graphs_per_table=None):
    print(table_start)
    graph_type = ''
    i = 0
    for name in sorted(list(data)):
#       X | n | m | <algorithms>
# <graph> |   |   |
        if graphs_per_table and i >= graphs_per_table:
            print(table_end)
            print(table_start)
            graph_type = ''
            i = 0
        graph = name.split('/')[-1][:-4]
        newtype = name.split('/')[-2]
        if newtype != graph_type:
            # emit a section header
            print('\\multicolumn{%s}{l}{\\textbf{%s}} \\\\' % (len(algos) + 2, newtype.replace('_',' ')))
            print('\hline')
        graph_type = newtype

        n = graph_n[name]
        for fmt in fmts:
            print(r'\verb|%s| & %s \\' % (graph, ' & '.join([n] + [get_cell_text(name, algo, fmt) for algo in algos])))
            graph = ' '
            n = ''
        print('\hline')
        i += 1
    print(table_end)

if __name__ == '__main__':
    for f in algos:
        parse_data(f, open('out/sample/' + f, 'r').readlines())
    get_tables(graphs_per_table=16)
