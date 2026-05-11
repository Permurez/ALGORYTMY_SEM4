#pragma once
#include "IGraph.h"

class AdjListGraph : public IGraph {
    int vertexCount;
    std::vector<std::vector<std::pair<int,int>>> adjacencyList; //wektor trzyma pary (sąsiedni wierzchołek, waga krawędzi) 
public:
    explicit AdjListGraph(int vertexCount); //konstruktor, o n wierzchołków, 0 krawedzi
    int V() const override; //zwraca liczbe wiercholkow
    void addEdge(int vertexA, int vertexB, int weight) override;//dodaje krawedz (u,v) o wadze w
    std::vector<std::pair<int,int>> neighbors(int vertex) const override;// zwraca wektor par dla podanego wiercholka
};
