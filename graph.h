#pragma once

#include <iostream>
#include <vector>

struct Node {
  int offset; // offset into the edge array
  int out; // number of outgoing edges
};

class Graph {
public:
  std::vector<Node> nodes;
  std::vector<int> edges;
  void readGraph(std::istream&);
  void writeGraph(std::ostream&);
};
