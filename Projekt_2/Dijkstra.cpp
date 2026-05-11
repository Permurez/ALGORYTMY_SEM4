#include "Dijkstra.h"
#include <queue>
#include <climits>

std::vector<int> dijkstra(const IGraph& graph, int sourceVertex) {//oblicza odleglosci najkrotsze od wierzcholka src do wszystkich pozostalych
    int vertexCount = graph.V();
    std::vector<int> shortestDistance(vertexCount, INT_MAX);
    shortestDistance[sourceVertex] = 0;

    // Kopiec min: (dystans, wierzcholek)
    using VertexDistance = std::pair<int,int>;
    std::priority_queue<VertexDistance, std::vector<VertexDistance>, std::greater<VertexDistance>> minHeap;
    minHeap.push({0, sourceVertex});

    while (!minHeap.empty()) {//
        auto [currentDistance, currentVertex] = minHeap.top(); minHeap.pop();//d - aktualna odleglosc do wierzcholka u

        if (currentDistance > shortestDistance[currentVertex]) continue;//jesli aktualna odleglosc jest wieksza niz znana odleglosc do u, to pomijamy ten wpis

        for (auto [neighborVertex, edgeWeight] : graph.neighbors(currentVertex)) {//tam gdzie v jest sasiadem u, a w jest waga krawedzi 
            if (shortestDistance[currentVertex] + edgeWeight < shortestDistance[neighborVertex]) {//
                shortestDistance[neighborVertex] = shortestDistance[currentVertex] + edgeWeight;
                minHeap.push({shortestDistance[neighborVertex], neighborVertex});
            }
        }
    }
    return shortestDistance;
}
