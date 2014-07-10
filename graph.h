#pragma once

#include <iostream>
#include <vector>

struct Node {
  int offset; // offset into the edge array
  int in; // number of incoming edges
  int out; // number of outgoing edges
  int level; // topological level
  int visited;
  // edge indices [offset, offset + out) belong to this node
};

class Graph {
private:
  void pushNode();
  void pushEdge(int dest);
  void setLevel(Node&);
  void setTopologicalLevels();
public:
  std::vector<Node> nodes;
  std::vector<int> edges;
  int levels; // total number of topological levels
  void readGraph(std::istream&);
  void writeGraph(std::ostream&);
  Graph* topologicalLevelSearch();
};
