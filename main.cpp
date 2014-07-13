#include <fstream>
#include <string>
#include <ctime>
#include "graph.h"

template <class G>
int exec(std::string algo) {
  Graph *g = new Graph();
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
    auto nodeCounts = g->bitParallelTopologicalLevelSearch();
    std::cerr << double(clock() - start) / CLOCKS_PER_SEC << "s" << std::endl;
    for (int i : nodeCounts)
      std::cout << i << " ";
    std::cout << std::endl;
    return 0;
  } else {
    std::cout << "Unknown algorithm" << std::endl;
    return 43;
  }
  std::cerr << double(clock() - start) / CLOCKS_PER_SEC << "s" << std::endl;

  gOut->writeGraph(std::cout);
  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3) {
    std::cout << "Usage: closure <algorithm> [-small_output]" << std::endl;
    return 42;
  }

  if (argc == 3)
    return exec<CountingGraph>(argv[1]);
  else
    return exec<Graph>(argv[1]);
}
