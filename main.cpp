#include <fstream>
#include <string>
#include "graph.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: closure <algorithm>" << std::endl;
    return 42;
  }

  Graph *g = new Graph();
  g->readGraph(std::cin);
  std::cerr << "n = " << g->nodes.size() << ", m = " << g->edges.size() << std::endl;

  std::string algo = argv[1];
  Graph *gOut;
  if (algo == "BFS")
    gOut = g->breadthFirstSearch();
  else if (algo == "DFS")
    gOut = g->depthFirstSearch();
  else if (algo == "TLS")
    gOut = g->topologicalLevelSearch();
  else {
    std::cout << "Unknown algorithm" << std::endl;
    return 43;
  }

  gOut->writeGraph(std::cout);
  std::cerr << "n = " << gOut->nodes.size() << ", m = " << gOut->edges.size() << std::endl;
}
