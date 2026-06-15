#pragma once
#include "Board.h"
#include "SI.h"
#include <memory>
#include <thread>
#include <atomic>

enum class GameMode { HumanVsHuman, HumanVsAI };

struct GameConfig {
    int      boardSize = 3;
    int      winLen    = 3;
    GameMode mode      = GameMode::HumanVsAI;
    AIConfig aiCfg     = {};
};

class Game {
public:
    explicit Game(GameConfig cfg);
    ~Game();

    void handleMove(int row, int col);
    void triggerAIMove();
    void reset();

    const Board& board()        const { return m_board; }
    GameState    state()        const { return m_state; }
    Cell         currentTurn()  const { return m_turn; }
    bool         isAITurn()     const;
    bool         isAIThinking() const { return m_aiThinking; }
    GameConfig   config()       const { return m_cfg; }
    int          scoreX()       const { return m_scoreX; }
    int          scoreO()       const { return m_scoreO; }

private:
    GameConfig           m_cfg;
    Board                m_board;
    GameState            m_state;
    Cell                 m_turn;
    int                  m_scoreX = 0;
    int                  m_scoreO = 0;
    std::unique_ptr<AI>  m_ai;

    std::thread          m_aiThread;
    std::atomic<bool>    m_aiThinking{false};

    void afterMove(int row, int col);
    void switchTurn();
};