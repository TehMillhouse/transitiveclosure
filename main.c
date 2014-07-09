#include <fstream>
#include "graph.h"

int main(int argc, char **argv) {
  if (argc != 2)
    return 42;

  Graph *g = new Graph();
  std::ifstream in(argv[1]);
  g->readGraph(in);
  std::cout << "n = " << g->nodes.size() << ", m = " << g->edges.size() << std::endl;
}
