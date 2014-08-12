#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <cstdlib>
#include <algorithm>

#include <omp.h>


struct Node {
  int offset; // offset into the edge array
  int in; // number of incoming edges
  int out; // number of outgoing edges
  int level; // topological level
  bool reached; // RecMerge
  uint64_t visited;
  // edge indices [offset, offset + out) belong to this node
};

class AdjacencyMatrixGraph {
private:
  int currentNode = -1; // to emulate adjacency array API
  const int segment_size = 64;
public:
  std::vector<std::vector<uint64_t>> adj;
  AdjacencyMatrixGraph(int n);
  void writeGraph(std::ostream&);

  void pushNode() {
    currentNode++;
  }

  void pushEdge(int dest) {
    addEdge(currentNode, dest);
  }

  void addEdge(int from, int to) {
    adj[from][to / segment_size] |= 1ul << (to % segment_size);
  }

  bool hasEdge(int from, int to)
  {
    return adj[from][to / segment_size] & (1ul << (to % segment_size));
  }
};

class AdjacencyListGraph {
private:
  int currentNode = -1; // to emulate adjacency array API
public:
  std::vector<std::vector<int>> adj;

  AdjacencyListGraph(int n);
  void writeGraph(std::ostream&);

  void pushNode() {
    currentNode++;
  }

  void pushEdge(int dest) {
    addEdge(currentNode, dest);
  }

  void addEdge(int from, int to) {
    adj[from].push_back(to);
  }

  bool hasEdge(int from, int to) {
    abort();
  }
};

// graph stub that just counts added edges instead of storing them
class CountingGraph {
private:
  std::vector<int> nodes;
  int currentNode = -1; // to emulate adjacency array API
public:
  CountingGraph(int n);
  void writeGraph(std::ostream&);

  void addEdge(int from, int to) {
    nodes[from]++;
  }

	bool hasEdge(int from, int to) {
		abort();
	}

  void pushNode() {
    currentNode++;
  }

  void pushEdge(int dest) {
    nodes[currentNode]++;
  }
};

class AdjacencyArrayGraph {
public:
  struct OutIterator {
    AdjacencyArrayGraph *g;
    int v;

    int* begin() const {
      return &g->edges[g->nodes[v].offset];
    }

    int* end() const {
      return &g->edges[g->nodes[v].offset + g->nodes[v].out];
    }
  };

  void setLevel(Node&);
  void setTopologicalLevels();
  std::vector<Node> nodes;
  std::vector<int> edges;
  int levels; // total number of topological levels
  void readGraph(std::istream&);
  void writeGraph(std::ostream&);

  OutIterator successors(int v) {
    OutIterator it {this, v};
    return it;
  }

	void addEdge(int from, int to) {
		abort();
	}

	bool hasEdge(int from, int to) {
    abort();
  }

  AdjacencyArrayGraph(int n) {}

  template <class G> 
  G * parallelBFS(int threads) {
    int maxThreads = threads;
    if(maxThreads > nodes.size()) {
      maxThreads = nodes.size();
    } else if(maxThreads < 1) {
      maxThreads = 1;
    }

    G * result = new G(nodes.size());
    
    double n = nodes.size();

    omp_set_num_threads(maxThreads);

    //parallel part.
    #pragma omp parallel for 
    for(int s = 0; s < nodes.size(); s++) {
      
      std::queue<int> queue;
      std::vector<int> visited(nodes.size());  
      queue.push(s);

      while(queue.size()) {
        int v = queue.front();
        queue.pop();

        for(int u : successors(v)) {
          result->addEdge(s, v);
          
          if(!visited[u]) {
            queue.push(u);
            visited[u] = 1;
          }
        }
      }

      visited.clear();
    }

    return result;
  }

  template <class G>
  G * parallelBFS2(int threads) {
    int maxThreads = threads;

    //check if number of threads makes sense.
    if(maxThreads < 1) {
      maxThreads = 1;
    } else if(maxThreads > nodes.size()) {
      maxThreads = nodes.size();
    }

    G * result = new G(nodes.size());

    //a visited-vector and a queue for each thread.
    std::vector< std::vector<int> > visitedVec(maxThreads, std::vector<int>(nodes.size(), 0));
    std::vector <std::queue<int> > queues(maxThreads, std::queue<int>());

    omp_set_num_threads(maxThreads);
    #pragma omp parallel
    {
      int cur = omp_get_thread_num();

      double n = (double) nodes.size();
      int lowerBound = cur * (n / maxThreads);
      int upperBound = (cur + 1) * (n / maxThreads);

      //queue and visited vector for the current thread..
      std::queue<int> * curQueue = &queues[cur];
      std::vector<int> * curVisited = &visitedVec[cur];

      //for each node of the graph do a BFS in parallel.
      for(int i = lowerBound; i < upperBound; i++) {
        curQueue->push(i);
        
        while(curQueue->size()) {
          int v = curQueue->front();
          curQueue->pop();

          for(int u : successors(v)) {
            result->addEdge(i, u);

            //prove on current timestamp.
            if( (*curVisited)[u] != i ) {
              curQueue->push(u);
              (*curVisited)[u] = i;
            }
          }
        }
      } 
    }

    return result;
  }


  template <class G>
  G* warshallALgorithm() {
		G * result = new G(nodes.size());
		for(int i = 0; i < nodes.size(); i++) {
			for(int j : successors(i)) {
				result->addEdge(i, j);
			}
		}

		for(int k = 0; k < this->nodes.size(); ++k) {
			for(int i = 0; i < this->nodes.size(); ++i) {
				if(result->hasEdge(i, k)) {
					for(int j = 0; j < this->nodes.size(); ++j) {
						if(result->hasEdge(k, j)) {
							result->addEdge(i, j);
						}
					}
				}
			}
		}

		return result;
	}

  template <class G>
  G* depthFirstSearch() {
    G *ret = new G(nodes.size());
    std::stack<int> stack;
    std::vector<int> visited;

    for (int s = 0; s < nodes.size(); s++) {
      ret->pushNode();
      stack.push(s);

      while (stack.size()) {
        int v = stack.top();
        stack.pop();
        ret->pushEdge(v);
        for (int u : successors(v)) {
          if (!nodes[u].visited) {
            stack.push(u);
            visited.push_back(u);
            nodes[u].visited = 1;
          }
        }
      }
      for (int v : visited) nodes[v].visited = 0;
      visited.clear();
    }
    return ret;
  }

  template <class G>
  G* breadthFirstSearch() {
    G *ret = new G(nodes.size());
    std::queue<int> queue;
    std::vector<int> visited;

    for (int s = 0; s < nodes.size(); s++) {
      ret->pushNode();
      queue.push(s);

      while (queue.size()) {
        int v = queue.front();
        queue.pop();
        ret->pushEdge(v);
        for (int u : successors(v)) {
          if (!nodes[u].visited) {
            queue.push(u);
            visited.push_back(u);
            nodes[u].visited = 1;
          }
        }
      }
      for (int v : visited) nodes[v].visited = 0;
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

    G *ret = new G(nodes.size());
    std::vector<std::vector<int>> next(levels);

    for (int s = 0; s < nodes.size(); s++) {
      ret->pushNode();
      next[nodes[s].level].push_back(s);

      for (int l = nodes[s].level; l < levels; l++) {
        for (int v : next[l]) {
          nodes[v].visited = 0;
          ret->pushEdge(v);
          for (int u : successors(v)) {
            if (!nodes[u].visited) {
              next[nodes[u].level].push_back(u);
              nodes[u].visited = 1;
            }
          }
        }
        next[l].clear();
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

  template<class G> G* bitParallelTopologicalLevelSearch() {
    setTopologicalLevels();
    for (Node &v : nodes) v.visited = 0;
    const int segment_size = sizeof(Node::visited) * 8;

    G *ret = new G(nodes.size());
    std::vector<std::vector<int>> next(levels);

    std::vector<std::vector<int>> levelBuckets(levels);
    for (int v = 0; v < nodes.size(); v++)
      levelBuckets[nodes[v].level].push_back(v);
    for (auto &levelNodes : levelBuckets) {
      for (int segment = 0; segment < levelNodes.size(); segment += segment_size) {
        int level = nodes[levelNodes[segment]].level;
        for (int i = 0; i < segment_size && segment + i < levelNodes.size(); i++) {
          int v = levelNodes[segment+i];
          nodes[v].visited = 1ull << i;
          next[level].push_back(v);
        }
        for (int l = level; l < levels; l++) {
          for (int v : next[l]) {
            for (int u : successors(v)) {
              if (!nodes[u].visited)
                next[nodes[u].level].push_back(u);
              nodes[u].visited |= nodes[v].visited;
            }
            for (int i = 0; i < segment_size; i++)
              if ((1ull << i) & nodes[v].visited)
                ret->addEdge(levelNodes[segment+i], v);
            nodes[v].visited = 0;
          }
          next[l].clear();
        }
      }
    }
    return ret;
  }

  std::vector<int> reached;

  template<class G> void mergeSuccessors(G &g, int v) {
    abort();
  }

  template<class G>
  void recursiveMergeAux(G &g, int v) {
    nodes[v].visited = true;
    for (int u : successors(v))
      if (!nodes[u].visited)
        recursiveMergeAux(g, u);
    mergeSuccessors(g, v);
  }

  template<class G>
  G* recursiveMerge() {
    G *ret = new G(nodes.size());
    for (int v = 0; v < nodes.size(); v++)
      if (!nodes[v].visited)
        recursiveMergeAux(*ret, v);
    return ret;
  }

  template<class G>
  G* reverseTopologicalLevelSearch() {
    setTopologicalLevels();
    std::vector<std::vector<int>> levelBuckets(levels);
    for (Node &v : nodes) {
      levelBuckets[v.level].push_back(&v-&nodes[0]);
    }

    G *ret = new G(nodes.size());
    for (int l = levels-1; l >= 0; l--)
      for (int v : levelBuckets[l])
        mergeSuccessors(*ret, v);
    return ret;
  }
};

template<> inline AdjacencyArrayGraph* AdjacencyArrayGraph::bitParallelTopologicalLevelSearch() {
  // no way to efficiently implement this
  abort();
}

template<> inline
void AdjacencyArrayGraph::mergeSuccessors(AdjacencyListGraph &g, int v) {
  for (int u : successors(v))
    for (int w : g.adj[u])
      if (!nodes[w].reached) {
        nodes[w].reached = true;
        reached.push_back(w);
        g.addEdge(v, w);
      }
  g.addEdge(v, v);

  for (int w : reached) nodes[w].reached = false;
  reached.clear();
}

template<> inline
void AdjacencyArrayGraph::mergeSuccessors(AdjacencyMatrixGraph &g, int v) {
  for (int u : successors(v)) {
    // adj[v] |= adj[u];
    for (int k = 0; k < g.adj[0].size(); k++)
      g.adj[v][k] |= g.adj[u][k];
  }
  g.addEdge(v, v);
}
