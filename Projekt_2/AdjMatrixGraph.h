#pragma once
#include "IGraph.h"

class AdjMatrixGraph : public IGraph {
    int vertexCount;
    std::vector<std::vector<int>> adjacencyMatrix; // mat[u][v] = waga, 0 = brak krawedzi
public:
    explicit AdjMatrixGraph(int vertexCount);
    int V() const override;//zwraca liczbe wiercholkow
    void addEdge(int vertexA, int vertexB, int weight) override;//dodaje krawedz (u,v) o wadze w
    std::vector<std::pair<int,int>> neighbors(int vertex) const override;// zwraca wektor par dla podanego wiercholka
};
