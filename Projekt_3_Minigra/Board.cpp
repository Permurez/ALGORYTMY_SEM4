#include "Board.h"
#include <algorithm>
#include <cmath>

// ─── Konstruktor ─────────────────────────────────────────────────────────────
Board::Board(int size, int winLen)
    : m_size(size)
    , m_winLen(winLen)
    , m_moveCount(0)
    , m_grid(size, std::vector<Cell>(size, Cell::Empty))
{}

// ─── Operacje na planszy ──────────────────────────────────────────────────────
bool Board::place(int row, int col, Cell player) {
    if (row < 0 || row >= m_size || col < 0 || col >= m_size) return false;
    if (m_grid[row][col] != Cell::Empty) return false;
    m_grid[row][col] = player;
    ++m_moveCount;
    return true;
}

void Board::undo(int row, int col) {
    if (m_grid[row][col] != Cell::Empty) {
        m_grid[row][col] = Cell::Empty;
        --m_moveCount;
    }
}

bool Board::isFull()                  const { return m_moveCount >= m_size * m_size; }
bool Board::isEmpty(int row, int col) const { return m_grid[row][col] == Cell::Empty; }
Cell Board::get(int row, int col)     const { return m_grid[row][col]; }

//Sprawdzanie wygranej 
// Sprawdza 4 kierunki od pola (row,col): poziom, pion, ukos \, ukos /
bool Board::checkWin(int row, int col, Cell player) const {
    const int dr[] = {0, 1, 1,  1};
    const int dc[] = {1, 0, 1, -1};

    for (int d = 0; d < 4; ++d) {
        int count = 1;
        // Liczymy w kierunku pozytywnym
        for (int k = 1; k < m_winLen; ++k) {
            int r = row + dr[d]*k, c = col + dc[d]*k;
            if (r < 0 || r >= m_size || c < 0 || c >= m_size) break;
            if (m_grid[r][c] != player) break;
            ++count;
        }
        // Liczymy w kierunku przeciwnym
        for (int k = 1; k < m_winLen; ++k) {
            int r = row - dr[d]*k, c = col - dc[d]*k;
            if (r < 0 || r >= m_size || c < 0 || c >= m_size) break;
            if (m_grid[r][c] != player) break;
            ++count;
        }
        if (count >= m_winLen) return true;
    }
    return false;
}

GameState Board::checkState() const {
    for (int r = 0; r < m_size; ++r)
        for (int c = 0; c < m_size; ++c) {
            Cell cell = m_grid[r][c];
            if (cell == Cell::Empty) continue;
            if (checkWin(r, c, cell))
                return (cell == Cell::X) ? GameState::WinX : GameState::WinO;
        }
    if (isFull()) return GameState::Draw;
    return GameState::Playing;
}

// Zwraca informacje o zwycieskiej linii (start i koniec)
std::optional<WinInfo> Board::getWinInfo() const {
    const int dr[] = {0, 1, 1,  1};
    const int dc[] = {1, 0, 1, -1};

    for (int r = 0; r < m_size; ++r)
        for (int c = 0; c < m_size; ++c) {
            Cell cell = m_grid[r][c];
            if (cell == Cell::Empty) continue;

            for (int d = 0; d < 4; ++d) {
                int count = 1;
                for (int k = 1; k < m_winLen; ++k) {
                    int nr = r + dr[d]*k, nc = c + dc[d]*k;
                    if (nr < 0 || nr >= m_size || nc < 0 || nc >= m_size) break;
                    if (m_grid[nr][nc] != cell) break;
                    ++count;
                }
                if (count >= m_winLen)
                    return WinInfo{r, c,
                                   r + dr[d]*(m_winLen-1),
                                   c + dc[d]*(m_winLen-1)};
            }
        }
    return std::nullopt;
}

// ─── Heurystyka ───────────────────────────────────────────────────────────────
// Ocenia ciag znakow tego samego gracza:
//   count      - dlugosc ciagu
//   openEnds   - ile konców ciagu jest otwartych (0, 1 lub 2)
// Wynik jest dodatni dla maximizera, ujemny dla przeciwnika
int Board::evalLine(int count, int openEnds, Cell player, Cell maximizer) const {
    if (count == 0 || openEnds == 0) return 0;

    int score = 0;
    if      (count >= m_winLen)          score = 100000; // wygrana
    else if (count == m_winLen - 1)      score = (openEnds == 2) ? 10000 : 1000;
    else if (count == m_winLen - 2)      score = (openEnds == 2) ? 100   : 10;
    else                                 score = openEnds;

    return (player == maximizer) ? score : -score;
}

// Skanuje plansze we wszystkich 4 kierunkach, sumuje oceny ciagow
int Board::heuristicScore(Cell maximizer) const {
    int total = 0;
    const int dr[] = {0, 1, 1,  1};
    const int dc[] = {1, 0, 1, -1};

    for (int d = 0; d < 4; ++d) {
        for (int r = 0; r < m_size; ++r) {
            for (int c = 0; c < m_size; ++c) {
                Cell first = m_grid[r][c];
                if (first == Cell::Empty) continue;

                // Pomijamy pola ktore nie sa poczatkiem ciagu w tym kierunku
                int pr = r - dr[d], pc = c - dc[d];
                if (pr >= 0 && pr < m_size && pc >= 0 && pc < m_size
                    && m_grid[pr][pc] == first) continue;

                // Liczymy dlugosc ciagu
                int count = 0;
                int nr = r, nc = c;
                while (nr >= 0 && nr < m_size && nc >= 0 && nc < m_size
                       && m_grid[nr][nc] == first) {
                    ++count;
                    nr += dr[d]; nc += dc[d];
                }

                // Sprawdzamy otwarte konce
                int openEnds = 0;
                int br = r - dr[d], bc = c - dc[d];
                if (br >= 0 && br < m_size && bc >= 0 && bc < m_size
                    && m_grid[br][bc] == Cell::Empty) ++openEnds;
                if (nr >= 0 && nr < m_size && nc >= 0 && nc < m_size
                    && m_grid[nr][nc] == Cell::Empty) ++openEnds;

                total += evalLine(count, openEnds, first, maximizer);
            }
        }
    }

    // Maly bonus za srodkowe pola (lepsze pozycyjnie)
    int center = m_size / 2;
    for (int r = 0; r < m_size; ++r)
        for (int c = 0; c < m_size; ++c) {
            if (m_grid[r][c] == Cell::Empty) continue;
            int dist  = std::abs(r - center) + std::abs(c - center);
            int bonus = std::max(0, m_size - dist);
            total += (m_grid[r][c] == maximizer) ? bonus : -bonus;
        }

    return total;
}

// Generowanie ruchów kandydatów
// Zamiast analizowac cala plansze, bierzemy tylko pola w poblizu istniejacych
// znakow — znacznie zmniejsza to liczbe wezlow w drzewie minimax.
std::vector<std::pair<int,int>> Board::candidateMoves() const {
    // Pierwszy ruch: srodek planszy
    if (m_moveCount == 0)
        return {{m_size / 2, m_size / 2}};

    const int RADIUS = 2;
    
    // Zamiast std::vector<std::vector<bool>> alokujemy płaską tablicę na stosie.
    // Max plansza to 15x15 = 225, dajemy 256 dla bezpieczeństwa.
    // Inicjalizacja {false} zeruje całą tablicę 
    bool seen[256] = {false};
    
    std::vector<std::pair<int,int>> moves;
    // rezerwujemy od razu pamięćna wektor ruchów, 
    moves.reserve(64); 

    for (int r = 0; r < m_size; ++r) {
        for (int c = 0; c < m_size; ++c) {
            if (m_grid[r][c] == Cell::Empty) continue;

            for (int dr = -RADIUS; dr <= RADIUS; ++dr) {
                for (int dc = -RADIUS; dc <= RADIUS; ++dc) {
                    int nr = r + dr, nc = c + dc;
                    if (nr < 0 || nr >= m_size || nc < 0 || nc >= m_size) continue;
                    if (m_grid[nr][nc] != Cell::Empty) continue;
                    
                    // Spłaszczamy indeksowanie 2D do 1D
                    int flatIndex = nr * m_size + nc;
                    if (seen[flatIndex]) continue;
                    
                    seen[flatIndex] = true;
                    moves.push_back({nr, nc});
                }
            }
        }
    }
    return moves;
}