#pragma once
#include "IGraph.h"
#include <vector>

// Algorytm Dijkstry – kolejka priorytetowa (kopiec min via std::priority_queue).
// Zwraca wektor odleglosci od src do kazdego wierzcholka.
// INT_MAX oznacza brak polaczenia.
std::vector<int> dijkstra(const IGraph& graph, int sourceVertex);
