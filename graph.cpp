#include "graph.h"
#include <sstream>
#include <stack>

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

Graph* Graph::depthFirstSearch() {
  Graph *ret = new Graph();
  std::vector<Node*> stack;

  for (Node &s : nodes) {
    ret->pushNode();
    stack.push_back(&s);
    for (Node &v : nodes) v.visited = 0;

    while (stack.size()) {
      Node *v = stack[stack.size()-1];
      stack.pop_back();
      if (v != &s)
        ret->pushEdge(v - &nodes[0]);
      for (int i = 0; i < v->out; i++) {
        Node &u = nodes[edges[v->offset + i]];
        if (!u.visited) {
          stack.push_back(&u);
          u.visited = 1;
        }
      }
    }
    stack.clear();
  }
  return ret;
}

Graph* Graph::breadthFirstSearch() {
  Graph *ret = new Graph();
  std::vector<Node*> stack;

  for (Node &s : nodes) {
    ret->pushNode();
    stack.push_back(&s);
    for (Node &v : nodes) v.visited = 0;

    while (stack.size()) {
      Node *v = stack[stack.size()-1];
      stack.pop_back();
      if (v != &s)
        ret->pushEdge(v - &nodes[0]);
      for (int i = 0; i < v->out; i++) {
        Node &u = nodes[edges[v->offset + i]];
        if (!u.visited) {
          stack.push_back(&u);
          u.visited = 1;
        }
      }
    }
    stack.clear();
  }
  return ret;
}

Graph* Graph::topologicalLevelSearch() {
  //TLS
  //Find topological levels
  //Foreach s in V do
  //  next[level(s)] := {s}
  //  For l:= level(s)+1 to L do // L is # of topological levels
  //    next[l] := {} // stacks
  //  For l:= level(s) to L do
  //    For each v in next(s) do
  //      reached := {v}; next := {}
  //      For each (v,u) in E do
  //        if u was not yet reached then
  //          next[level(u)].push(u)

  setTopologicalLevels();
  for (Node &v : nodes) v.visited = 0;

  Graph *ret = new Graph();
  std::vector<std::vector<Node*>> next(levels);

  for (Node &s : nodes) {
    ret->pushNode();
    next[s.level].push_back(&s);

    for (int l = s.level; l < levels; l++) {
      for (Node *v : next[l]) {
        v->visited = 0;
        if (v != &s)
          ret->pushEdge(v - &nodes[0]);
        for (int i = 0; i < v->out; i++) {
          Node &u = nodes[edges[v->offset + i]];
          if (!u.visited) {
            next[u.level].push_back(&u);
            u.visited = 1;
          }
        }
      }
      next[l].clear();
    }
  }
  return ret;
}

void Graph::pushNode() {
  Node v {};
  v.offset = edges.size();
  nodes.push_back(v);
}

void Graph::pushEdge(int dest) {
  edges.push_back(dest);
  nodes[nodes.size()-1].out++;
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
