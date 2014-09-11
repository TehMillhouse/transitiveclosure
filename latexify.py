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
import pprint

# table format:

algos = ['BFS','paraBFS','DFS','TLS','TLS64','WAR','RecMerge','RTLS']
fmts = ['count','array','matrix','list']

table_start = """\\begin{tabular}{ | l | c | %s | }
  Graph & $n$ & %s \\\\
    \hline
    \hline
    """ % (' | '.join(['c' for _ in algos]), ' & '.join(algos))

table_end = """  \end{tabular}
"""

data = {}

def newgraph():
    data = {}
    # fill in dummy values
    for a in algos:
        data[a] = {}
        for fmt in fmts:
            data[a][fmt] = 'N/A'
    return data

def get_data():
    global data
    with open('average_grouped', 'r') as inf:
        line = inf.readline()
        while line:
            if line.startswith('['):
                tmp = line[1:-2].split('|')
                name = tmp[0]
                n = tmp[1]
                m = tmp[2]
                data[name] = newgraph()
                data[name]['n'] = n
                data[name]['m'] = m
                line = inf.readline()
                continue
            tmp = line.split()
            data[name][tmp[0]][tmp[1]] = float(tmp[2])
            line = inf.readline()
    return data
        # pprint.pprint(json.dumps(data['large_real/citeseer.gra']), width=99999)

row_tmpl = '\\verb|%s| & %s & %s & %s & %s & %s & %s & %s & %s & %s \\\\'
def get_table():
    print(table_start)
    for name in data:
#       X | n | m | <algorithms>
# <graph> |   |   |

        graph = name
        n = data[name]['n']
        for fmt in fmts:
            print(row_tmpl % ( (graph, n) + tuple([('%.5f' % data[name][algo][fmt]) if data[name][algo][fmt] != 'N/A' else '\\verb|N/A|' for algo in algos ]) ) )
            graph = ' '
            n = ''
            m = ''
        print('\hline')
    print(table_end)


if __name__ == '__main__':
    get_data()
    get_table()
