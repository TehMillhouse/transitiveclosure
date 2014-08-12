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
  /*
  int threads = atoi(argv[1]);
  
  AdjacencyArrayGraph * g = new AdjacencyArrayGraph(0);
  std::fstream f;
  f.open("graphs/2.gra");
  g->readGraph(f);
  f.close();

  std::cout << "old Graph:" << std::endl << std::endl;
  g->writeGraph(std::cout);

    std::cout  << std::endl << std::endl  << std::endl << std::endl;


  AdjacencyMatrixGraph * gOut = g->parallelBFS2<AdjacencyMatrixGraph>(threads);

  gOut->writeGraph(std::cout);

  return 42;
  */

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
