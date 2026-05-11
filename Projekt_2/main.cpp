#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <climits>
#include <numeric>
#include <algorithm>
#include <set>

#include "IGraph.h"
#include "AdjListGraph.h"
#include "AdjMatrixGraph.h"
#include "Dijkstra.h"

// ---------------------------------------------------------------------------
// GENERATOR LOSOWEGO GRAFU
// ---------------------------------------------------------------------------

void generateRandomGraph(IGraph& graph, int V, int densityPercent, std::mt19937& rng) {
    int maxEdges = V * (V - 1) / 2; // maks. krawedzi dla grafu nieskierowanego
    int targetEdges = std::max(V - 1, (int)std::round(maxEdges * densityPercent / 100.0)); // docelowa liczba krawedzi
    if (targetEdges > maxEdges) targetEdges = maxEdges; // ograniczenie do grafu pelnego

    std::uniform_int_distribution<int> weightDist(1, 99); // wagi krawedzi od 1 do 99

    // losowa permutacja wierzcholkow - podstawa drzewa rozpinajacego
    std::vector<int> perm(V);
    std::iota(perm.begin(), perm.end(), 0); // wypelnienie 0..V-1
    std::shuffle(perm.begin(), perm.end(), rng);

    std::set<std::pair<int,int>> added; // zbior dodanych krawedzi (u < v)

    for (int i = 1; i < V; ++i) { // dodaj V-1 krawedzi tworzacych drzewo rozpinajace (gwarantuje spojnosc)
        int u = std::min(perm[i-1], perm[i]);
        int v = std::max(perm[i-1], perm[i]);
        graph.addEdge(u, v, weightDist(rng));
        added.insert({u, v});
    }

    std::uniform_int_distribution<int> vertexDist(0, V - 1);
    int attempts = 0;
    while ((int)added.size() < targetEdges && attempts < targetEdges * 20) { // dolosowuj krawedzie az do osiagniecia gestosci
        ++attempts;
        int u = vertexDist(rng);
        int v = vertexDist(rng);
        if (u == v) continue; // pomijamy petle
        if (u > v) std::swap(u, v); // zawsze u < v dla zbioru
        if (added.count({u, v})) continue; // krawedz juz istnieje
        graph.addEdge(u, v, weightDist(rng));
        added.insert({u, v});
    }
}

// ---------------------------------------------------------------------------
// DEMO - pokazuje dzialanie Dijkstry na malym przykladzie
// ---------------------------------------------------------------------------

void runDemo() {
    std::cout << "========================================\n";
    std::cout << "  DEMO - Dijkstra, graf V=6\n";
    std::cout << "========================================\n";

    AdjListGraph demo(6); // graf testowy z 6 wierzcholkami
    demo.addEdge(0, 1, 4);
    demo.addEdge(0, 2, 2);
    demo.addEdge(1, 3, 5);
    demo.addEdge(2, 3, 1);
    demo.addEdge(2, 4, 7);
    demo.addEdge(3, 5, 3);
    demo.addEdge(4, 5, 3);

    int src = 0;
    auto dist = dijkstra(demo, src); // uruchomienie algorytmu od wierzcholka 0

    std::cout << "Zrodlo: " << src << "\n";
    std::cout << "Wierzcholek | Najkrotsza droga\n";
    std::cout << "------------|------------------\n";
    for (int i = 0; i < (int)dist.size(); ++i) {
        if (dist[i] == INT_MAX)
            std::cout << "     " << i << "      |     INF\n";
        else
            std::cout << "     " << i << "      |     " << dist[i] << "\n"; // wypisanie odleglosci od src do i
    }
    std::cout << "\n";
}

// ---------------------------------------------------------------------------
// BENCHMARK
// ---------------------------------------------------------------------------

int main() {
    runDemo(); // najpierw demonstracja na malym grafie

    const std::vector<int> VERTICES  = {10, 50, 100, 500, 1000}; // 5 rozmiarow zgodnie z trescia
    const std::vector<int> DENSITIES = {25, 50, 75, 100};         // 4 gestosci: 25%, 50%, 75%, pelny
    const int REPS = 100; // liczba losowych instancji na kazdy zestaw parametrow

    std::mt19937 rng(42); // generator z ustalonym seedem dla powtarzalnosci

    std::ofstream csv("results.csv");
    if (!csv) {
        std::cerr << "Blad: nie mozna otworzyc results.csv\n";
        return 1;
    }
    csv << "representation,vertices,density,avg_time_us\n"; // naglowek CSV

    std::cout << "Benchmark uruchomiony...\n";
    std::cout << "repr\t\tV\tdensity\tavg_time_us\n";
    std::cout << "--------------------------------------------\n";

    for (int V : VERTICES) {
        for (int density : DENSITIES) {

            // -- lista sasiedztwa --
            {
                double totalUs = 0.0;
                for (int r = 0; r < REPS; ++r) {
                    AdjListGraph g(V); // nowy graf dla kazdej instancji
                    generateRandomGraph(g, V, density, rng);

                    auto t0 = std::chrono::high_resolution_clock::now();
                    dijkstra(g, 0); // mierzymy sam algorytm, bez generowania
                    auto t1 = std::chrono::high_resolution_clock::now();

                    totalUs += std::chrono::duration<double, std::micro>(t1 - t0).count();
                }
                double avg = totalUs / REPS; // sredni czas z 100 instancji [us]
                csv  << "list,"   << V << "," << density << "," << avg << "\n";
                std::cout << "list\t\t" << V << "\t" << density << "%\t" << avg << "\n";
            }

            // -- macierz sasiedztwa --
            {
                double totalUs = 0.0;
                for (int r = 0; r < REPS; ++r) {
                    AdjMatrixGraph g(V); // nowy graf dla kazdej instancji
                    generateRandomGraph(g, V, density, rng);

                    auto t0 = std::chrono::high_resolution_clock::now();
                    dijkstra(g, 0); // mierzymy sam algorytm, bez generowania
                    auto t1 = std::chrono::high_resolution_clock::now();

                    totalUs += std::chrono::duration<double, std::micro>(t1 - t0).count();
                }
                double avg = totalUs / REPS; // sredni czas z 100 instancji [us]
                csv  << "matrix," << V << "," << density << "," << avg << "\n";
                std::cout << "matrix\t\t" << V << "\t" << density << "%\t" << avg << "\n";
            }
        }
    }

    csv.close();
    std::cout << "\nGotowe. Wyniki zapisane do results.csv\n";
    std::cout << "Uruchom: python3 plot.py\n";
    return 0;
}
