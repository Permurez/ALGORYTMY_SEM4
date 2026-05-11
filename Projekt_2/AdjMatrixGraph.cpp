#include "AdjMatrixGraph.h"

AdjMatrixGraph::AdjMatrixGraph(int vertexCount) : vertexCount(vertexCount), adjacencyMatrix(vertexCount, std::vector<int>(vertexCount, 0)) {}//macierz n^2 bez krawedzi

int AdjMatrixGraph::V() const { return vertexCount; }//liczba wierzcholkow 

void AdjMatrixGraph::addEdge(int vertexA, int vertexB, int weight) {
    adjacencyMatrix[vertexA][vertexB] = weight;
    adjacencyMatrix[vertexB][vertexA] = weight; 
}//krawedz w obie strony o wadze w 

std::vector<std::pair<int,int>> AdjMatrixGraph::neighbors(int vertex) const {//wektor par z uvw dla wiecholka u
    std::vector<std::pair<int,int>> neighborList;
    for (int i = 0; i < vertexCount; ++i)
        if (adjacencyMatrix[vertex][i] != 0)
            neighborList.emplace_back(i, adjacencyMatrix[vertex][i]);
    return neighborList;
}//u - wierzcholek, i - sasiad u, mat[u][i] - waga krawedzi (u,i)
