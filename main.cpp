#include <fstream>
#include <string>
#include <ctime>
#include "graph.h"

template <class G>
int exec(std::string algo) {
  AdjacencyArrayGraph *g = new AdjacencyArrayGraph(0);
  g->readGraph(std::cin);

  G *gOut;
  clock_t start = clock();
  if (algo == "BFS")
    gOut = g->breadthFirstSearch<G>();
  else if (algo == "DFS")
    gOut = g->depthFirstSearch<G>();
  else if (algo == "TLS")
    gOut = g->topologicalLevelSearch<G>();
  else if (algo == "TLS64") {
    gOut = g->bitParallelTopologicalLevelSearch<G>();
  } else {
    std::cout << "Unknown algorithm" << std::endl;
    return 43;
  }
  // output format feasible for gnuplot:
  // <#nodes>  <time / #nodes>
  std::cerr << g->nodes.size() << "  ";
  std::cerr << double(clock() - start) / CLOCKS_PER_SEC << std::endl;

  gOut->writeGraph(std::cout);
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "Usage: closure <algorithm> <output format>" << std::endl;
    return 42;
  }

  std::string format = argv[2];
  if (format == "count")
    return exec<CountingGraph>(argv[1]);
  else if (format == "array")
    return exec<AdjacencyArrayGraph>(argv[1]);
  else if (format == "matrix")
    return exec<AdjacencyMatrixGraph>(argv[1]);
}
