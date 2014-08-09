#include <fstream>
#include <string>
#include <ctime>
#include "graph.h"

template <class G>
int exec(std::string algo, bool output) {
  AdjacencyArrayGraph *g = new AdjacencyArrayGraph(0);
  g->readGraph(std::cin);

  G *gOut;
  clock_t start = clock();
  if (algo == "BFS") {
    gOut = g->breadthFirstSearch<G>();
  } else if (algo == "DFS") {
    gOut = g->depthFirstSearch<G>();
  } else if (algo == "TLS") {
    gOut = g->topologicalLevelSearch<G>();
  } else if (algo == "TLS64") {
    gOut = g->bitParallelTopologicalLevelSearch<G>();
  } else if (algo == "WAR") {
		gOut = g->warshallALgorithm<G>();
  } else if (algo == "RecMerge") {
    gOut = g->recursiveMerge<G>();
  } else if (algo == "RTLS") {
    gOut = g->reverseTopologicalLevelSearch<G>();
	} else {
    std::cout << "Unknown algorithm" << std::endl;
    return 43;
  }
  // output format feasible for gnuplot:
  // <#nodes>  <time / #nodes>
  std::cerr << g->nodes.size() << "  ";
  std::cerr << double(clock() - start) / CLOCKS_PER_SEC << std::endl;

  if (output)
    gOut->writeGraph(std::cout);
  return 0;
}

int main(int argc, char **argv) {
  //hardcoded testing code for parallel algorithms.
  /*
  int size;
  int cur;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &cur);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(size < 2) {
    MPI_Finalize();
    std::cout << "Please select more than 1 Thread!" << std::endl;
    return 42;
  }

  AdjacencyArrayGraph *g = new AdjacencyArrayGraph(0);
  
  //each thread -> read the graph.
  std::fstream f;
  f.open("graphs/3.gra");
  g->readGraph(f);
  f.close();

  //each thread generate one output-graph.   
  AdjacencyMatrixGraph * gOut;

  gOut = g->parallelBFS<AdjacencyMatrixGraph>(cur, size);
  
  if(cur == 0) {
    gOut->writeGraph(std::cout);
  }

  delete gOut;  

  MPI_Finalize();

  return 0;
  */

  //old code.

  if (argc < 3 || argc > 4) {
    std::cout << "Usage: closure <algorithm> <output format> [-no-output]" << std::endl;
    return 42;
  }

  std::string format = argv[2];
  bool output = argc == 3;
  if (format == "count")
    return exec<CountingGraph>(argv[1], output);
  else if (format == "array")
    return exec<AdjacencyArrayGraph>(argv[1], output);
  else if (format == "matrix")
    return exec<AdjacencyMatrixGraph>(argv[1], output);
  else if (format == "list")
    return exec<AdjacencyListGraph>(argv[1], output);
}
