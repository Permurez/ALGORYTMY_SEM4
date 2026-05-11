#include "AdjListGraph.h"

AdjListGraph::AdjListGraph(int vertexCount) : vertexCount(vertexCount), adjacencyList(vertexCount) {} //wektor n elemtów, każdy to wektor trzyma pare wiercholka i wagi krawedzi

int AdjListGraph::V() const { return vertexCount; }

void AdjListGraph::addEdge(int vertexA, int vertexB, int weight) {// krawedz mierzy u i v, waga W
    adjacencyList[vertexA].emplace_back(vertexB, weight);
    adjacencyList[vertexB].emplace_back(vertexA, weight); 
}//graf nieskierowany, wiec krawedz jest dodawana w obu kierunkach

std::vector<std::pair<int,int>> AdjListGraph::neighbors(int vertex) const {
    return adjacencyList[vertex];//zwraca wektor par dla podanego wiercholka
}
