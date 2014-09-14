#! /usr/bin/env python
"""Uses the average_grouped file to generate a big latex table with the results.

For the record, the complete pipeline to generate these tables is as follows:
% ./run_all.py > run_1; ./cleanup.py; ./average.py; ./group_averages.py; ./latexify.py

Yes, I have a bad habit of splitting my logic into a ton of different files, sorry about that.
Whereas run_all.py is python2-only, as that's what we've got on the institute server

Also, the generated table is so huge it doesn't fit on a single page. I suggest manually
breaking it apart later.
"""


import json
import os
import pprint
import sys

# table format:

algos = ['BFS','paraBFS','DFS','TLS','TLS64','WAR','RecMerge','RTLS']
fmts = ['array','matrix','list']

table_start = """\\adjustbox{max width=\columnwidth}{
\\begin{tabular}{ l | c %s }
  Graph & $n$ & %s \\\\
    \hline
    \hline
    """ % (' '.join(['c' for _ in algos]), ' & '.join(algos))

table_end = """  \end{tabular}
}
"""


run_files = filter(lambda x : x.startswith('run_'), os.listdir())

def cleanup(line):
    if line == '\n' or line.startswith('<return code') or line == 'Unknown algorithm\n' or line.startswith('<timeout'):
        return None
    else:
        return line


def group(lines):
    outf = []
    lastgraph = ''
    for line in sorted(lines):
        parts = line.split()
        if lastgraph != parts[0]:
            outf.append('[' + parts[0] + '|' + parts[3] + ']\n')
            lastgraph = parts[0]
        outf.append(' '.join([parts[1],parts[2], parts[4]]) + '\n')
    return outf


data = {}

def newgraph():
    data = {}
    # fill in dummy values
    for a in algos:
        data[a] = {}
        for fmt in fmts:
            data[a][fmt] = 'N/A'
    return data


def parse_data(lines):
    global data
    for line in lines:
        if line.startswith('['):
            tmp = line[1:-2].split('|')
            name = tmp[0]
            n = tmp[1]
            data[name] = newgraph()
            data[name]['n'] = n
            continue
        tmp = line.split()
        data[name][tmp[0]][tmp[1]] = float(tmp[2])
    # calculate which (algorithm, output) pair was fastest
    # yes, I'm a dirty hackjob.
    for graph in data:
        fastest_time = 999999
        for algo in algos:
            for fmt in fmts:
                # print(type(data[graph][algo][fmt]), file=sys.stderr)
                if type(data[graph][algo][fmt]) is float and data[graph][algo][fmt] < fastest_time:
                    data[graph]['fastest'] = (algo, fmt)
                    fastest_time = data[graph][algo][fmt]
    return data
        # pprint.pprint(json.dumps(data['large_real/citeseer.gra']), width=99999)



def get_cell_text(graph, algo, fmt):
    if data[graph][algo][fmt] == 'N/A':
        return '\\verb|N/A|'
    string = '%.5f' % data[graph][algo][fmt]
    if 'fastest' in data[graph] and data[graph]['fastest'] == (algo, fmt):
        # print(data[graph]['fastest'], file=sys.stderr)
        return '\\textbf{%s}' % string
    return string



row_tmpl = '\\verb|%s| & %s & %s & %s & %s & %s & %s & %s & %s & %s \\\\'
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

        n = data[name]['n']
        for fmt in fmts:

            print(row_tmpl % ( (graph, n) + tuple([get_cell_text(name, algo, fmt) for algo in algos ]) ) )
            graph = ' '
            n = ''
            m = ''
        print('\hline')
        i += 1
    print(table_end)


if __name__ == '__main__':
    buf = []
    for f in run_files:
        for l in open(f, 'r'):
            l = cleanup(l)
            if l:
                buf.append(l)
    buf = group(buf)
    parse_data(buf)
    get_tables(graphs_per_table=16)
