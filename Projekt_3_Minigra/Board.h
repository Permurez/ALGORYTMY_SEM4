#pragma once
#include <vector>
#include <optional>
#include <utility>

// Mozliwe stany pojedynczego pola planszy
enum class Cell { Empty, X, O };

// Mozliwe stany gry
enum class GameState { Playing, WinX, WinO, Draw };

// Informacja o zwycieskiej linii (do narysowania)
struct WinInfo {
    int startRow, startCol;
    int endRow,   endCol;
};

class Board {
public:
    Board(int size, int winLen);

    // Ustawia znak gracza na polu; zwraca false jesli pole zajete lub poza plansza
    bool place(int row, int col, Cell player);

    // Cofa ostatni ruch (potrzebne przez algorytm minimax)
    void undo(int row, int col);

    bool isEmpty(int row, int col) const;
    bool isFull()                  const;
    Cell get(int row, int col)     const;

    int size()      const { return m_size; }
    int winLen()    const { return m_winLen; }
    int moveCount() const { return m_moveCount; }

    // Sprawdza aktualny stan gry (kto wygral / remis / gra trwa)
    GameState              checkState() const;
    std::optional<WinInfo> getWinInfo() const;

    // Ocena heurystyczna planszy dla algorytmu AI
    int heuristicScore(Cell maximizer) const;

    // Zwraca liste pol kandydujacych do ruchu (sasiedztwo istniejacych znakow)
    std::vector<std::pair<int,int>> candidateMoves() const;

private:
    int m_size;
    int m_winLen;
    int m_moveCount;
    std::vector<std::vector<Cell>> m_grid;

    // Sprawdza czy ruch na (row,col) daje wygrana dla 'player'
    bool checkWin(int row, int col, Cell player) const;

    // Ocenia pojedynczy ciag znakow (ilosc + otwarte konce)
    int evalLine(int count, int openEnds, Cell player, Cell maximizer) const;
};
