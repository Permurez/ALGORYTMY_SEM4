#pragma once
#include "Board.h"
#include <utility>

// Konfiguracja AI — maksymalna glebokosc przeszukiwania drzewa gry
struct AIConfig {
    int maxDepth = 6;
};

class AI {
public:
    explicit AI(Cell side, AIConfig cfg = {});

    // Zwraca najlepszy ruch (wiersz, kolumna) dla biezacego stanu planszy
    std::pair<int,int> bestMove(Board& board);

private:
    Cell     m_computerSide; // strona AI
    Cell     m_humanSide;    // strona gracza
    AIConfig m_cfg;

    // Minimax z alpha-beta pruning:
    //   depth      - pozostala glebokosc przeszukiwania
    //   alpha      - najlepszy wynik dla maximizera (AI)
    //   beta       - najlepszy wynik dla minimizera (gracz)
    //   maximizing - true gdy tura AI, false gdy tura gracza
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizing);//zwraca ocenę planszy z perspektywy AI (dodatnia to dobra dla AI, ujemna to dobra dla gracza)
};
