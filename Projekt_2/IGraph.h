#pragma once
#include <vector>
#include <utility>

// Interfejs grafu nieskierowanego wazonego.
// Jedyna operacja wymagana przez Dijkstre to neighbors(u).
class IGraph {
public:
    virtual int V() const = 0;
    virtual void addEdge(int vertexA, int vertexB, int weight) = 0;
    virtual std::vector<std::pair<int,int>> neighbors(int vertex) const = 0;//zwraca wektor par (sasiad, waga) dla sasiadow u
    virtual ~IGraph() = default;
};
