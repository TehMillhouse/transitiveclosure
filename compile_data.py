#!/usr/bin/env python

from __future__ import print_function
import threading
import subprocess
import os
import time
import sys


algos = ['BFS','DFS','TLS','TLS64','WAR','RecMerge','RTLS']
fmts = ['count','array','matrix','list']

stop = False

def term_after_timeout(subproc, timeout):
  global stop
  for _ in range(timeout * 2):
    time.sleep(0.5)
    if stop:
      break
  if subproc.poll() is None:
    # hasn't exited yet
    subproc.terminate()
    print("<timeout> : ", end='')
  else:
    if subproc.returncode:
      print("<return code %s> : " % subproc.returncode, end='')
  sys.stdout.flush()

def result(algo, fmt, data):
  global stop
  try:
    with open(data) as infile:
      p = subprocess.Popen(['./closure', algo, fmt, '-no-output'],stdin=infile, stderr=subprocess.PIPE)
      t = threading.Thread(target=term_after_timeout, args=[p,20])
      t.start()
      p.wait()
      stop = True
      t.join()
      stop = False
      result = p.stderr.read()
      return data + ' ' + result
  except subprocess.CalledProcessError:
    # we expect this in many cases, just ignore it.
    return ""

def graphs():
  for root, _, files in os.walk('data'):
    for f in files:
      if f.endswith('.gra'):
        yield os.path.join(root, f)

safe_graphs = [
"data/small_real_sparse/kegg_dag_uniq.gra",
"data/small_real_sparse/mtbrv_dag_uniq.gra",
"data/small_real_sparse/ecoo_dag_uniq.gra",
"data/small_real_sparse/amaze_dag_uniq.gra",
"data/small_real_sparse/vchocyc_dag_uniq.gra",
"data/small_real_sparse/nasa_dag_uniq.gra",
"data/small_real_sparse/agrocyc_dag_uniq.gra",
"data/small_real_sparse/human_dag_uniq.gra",
"data/small_real_sparse/anthra_dag_uniq.gra",
"data/small_real_sparse/xmark_dag_uniq.gra",
# "data/rand1m/rand1m4.gra",
"data/rand1m/rand1m1.gra",
"data/rand1m/rand1m2.gra",
# "data/rand1m/rand1m32.gra",
# "data/rand1m/rand1m8.gra",
# "data/rand1m/rand1m16.gra",
"data/small_scalefree/ba10k5d.gra",
"data/small_scalefree/ba10k2d.gra",
# "data/kronecker_deg/kron_g500-log16-d1.gra",
# "data/kronecker_deg/kron_g500-log16-d16.gra",
# "data/kronecker_deg/kron_g500-log16-d32.gra",
# "data/kronecker_deg/kron_g500-log16-d8.gra",
# "data/kronecker_deg/kron_g500-log16-d64.gra",
# "data/kronecker_deg/kron_g500-log16-d4.gra",
# "data/kronecker_deg/kron_g500-log16-d2.gra",
"data/small_real_dense/citeseer-sub_sub_10720.gra",
"data/small_real_dense/go_sub_6793.gra",
"data/small_real_dense/yago_sub_6642.gra",
"data/small_real_dense/arxiv_sub_6000-1.gra",
"data/small_real_dense/pubmed_sub_9000-1.gra",
# "data/tfrand/rand60k8.gra",
# "data/tfrand/rand80k8.gra",
"data/tfrand/rand40k8.gra",
"data/tfrand/rand20k8.gra",
# "data/coPapers/coPapersDBLP.gra",
# "data/coPapers/coPapersCiteseer.gra",
# "data/large_random/random1m2x.gra",
# "data/large_random/random10m2x.gra",
# "data/large_random/random10m10x.gra",
# "data/large_random/random1m5x.gra",
# "data/large_random/random10m5x.gra",
# "data/large_random/random1m10x.gra",
# "data/stanford/email-EuAll.gra",
# "data/stanford/web-Google.gra",
# "data/stanford/wiki-Talk.gra",
"data/stanford/p2p-Gnutella31.gra",
# "data/stanford/soc-LiveJournal1.gra",
# "data/to_big/rand100m10.gra",
# "data/kronecker/kron_g500-log12.gra",
# "data/kronecker/kron_g500-log22.gra",
# "data/kronecker/kron_g500-log17.gra",
# "data/rand100k/rand100k64.gra",
# "data/rand100k/rand100k2.gra",
# "data/rand100k/rand100k1.gra",
# "data/rand100k/rand100k16.gra",
# "data/rand100k/rand100k4.gra",
# "data/rand100k/rand100k32.gra",
# "data/rand100k/rand100k8.gra",
# "data/huge_random/rand100m2.gra",
# "data/huge_random/rand100m5.gra",
# "data/kronecker_vary/kron_g500-log11.gra",
# "data/kronecker_vary/kron_g500-log12.gra",
# "data/kronecker_vary/kron_g500-log20.gra",
# "data/kronecker_vary/kron_g500-log19.gra",
# "data/kronecker_vary/kron_g500-log22.gra",
# "data/kronecker_vary/kron_g500-log23.gra",
# "data/kronecker_vary/kron_g500-log10.gra",
# "data/kronecker_vary/kron_g500-log15.gra",
# "data/kronecker_vary/kron_g500-log16.gra",
# "data/kronecker_vary/kron_g500-log14.gra",
# "data/kronecker_vary/kron_g500-log17.gra",
# "data/kronecker_vary/kron_g500-log21.gra",
# "data/kronecker_vary/kron_g500-log18.gra",
# "data/kronecker_vary/kron_g500-log13.gra",
# "data/rand_vary8/rand10000k8.gra",
# "data/rand_vary8/rand10k8.gra",
# "data/rand_vary8/rand1000k8.gra",
# "data/rand_vary8/rand100k8.gra",
# "data/rand_vary2/rand100k2.gra",
# "data/rand_vary2/rand100000k2.gra",
# "data/rand_vary2/rand10k2.gra",
# "data/rand_vary2/rand10000k2.gra",
# "data/rand_vary2/rand1000k2.gra",
# "data/large_real/uniprotenc-150m.gra",
"data/large_real/uniprotenc-22m.gra",
# "data/large_real/uniprotenc-100m.gra",
# "data/large_real/go-uniprot.gra",
# "data/large_real/cit-Patents.gra",
# "data/large_real/citeseerx.gra",
"data/large_real/citeseer.gra",
"data/rand10k/rand10k32.gra",
"data/rand10k/rand10k16.gra",
"data/rand10k/rand10k64.gra",
"data/rand10k/rand10k8.gra",
"data/rand10k/rand10k2.gra",
"data/rand10k/rand10k1.gra",
"data/rand10k/rand10k4.gra",
]

def graphs2():
  # laziness enabling function
  for i in safe_graphs:
    yield i


for fmt in fmts:
  for algo in algos:
    for graph in graphs2():
      print(result(algo, fmt, graph))
      sys.stdout.flush()
