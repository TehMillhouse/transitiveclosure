#include <fstream>
#include <string>
#include <chrono>
#include <memory>
#include <omp.h>
#include "graph.h"
#include <ConstructionGraph.hh>

template <class G>
int exec(std::string algo, bool output, double seconds) {
  AdjacencyArrayGraph *g = new AdjacencyArrayGraph(0);
  g->readGraph(std::cin);

  std::unique_ptr<G> gOut;
  auto start = std::chrono::high_resolution_clock::now();
  double runtime;
  int iterations = 0;
  do {
    if (algo == "BFS") {
      gOut = g->breadthFirstSearch<G>();
    } else if (algo == "paraBFS") {
      gOut = g->parallelBFS<G>();
    } else if (algo == "DFS") {
      gOut = g->depthFirstSearch<G>();
    } else if (algo == "TLS") {
      gOut = g->topologicalLevelSearch<G>();
    } else if (algo == "TLS64") {
      gOut = g->bitParallelTopologicalLevelSearch<G>();
    } else if (algo == "WAR") {
      gOut = g->warshallALgorithm<G>();
    } else if (algo == "RecMerge") {
      gOut = g->recursiveMerge<G>();
    } else if (algo == "RTLS") {
      gOut = g->reverseTopologicalLevelSearch<G>();
    } else if (algo == "paraRTLS") {
      gOut = g->paraRTLS<G>();
    } else {
      std::cout << "Unknown algorithm" << std::endl;
      return 43;
    }
    auto end = std::chrono::high_resolution_clock::now();
    runtime = std::chrono::duration<double>(end-start).count();
    iterations++;
  } while (runtime < seconds);

  // output format feasible for gnuplot:
  // <#nodes>  <time>
  std::cerr << g->nodes.size() << "  ";
  std::cerr << runtime / iterations << std::endl;

  if (output)
    gOut->writeGraph(std::cout);
  return 0;
}

int execPreach(double seconds) {
  preach::ConstructionGraph cg;
  cg.read(std::cin);
  auto start = std::chrono::high_resolution_clock::now();
  double runtime;
  // we're only comparing the construction time for preach graphs
  std::unique_ptr<preach::QueryGraph> qg = cg.construct();
  cg.qg.reset();

  auto end = std::chrono::high_resolution_clock::now();
  runtime = std::chrono::duration<double>(end-start).count();

  // output format feasible for gnuplot:
  // <#nodes>  <time>
  std::cerr << cg.n << "  ";
  std::cerr << runtime << std::endl;

  return 0;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << "Usage: closure <algorithm> <output format> [-threads <num-threads>] [-seconds <num>] [-no-output]" << std::endl;
    return 42;
  }

  std::string format = argv[2];
  double seconds = 0;
  bool output = true;

  for (int i = 3; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "-threads") {
      omp_set_num_threads(atoi(argv[++i]));
    } else if (arg == "-seconds") {
      seconds = atof(argv[++i]);
    } else if (arg == "-no-output") {
      output = false;
    } else {
      std::cerr << "Unknown flag: " << arg << std::endl;
      return 42;
    }
  }

  std::cerr << argv[1] << "  " << format << "  ";
  std::string algo = argv[1];
  if (algo == "preach") {
    return execPreach(seconds);
  }
  if (format == "count")
    return exec<CountingGraph>(argv[1], output, seconds);
  else if (format == "array")
    return exec<AdjacencyArrayGraph>(argv[1], output, seconds);
  else if (format == "matrix")
    return exec<AdjacencyMatrixGraph>(argv[1], output, seconds);
  else if (format == "list")
    return exec<AdjacencyListGraph>(argv[1], output, seconds);
}
