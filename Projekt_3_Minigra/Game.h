#pragma once
#include "Board.h"
#include "SI.h"
#include <memory>

// Tryby gry
enum class GameMode { HumanVsHuman, HumanVsAI };

// Konfiguracja rozgrywki (wybierana w menu)
struct GameConfig {
    int      boardSize = 3;
    int      winLen    = 3;
    GameMode mode      = GameMode::HumanVsAI;
    AIConfig aiCfg     = {};
};

class Game {
public:
    explicit Game(GameConfig cfg);

    // Obsluga ruchu gracza (klikniecie w pole)
    void handleMove(int row, int col);

    // Wywolywane co klatkE gdy jest tura AI
    void triggerAIMove();

    // Resetuje plansze, zachowuje wyniki
    void reset();

    // Gettery
    const Board& board()       const { return m_board; }
    GameState    state()       const { return m_state; }
    Cell         currentTurn() const { return m_turn; }
    bool         isAITurn()    const;
    GameConfig   config()      const { return m_cfg; }
    int          scoreX()      const { return m_scoreX; }
    int          scoreO()      const { return m_scoreO; }

private:
    GameConfig           m_cfg;
    Board                m_board;
    GameState            m_state;
    Cell                 m_turn;
    int                  m_scoreX = 0;
    int                  m_scoreO = 0;
    std::unique_ptr<AI>  m_ai;

    void afterMove(int row, int col);
    void switchTurn();
};
