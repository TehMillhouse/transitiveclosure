#include <fstream>
#include <string>
#include <ctime>
#include "graph.h"

template <class G>
int exec(std::string algo) {
  Graph *g = new Graph();
  g->readGraph(std::cin);
  std::cerr << "n = " << g->nodes.size() << ", m = " << g->edges.size() << std::endl;

  G *gOut;
  clock_t start = clock();
  if (algo == "BFS")
    gOut = g->breadthFirstSearch<G>();
  else if (algo == "DFS")
    gOut = g->depthFirstSearch<G>();
  else if (algo == "TLS")
    gOut = g->topologicalLevelSearch<G>();
  else {
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
