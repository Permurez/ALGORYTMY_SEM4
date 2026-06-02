#include "SI.h"
#include <limits>
#include <algorithm>

static constexpr int INF = std::numeric_limits<int>::max() / 2;

AI::AI(Cell side, AIConfig cfg)
    : m_computerSide(side)
    , m_humanSide(side == Cell::X ? Cell::O : Cell::X)
    , m_cfg(cfg)
{}

// ─── Minimax z Alpha-Beta Pruning ─────────────────────────────────────────────
// Algorytm rekurencyjnie przeszukuje drzewo mozliwych ruchow.
// Alpha-beta pruning pomija galEzie, ktore nie moga poprawic wyniku —
// dzieki temu mozna przeszukac znacznie wieksze drzewa niz czystym minimaxem.
//depth ruchy do przodu, maximizing to tura AI, alpha to najlepszy wynik dla AI, beta to najlepszy wynik dla gracza
int AI::minimax(Board& board, int depth, int alpha, int beta, bool maximizing) {
    
    GameState state = board.checkState();
    if (state == GameState::WinX)
        return (m_computerSide == Cell::X) ? (INF - board.moveCount())
                                           : -(INF - board.moveCount());// Remis gorszy od wygranej, ale lepszy od przegranej
    if (state == GameState::WinO)
        return (m_computerSide == Cell::O) ? (INF - board.moveCount())
                                           : -(INF - board.moveCount());// Remis gorszy od wygranej, ale lepszy od przegranej
    if (state == GameState::Draw)//Remis gorszy od wygranej, ale lepszy od przegranej
        return 0;

    // Osiagnelismy limit glebokosci — uzywamy heurystyki zamiast pelnego przeszukania
    if (depth == 0)
        return board.heuristicScore(m_computerSide);

    auto moves = board.candidateMoves();

    if (maximizing) {
        // Tura AI — szukamy maksymalnego wyniku
        int best = -INF;
        for (auto [r, c] : moves) {
            board.place(r, c, m_computerSide);
            int score = minimax(board, depth - 1, alpha, beta, false);
            board.undo(r, c);

            best  = std::max(best, score);
            alpha = std::max(alpha, best);
            if (alpha >= beta) break; // odciecie beta — gracz nie wybierze tej galezi
        }
        return best;
    } else {
        // Tura gracza — szukamy minimalnego wyniku (gracz gra optymalnie)
        int best = INF;
        for (auto [r, c] : moves) {
            board.place(r, c, m_humanSide);
            int score = minimax(board, depth - 1, alpha, beta, true);
            board.undo(r, c);

            best = std::min(best, score);
            beta = std::min(beta, best);
            if (alpha >= beta) break; // odciecie alfa — AI nie wybierze tej galezi
        }
        return best;
    }
}

std::pair<int,int> AI::bestMove(Board& board) {//zwraca najlepszy ruch (wiersz, kolumna) dla biezacego stanu planszy
    auto moves = board.candidateMoves();
    if (moves.empty())   return {-1, -1};
    if (moves.size() == 1) return moves[0];

    int bestScore          = -INF;
    std::pair<int,int> bestMv = moves[0];

    for (auto [r, c] : moves) {
        board.place(r, c, m_computerSide);
        // Wywolujemy minimax z gleba (maxDepth - 1) bo jeden ruch juz zrobiono
        int score = minimax(board, m_cfg.maxDepth - 1, -INF, INF, false);
        board.undo(r, c);

        if (score > bestScore) {
            bestScore = score;
            bestMv    = {r, c};
        }
    }

    return bestMv;
}
