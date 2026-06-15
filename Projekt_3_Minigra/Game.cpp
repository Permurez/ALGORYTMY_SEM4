#include "Game.h"

Game::Game(GameConfig cfg)
    : m_cfg(cfg)
    , m_board(cfg.boardSize, cfg.winLen)
    , m_state(GameState::Playing)
    , m_turn(Cell::X)
{
    if (cfg.mode == GameMode::HumanVsAI)
        m_ai = std::make_unique<AI>(Cell::O, cfg.aiCfg);
}

Game::~Game() {
    if (m_aiThread.joinable())
        m_aiThread.join();
}

bool Game::isAITurn() const {
    return m_cfg.mode == GameMode::HumanVsAI && m_turn == Cell::O;
}

void Game::handleMove(int row, int col) {
    if (m_state != GameState::Playing) return;
    if (isAITurn()) return;
    if (m_aiThinking) return;

    if (m_board.place(row, col, m_turn))
        afterMove(row, col);
}

void Game::triggerAIMove() {
    if (m_state != GameState::Playing || !isAITurn() || !m_ai) return;
    if (m_aiThinking) return;

    m_aiThinking = true;

    if (m_aiThread.joinable())
        m_aiThread.join();

    Board boardCopy = m_board; // kopia planszy dla AI

    m_aiThread = std::thread([this, boardCopy]() mutable {
        auto [r, c] = m_ai->bestMove(boardCopy); // AI gra na kopii
        if (r >= 0 && m_board.place(r, c, m_turn)) // tylko wynik na głównej planszy
            afterMove(r, c);
        m_aiThinking = false;
    });
}

void Game::afterMove(int /*row*/, int /*col*/) {
    m_state = m_board.checkState();
    if (m_state == GameState::WinX) ++m_scoreX;
    if (m_state == GameState::WinO) ++m_scoreO;
    if (m_state == GameState::Playing)
        switchTurn();
}

void Game::switchTurn() {
    m_turn = (m_turn == Cell::X) ? Cell::O : Cell::X;
}

void Game::reset() {
    if (m_aiThread.joinable())
        m_aiThread.join();
    m_board = Board(m_cfg.boardSize, m_cfg.winLen);
    m_state = GameState::Playing;
    m_turn  = Cell::X;
    m_aiThinking = false;
}