#include "graph.h"
#include <sstream>
#include <stack>
#include <queue>

// Algorithm 4.3
void Graph::setLevel(Node &v) {
  for (int i = 0; i < v.out; i++) {
    Node &u = nodes[edges[v.offset + i]];
    u.level = std::max(u.level, v.level + 1);
    levels = std::max(levels, u.level + 1);
    u.visited++;
    if (u.visited == u.in)
      setLevel(u);
  }
}

// Algorithm 4.4
void Graph::setTopologicalLevels() {
  for (Node &v : nodes)
    if (v.in == 0)
      setLevel(v);
}

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
      nodes[tid].in++;
      edges.push_back(tid);
      m++;
    }
  }
}

void Graph::writeGraph(std::ostream& out) {
  out << "graph_for_greach" << std::endl;
  out << nodes.size() << std::endl;

  for (unsigned int i = 0; i < nodes.size(); i++) {
    out << i << ": ";
    for (unsigned int j = 0; j < nodes[i].out; j++)
      out << edges[nodes[i].offset + j] << " ";
    out << "#" << std::endl;
  }
}

void CountingGraph::writeGraph(std::ostream& out) {
  for (int edges : nodes)
    out << edges << " ";
  out << std::endl;
}
