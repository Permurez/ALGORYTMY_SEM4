#pragma once
#include "Board.h"
#include <utility>
#include <chrono>
// Konfiguracja AI — maksymalna glebokosc przeszukiwania drzewa gry
struct AIConfig {
    int maxDepth = 6;
};

class AI {
public:
    explicit AI(Cell side, AIConfig cfg = {});
   // Zwraca najlepszy ruch (wiersz, kolumna) dla biezacego stanu planszy
    std::pair<int,int> bestMove(Board& board);
     // Liczba wezlow drzewa minimax odwiedzonych przy ostatnim wywolaniu bestMove
    long long nodesVisited() const { return m_nodes; }
    // tylko do benchmarku - wywoluje minimax bezposrednio
    long long benchmarkNodes(Board& board, int depth);

private:
    using Clock = std::chrono::steady_clock;

    Cell      m_computerSide;
    Cell      m_humanSide;
    AIConfig  m_cfg;
    long long m_nodes = 0;// licznik odwiedzonych wezlow

    // Minimax z alpha-beta pruning:
    //   depth      - pozostala glebokosc przeszukiwania
    //   alpha      - najlepszy wynik dla maximizera (AI)
    //   beta       - najlepszy wynik dla minimizera (gracz)
    //   maximizing - true gdy tura AI, false gdy tura gracza
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizing,
                const Clock::time_point& deadline);
};