#include "graph.h"
#include <sstream>

void Graph::readGraph(std::istream& in) {
  std::string line;
  int n, m;
  std::stringstream ss;
  std::getline(in, line);
  std::getline(in, line);
  ss << line;
  ss >> n;

  std::vector<Node>(n).swap(nodes);

  m = 0;
  for (int i = 0; i < n; i++) {
    std::stringstream ss;
    int sid, tid;
    std::getline(in, line);
    ss << line;

    ss >> sid;
    sid = i; // stupid bug in go-uniprot
    nodes[sid].offset = m;
    ss.ignore();
    while(ss >> tid) {
      nodes[sid].out++;
      edges.push_back(tid);
      m++;
    }
  }
}

void Graph::writeGraph(std::ostream& out) {
  std::cout << "Nodes: " << nodes.size() << " Edges: " << edges.size() << std::endl;
  out << "graph_for_greach" << std::endl;
  out << nodes.size() << std::endl;

  for (unsigned int i = 0; i < nodes.size(); i++) {
    out << i << ": ";
    for (unsigned int j = 0; j < nodes[i].out; j++)
      out << edges[nodes[i].offset + j] << " ";
    out << "#" << std::endl;
  }
}
