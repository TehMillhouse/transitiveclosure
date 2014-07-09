#pragma once

#include <iostream>
#include <vector>

struct Node {
  int offset; // offset into the edge array
  int out; // number of outgoing edges
};

class Graph {
private:
  void addNode(int);
  void addEdge(int, int);
  void strTrimRight(std::string&);
  int addEdgeLine(std::string&, int);
  void addNodeBuild(int);

  // Refinement
  int LevelBackDfs(int, int);
  int LevelDfs(int, int);
  int OrderBackDfs(int, int);
  int OrderDfs(int, int);
public:
  std::vector<Node> nodes;
  std::vector<int> edges;
  void readGraph(std::istream&);
  void writeGraph(std::ostream&);
};
