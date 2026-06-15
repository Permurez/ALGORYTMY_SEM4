#include "SI.h"
#include <limits>
#include <algorithm>
#include <chrono>

static constexpr int INF = std::numeric_limits<int>::max() / 2;
using Clock = std::chrono::steady_clock;

long long AI::benchmarkNodes(Board& board, int depth) {
    m_nodes = 0;
    auto deadline = Clock::now() + std::chrono::seconds(60);
    minimax(board, depth, -INF, INF, true, deadline);
    return m_nodes;
}

AI::AI(Cell side, AIConfig cfg)
    : m_computerSide(side)
    , m_humanSide(side == Cell::X ? Cell::O : Cell::X)
    , m_cfg(cfg)
{}

int AI::minimax(Board& board, int depth, int alpha, int beta, bool maximizing,
                const Clock::time_point& deadline) {
    ++m_nodes;

    if (Clock::now() >= deadline) return 0;

    GameState state = board.checkState();
    if (state == GameState::WinX)
        return (m_computerSide == Cell::X) ? (INF - board.moveCount())
                                           : -(INF - board.moveCount());
    if (state == GameState::WinO)
        return (m_computerSide == Cell::O) ? (INF - board.moveCount())
                                           : -(INF - board.moveCount());
    if (state == GameState::Draw)
        return 0;

    if (depth == 0)
        return board.heuristicScore(m_computerSide);

    auto moves = board.candidateMoves();

    if (maximizing) {
        int best = -INF;
        for (auto [r, c] : moves) {
            if (Clock::now() >= deadline) break;
            board.place(r, c, m_computerSide);
            int score = minimax(board, depth - 1, alpha, beta, false, deadline);
            board.undo(r, c);
            best  = std::max(best, score);
            alpha = std::max(alpha, best);
            if (alpha >= beta) break;
        }
        return best;
    } else {
        int best = INF;
        for (auto [r, c] : moves) {
            if (Clock::now() >= deadline) break;
            board.place(r, c, m_humanSide);
            int score = minimax(board, depth - 1, alpha, beta, true, deadline);
            board.undo(r, c);
            best = std::min(best, score);
            beta = std::min(beta, best);
            if (alpha >= beta) break;
        }
        return best;
    }
}

std::pair<int,int> AI::bestMove(Board& board) {
    m_nodes = 0;
    auto deadline = Clock::now() + std::chrono::milliseconds(3000);

    auto moves = board.candidateMoves();
    if (moves.empty())     return {-1, -1};
    if (moves.size() == 1) return moves[0];

    // 1. Sprawdź czy AI może wygrać w tym ruchu
    for (auto [r, c] : moves) {
        board.place(r, c, m_computerSide);
        GameState s = board.checkState();
        board.undo(r, c);
        if ((s == GameState::WinX && m_computerSide == Cell::X) ||
            (s == GameState::WinO && m_computerSide == Cell::O))
            return {r, c};
    }

    // 2. Sprawdź czy gracz wygra w następnym ruchu — zablokuj natychmiast
    for (auto [r, c] : moves) {
        board.place(r, c, m_humanSide);
        GameState s = board.checkState();
        board.undo(r, c);
        if ((s == GameState::WinX && m_humanSide == Cell::X) ||
            (s == GameState::WinO && m_humanSide == Cell::O))
            return {r, c};
    }

    // 3. Blokuj gracza jeśli ma winLen-1 w linii z otwartym końcem
    int bestBlockScore = -INF;
    std::pair<int,int> bestBlock = {-1, -1};
    for (auto [r, c] : moves) {
        board.place(r, c, m_humanSide);
        int threat = -board.heuristicScore(m_computerSide);
        board.undo(r, c);
        if (threat > bestBlockScore) {
            bestBlockScore = threat;
            bestBlock = {r, c};
        }
    }
    if (bestBlockScore >= 1000) return bestBlock;

    // 4. Normalny minimax
    int bestScore = -INF;
    std::pair<int,int> bestMv = moves[0];

    for (auto [r, c] : moves) {
        if (Clock::now() >= deadline) break;
        board.place(r, c, m_computerSide);
        int score = minimax(board, m_cfg.maxDepth - 1, -INF, INF, false, deadline);
        board.undo(r, c);

        if (score > bestScore) {
            bestScore = score;
            bestMv    = {r, c};
        }
    }

    return bestMv;
}