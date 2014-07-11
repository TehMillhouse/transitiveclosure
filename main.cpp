#include <fstream>
#include "graph.h"

int main(int argc, char **argv) {
  if (argc != 3)
    return 42;

  Graph *g = new Graph();
  std::ifstream in(argv[1]);
  g->readGraph(in);
  std::cout << "n = " << g->nodes.size() << ", m = " << g->edges.size() << std::endl;
  std::ofstream out(argv[2]);
  Graph *gOut = g->depthFirstSearch();
  gOut->writeGraph(out);
  std::cout << "n = " << gOut->nodes.size() << ", m = " << gOut->edges.size() << std::endl;
}
