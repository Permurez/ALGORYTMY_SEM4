#pragma once
#include <vector>
#include <fstream>
#include <chrono>
#include <iostream>
#include <string>
#include <algorithm>
#include "Board.h"
#include "SI.h"

struct BenchmarkResult {
    int depth;
    int boardSize;
    double timeMs;
    long long nodes;
};

class BenchmarkSystem {//system benchmarkowania zapisujący do CSV
public:
    static void SaveToCSV(const std::string& filename, const std::vector<BenchmarkResult>& results) {
        std::ofstream file(filename);
        file << "Depth,BoardSize,Time_ms,Nodes\n";
        for (const auto& r : results) {
            file << r.depth << "," << r.boardSize << "," << r.timeMs << "," << r.nodes << "\n";
        }
        std::cout << "--- Benchmark zapisany do: " << filename << " ---" << std::endl;
    }

    static void RunFull(const std::string& filename,
                        const std::vector<int>& sizes = {3, 5, 7, 9},
                        int maxDepth = 6,
                        int repeats = 3)
    {
        std::vector<BenchmarkResult> results;

        for (int size : sizes) {
            int winLen = std::min(size, 5);

            for (int depth = 1; depth <= maxDepth; ++depth) {
                double totalMs = 0.0;
                long long nodes = 0;

                for (int rep = 0; rep < repeats; ++rep) {
                    Board board(size, winLen);
                    int c = size / 2;
                    board.place(c,   c,   Cell::X);
                    board.place(c-1, c-1, Cell::O);
                    board.place(c+1, c,   Cell::X);
                    board.place(c,   c+1, Cell::O);
                    board.place(c-1, c+1, Cell::X);//wstepne ruchy aby nie zaczynac od pustej planszy

                    AI ai(Cell::O, {depth});

                    auto start = std::chrono::high_resolution_clock::now();
                    long long n = ai.benchmarkNodes(board, depth);// bezposrednie wywolwanie minmaxa
                    auto end = std::chrono::high_resolution_clock::now();

                    totalMs += std::chrono::duration<double, std::milli>(end - start).count();
                    nodes = n;
                }

                double avgMs = totalMs / repeats;
                results.push_back({depth, size, avgMs, nodes});//zapis

                std::cout << "Size " << size << " Depth " << depth
                          << ": " << avgMs << " ms, " << nodes << " nodes\n";
            }
        }

        SaveToCSV(filename, results);
    }
};