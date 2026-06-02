#include "Game.h"

// ─── Konstruktor ──────────────────────────────────────────────────────────────
Game::Game(GameConfig cfg)
    : m_cfg(cfg)
    , m_board(cfg.boardSize, cfg.winLen)
    , m_state(GameState::Playing)
    , m_turn(Cell::X)
{
    // Tworzymy obiekt AI tylko w trybie Gracz vs AI
    if (cfg.mode == GameMode::HumanVsAI)
        m_ai = std::make_unique<AI>(Cell::O, cfg.aiCfg);
}

// ─── Sprawdzenie tury ─────────────────────────────────────────────────────────
bool Game::isAITurn() const {
    return m_cfg.mode == GameMode::HumanVsAI && m_turn == Cell::O;
}

// ─── Ruch gracza ─────────────────────────────────────────────────────────────
void Game::handleMove(int row, int col) {
    if (m_state != GameState::Playing) return;
    if (isAITurn()) return; // blokujemy ruch gracza gdy tura AI

    if (m_board.place(row, col, m_turn))
        afterMove(row, col);
}

// ─── Ruch AI ─────────────────────────────────────────────────────────────────
void Game::triggerAIMove() {
    if (m_state != GameState::Playing || !isAITurn() || !m_ai) return;

    auto [r, c] = m_ai->bestMove(m_board);
    if (r >= 0 && m_board.place(r, c, m_turn))
        afterMove(r, c);
}

// ─── Po kazdym ruchu ─────────────────────────────────────────────────────────
void Game::afterMove(int /*row*/, int /*col*/) {
    m_state = m_board.checkState();

    // Aktualizacja wynikow
    if (m_state == GameState::WinX) ++m_scoreX;
    if (m_state == GameState::WinO) ++m_scoreO;

    // Zmiana tury tylko gdy gra nadal trwa
    if (m_state == GameState::Playing)
        switchTurn();
}

void Game::switchTurn() {
    m_turn = (m_turn == Cell::X) ? Cell::O : Cell::X;
}

// ─── Reset planszy ────────────────────────────────────────────────────────────
void Game::reset() {
    m_board = Board(m_cfg.boardSize, m_cfg.winLen);
    m_state = GameState::Playing;
    m_turn  = Cell::X;
}
