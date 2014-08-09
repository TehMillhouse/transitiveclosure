#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <cstdlib>
#include <math.h>

#include <mpi.h>




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

	bool hasEdge(int from, int to);

  AdjacencyArrayGraph(int n) {}

  template <class G>
  G* parallelDFS(int cur, int size) {
    //initialize MPI.
    G * result = NULL; 

    if(cur == 0) {
      //thread that cllects all found edges from the other threads.
      result = new G(nodes.size());
      
      int finished = 0;

      //When there are running threads, listen for new edges or finishing message.
      while(finished < (size - 1)) {
        int edge[2];
        MPI_Recv(&edge, 2, MPI_INTEGER, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        //check if message is new edge of finishing-message.
        if(edge[0] != -1) {
          result->addEdge(edge[0], edge[1]);
        } else {
          finished++;
        }
      }

    } else {
      double n = double(nodes.size());
      //mapping of the nodes to the current thread.
      int lowerBound = (cur - 1) * (n / (size - 1.0));
      int upperBound = cur * (n / (size - 1.0));

      std::stack<Node*> stack;
      std::vector<Node*> visited;

      for(int i = lowerBound; i < upperBound; i++) {
        stack.push(&nodes[i]);

        while(stack.size()) {
          Node * v = stack.top();
          stack.pop();
          
          int j = v - &nodes[0];
          //new edge found! send to thread 0! 
          int edge[2];
          edge[0] = i;
          edge[1] = j;
          MPI_Ssend(&edge[0], 2, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);
          // 
          
          for(int j = 0; j < v->out; j++) {
            Node &u = nodes[ edges[ v->offset + j ]];

            if(!u.visited) {
              stack.push(&u);
              visited.push_back(&u);
              u.visited = 1;
            }
          }
        }
        
        //reset 
        for(Node *v : visited) {
          v->visited = 0;
        }

        visited.clear();
      }
      
      //send signal that thread is finished.
      int finished[2];
      finished[0] = -1;
      finished[1] = -1;
      MPI_Send(&finished, 2, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);
    }

    return result;



  }


  template <class G>
  G* parallelBFS(int cur, int size) {
    //initialize MPI.
    G * result = NULL; 

    if(cur == 0) {
      //thread that cllects all found edges from the other threads.
      result = new G(nodes.size());
      
      int finished = 0;

      //When there are running threads, listen for new edges or finishing message.
      while(finished < (size - 1)) {
        int edge[2];
        MPI_Recv(&edge, 2, MPI_INTEGER, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        //check if message is new edge of finishing-message.
        if(edge[0] != -1) {
          result->addEdge(edge[0], edge[1]);
        } else {
          finished++;
        }
      }

    } else {
      double n = double(nodes.size());
      //mapping of the nodes to the current thread.
      int lowerBound = (cur - 1) * (n / (size - 1.0));
      int upperBound = cur * (n / (size - 1.0));

      std::queue<Node*> queue;
      std::vector<Node*> visited;

      for(int i = lowerBound; i < upperBound; i++) {
        queue.push(&nodes[i]);

        while(queue.size()) {
          Node * v = queue.front();
          queue.pop();
          
          int j = v - &nodes[0];
          //new edge found! send to thread 0! 
          int edge[2];
          edge[0] = i;
          edge[1] = j;
          MPI_Ssend(&edge[0], 2, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);
          // 
          
          for(int j = 0; j < v->out; j++) {
            Node &u = nodes[ edges[ v->offset + j ]];

            if(!u.visited) {
              queue.push(&u);
              visited.push_back(&u);
              u.visited = 1;
            }
          }
        }
        
        //reset 
        for(Node *v : visited) {
          v->visited = 0;
        }

        visited.clear();
      }
      
      //send signal that thread is finished.
      int finished[2];
      finished[0] = -1;
      finished[1] = -1;
      MPI_Send(&finished, 2, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);
    }

    return result;
  }


  template <class G>
  G* warshallALgorithm() {
		G * result = new G(nodes.size());
		for(int i = 0; i < nodes.size(); i++) {
			Node & v = nodes[i];
			result->addEdge(i, i);
			for(int l = v.offset; l < v.offset + v.out; l++) {
				result->addEdge(i, edges[l]);
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
    std::queue<Node*> queue;
    std::vector<Node*> visited;

    for (Node &s : nodes) {
      ret->pushNode();
      queue.push(&s);

      while (queue.size()) {
        Node *v = queue.front();
        queue.pop();
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

    G *ret = new G(nodes.size());
    std::vector<std::vector<Node*>> next(levels);

    for (Node &s : nodes) {
      ret->pushNode();
      next[s.level].push_back(&s);

      for (int l = s.level; l < levels; l++) {
        for (Node *v : next[l]) {
          v->visited = 0;
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

  template <class G> G* reverseTopologicalLevelSearch() {
    abort();
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
    std::vector<std::vector<Node*>> next(levels);

    std::vector<std::vector<Node*>> levelBuckets(levels);
    for (Node &v : nodes)
      levelBuckets[v.level].push_back(&v);
    for (auto &levelNodes : levelBuckets) {
      for (int segment = 0; segment < levelNodes.size(); segment += segment_size) {
        int level = levelNodes[segment]->level;
        for (int i = 0; i < segment_size && segment + i < levelNodes.size(); i++) {
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
            for (int i = 0; i < segment_size; i++)
              if ((1ull << i) & v->visited)
                ret->addEdge(levelNodes[segment+i] - &nodes[0], v - &nodes[0]);
            v->visited = 0;
          }
          next[l].clear();
        }
      }
    }
    return ret;
  }

  std::vector<int> reached;

  void recursiveMergeAux(AdjacencyListGraph &g, int v) {
    nodes[v].visited = true;
    for (int u : successors(v))
      if (!nodes[u].visited)
        recursiveMergeAux(g, u);

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

  void recursiveMergeAux(AdjacencyMatrixGraph &g, int v) {
    nodes[v].visited = true;
    for (int i = 0; i < nodes[v].out; i++) {
      int u = edges[nodes[v].offset + i];
      if (!nodes[u].visited)
        recursiveMergeAux(g, u);
      // adj[v] |= adj[u];
      for (int k = 0; k < g.adj[0].size(); k++)
        g.adj[v][k] |= g.adj[u][k];
    }
    g.addEdge(v, v);
  }

  template<class G> G* recursiveMerge() {
    G *ret = new G(nodes.size());
    for (int v = 0; v < nodes.size(); v++)
      if (!nodes[v].visited)
        recursiveMergeAux(*ret, v);
    return ret;
  }
};

template<> inline AdjacencyArrayGraph* AdjacencyArrayGraph::bitParallelTopologicalLevelSearch<AdjacencyArrayGraph>() {
  // no way to efficiently implement this
  abort();
}

template<> inline AdjacencyMatrixGraph* AdjacencyArrayGraph::reverseTopologicalLevelSearch<AdjacencyMatrixGraph>() {
  /*
     reverse TLS
  // compute reachable node as union of
  // reachable nodes of immediate neighbors
  Find topological levels
  for each v in V do reached[v] := {} // stacks
  For l:= L downto 1 do
  For each v in level(l) do
  reset reachedFlags // see b2
  reached[v].push(v)
  For each (v,u) in E do
  For each w in reached[u] do
  if not reachedFlag[w] then
  reachedFlag[w] := 1
  reached[v].push(w)
  */

  setTopologicalLevels();
  std::vector<std::vector<Node*>> levelBuckets(levels);
  std::priority_queue<int> mergeQueue;
  for (Node &v : nodes) {
    levelBuckets[v.level].push_back(&v);
  }

  AdjacencyMatrixGraph *ret = new AdjacencyMatrixGraph(nodes.size());
  for (int l = levels; l > 0; l--) {
    for (Node *n : levelBuckets[l-1]) {
      mergeQueue = std::priority_queue<int>();
      mergeQueue.push(n - &nodes[0]);  // reflexive-transitive closure
      for (int i = 0; i < n->out; i++) {
        int k = edges[n->offset + i];
        for (int j = 0; j < nodes.size(); j++) {
          if (ret->hasEdge(k,j)) {
            mergeQueue.push(j);
          }
        }
      }
      for (int i = 0; i < n->out; i++) {
        mergeQueue.push(edges[n->offset + i]);
      }
      while (!mergeQueue.empty()) {
        int recent = mergeQueue.top();
        while (mergeQueue.top() == recent && !mergeQueue.empty()) {
          mergeQueue.pop();
        }
        ret->addEdge(n - &nodes[0], recent);
      }

    }
  }
  return ret;
}


// graph stub that just counts added edges instead of storing them
class CountingGraph {
private:
  std::vector<int> nodes;
  int currentNode = -1; // to emulate adjacency array API
public:
  CountingGraph(int n);
  void addEdge(int from, int to);
  void writeGraph(std::ostream&);

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

template<> inline AdjacencyArrayGraph* AdjacencyArrayGraph::recursiveMerge() {
  abort();
}

template<> inline CountingGraph* AdjacencyArrayGraph::recursiveMerge() {
  abort();
}
