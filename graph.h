#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <queue>

struct Node {
  int offset; // offset into the edge array
  int in; // number of incoming edges
  int out; // number of outgoing edges
  int level; // topological level
  unsigned long long visited;
  // edge indices [offset, offset + out) belong to this node
};

class Graph {
public:
  void setLevel(Node&);
  void setTopologicalLevels();
  std::vector<Node> nodes;
  std::vector<int> edges;
  int levels; // total number of topological levels
  void readGraph(std::istream&);
  void writeGraph(std::ostream&);

  template <class G>
  G* depthFirstSearch() {
    G *ret = new G();
    std::stack<Node*> stack;
    std::vector<Node*> visited;

    for (Node &s : nodes) {
      ret->pushNode();
      stack.push(&s);

      while (stack.size()) {
        Node *v = stack.top();
        stack.pop();
        if (v != &s)
          ret->pushEdge(v - &nodes[0]);
        for (int i = 0; i < v->out; i++) {
          Node &u = nodes[edges[v->offset + i]];
          if (!u.visited) {
            stack.push(&u);
            visited.push_back(&u);
            u.visited = 1;
          }
        }
      }
      for (Node *v : visited) v->visited = 0;
      visited.clear();
    }
    return ret;
  }

  template <class G>
  G* breadthFirstSearch() {
    G *ret = new G();
    std::queue<Node*> queue;
    std::vector<Node*> visited;

    for (Node &s : nodes) {
      ret->pushNode();
      queue.push(&s);

      while (queue.size()) {
        Node *v = queue.front();
        queue.pop();
        if (v != &s)
          ret->pushEdge(v - &nodes[0]);
        for (int i = 0; i < v->out; i++) {
          Node &u = nodes[edges[v->offset + i]];
          if (!u.visited) {
            queue.push(&u);
            visited.push_back(&u);
            u.visited = 1;
          }
        }
      }
      for (Node *v : visited) v->visited = 0;
      visited.clear();
    }
    return ret;
  }

  template <class G>
  G* topologicalLevelSearch() {
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

    G *ret = new G();
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

  std::vector<int> bitParallelTopologicalLevelSearch() {
    setTopologicalLevels();
    for (Node &v : nodes) v.visited = 0;

    std::vector<int> ret(nodes.size());
    std::vector<std::vector<Node*>> next(levels);

    std::vector<std::vector<Node*>> levelBuckets(levels);
    for (Node &v : nodes)
      levelBuckets[v.level].push_back(&v);
    for (auto &levelNodes : levelBuckets) {
      for (int segment = 0; segment < levelNodes.size(); segment += 64) {
        int level = levelNodes[segment]->level;
        for (int i = 0; i < 64 && segment + i < levelNodes.size(); i++) {
          Node *v = levelNodes[segment+i];
          v->visited = 1ull << i;
          next[level].push_back(v);
        }
        for (int l = level; l < levels; l++) {
          for (Node *v : next[l]) {
            for (int i = 0; i < v->out; i++) {
              Node &u = nodes[edges[v->offset + i]];
              if (!u.visited)
                next[u.level].push_back(&u);
              u.visited |= v->visited;
            }
            if (l > level)
              for (int i = 0; i < 64; i++)
                if ((1ull << i) & v->visited)
                  ret[levelNodes[segment+i] - &nodes[0]]++;
            v->visited = 0;
          }
          next[l].clear();
        }
      }
    }
    return ret;
  }

  void pushNode() {
    Node v {};
    v.offset = edges.size();
    nodes.push_back(v);
  }

  void pushEdge(int dest) {
    edges.push_back(dest);
    nodes[nodes.size()-1].out++;
  }
};

// graph stub that just counts added edges instead of storing them
class CountingGraph {
private:
  std::vector<int> nodes;
public:
  void pushNode() {
    nodes.push_back(0);
  }

  void pushEdge(int dest) {
    nodes[nodes.size()-1]++;
  }

  void writeGraph(std::ostream&);
};
