#include <fstream>
#include <string>
#include "graph.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cout << "Usage: hull <algorithm> <input file> <output file>" << std::endl;
    return 42;
  }

  Graph *g = new Graph();
  std::ifstream in(argv[2]);
  g->readGraph(in);
  std::cout << "n = " << g->nodes.size() << ", m = " << g->edges.size() << std::endl;
  std::ofstream out(argv[3]);

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

  gOut->writeGraph(out);
  std::cout << "n = " << gOut->nodes.size() << ", m = " << gOut->edges.size() << std::endl;
}
