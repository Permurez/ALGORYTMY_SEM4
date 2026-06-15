#include "Board.h"
#include <algorithm>
#include <cmath>
Board::Board(int size, int winLen)//rozmiar planszy, rzad do wygranej,inicjalizacja planszy i liczników
    : m_size(size)
    , m_winLen(winLen)
    , m_moveCount(0)
    , m_grid(size, std::vector<Cell>(size, Cell::Empty))
{}

bool Board::place(int row, int col, Cell player) {//ustawia znak gracza na polu; zwraca false jesli pole zajete lub poza plansza
    if (row < 0 || row >= m_size || col < 0 || col >= m_size) return false;
    if (m_grid[row][col] != Cell::Empty) return false;
    m_grid[row][col] = player;
    ++m_moveCount;
    return true;
}

void Board::undo(int row, int col) {//potrzbena cofnienie ruchu przez algorytm minimax
    if (m_grid[row][col] != Cell::Empty) {
        m_grid[row][col] = Cell::Empty;
        --m_moveCount;
    }
}

bool Board::isFull()                  const { return m_moveCount >= m_size * m_size; }//uzywane przy sprawdzaniu czy plansza jest pelna
bool Board::isEmpty(int row, int col) const { return m_grid[row][col] == Cell::Empty; }//uzywane przy sprawdzaniu czy pole jest puste
Cell Board::get(int row, int col)     const { return m_grid[row][col]; }

//Sprawdzanie wygranej 
// Sprawdza 4 kierunki od pola (row,col): poziom, pion, ukosy
bool Board::checkWin(int row, int col, Cell player) const {//sprawdza czy ruch na (row,col) daje wygrana dla 'gracza
    const int dr[] = {0, 1, 1,  1};
    const int dc[] = {1, 0, 1, -1};

    for (int d = 0; d < 4; ++d) {
        int count = 1;
        // Liczymy w kierunku pozytywnym
        for (int k = 1; k < m_winLen; ++k) {//sprawdza kolejne pola w kierunku d, liczy ile jest znakow gracza
            int r = row + dr[d]*k, c = col + dc[d]*k;//sprawdza czy pole jest w planszy i czy jest zajete przez tego samego gracza
            if (r < 0 || r >= m_size || c < 0 || c >= m_size) break;//jesli pole jest poza plansza, przestaje liczyc w tym kierunku
            if (m_grid[r][c] != player) break;//jesli napotka pole puste lub zajete przez innego gracza, przestaje liczyc w tym kierunku
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

GameState Board::checkState() const {//sprawdza czy ktos wygral lub remis
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
std::optional<WinInfo> Board::getWinInfo() const {//sprawdza czy ktos wygral i zwraca informacje o zwycieskiej linii (start i koniec)
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


// Ocenia ciag znakow tego samego gracza:
// count-dlugosc ciagu
// openEnds-ile konców ciagu jest otwartych (0, 1 lub 2)
// Wynik jest dodatni dla maximizera, ujemny dla przeciwnika, maximizerem jest SI
int Board::evalLine(int count, int openEnds, Cell player, Cell maximizer) const {
    if (count == 0 || openEnds == 0) return 0;
//prosta heurystyka - im mniej do wygranej tym wykladniczo wiecej punktow za ten ruch
    int score = 0;
    if      (count >= m_winLen)          score = 100000;
    else if (count == m_winLen - 1)      score = (openEnds == 2) ? 50000 : 10000; 
    else if (count == m_winLen - 2)      score = (openEnds == 2) ? 5000  : 1000;  
    else if (count == m_winLen - 3)      score = (openEnds == 2) ? 500   : 100;
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
    if (m_moveCount == 0)
        return {{m_size / 2, m_size / 2}};

    const int RADIUS = 2;

    std::vector<bool> seen(m_size * m_size, false);
    std::vector<std::pair<int,int>> moves;
    moves.reserve(64);

    for (int r = 0; r < m_size; ++r) {
        for (int c = 0; c < m_size; ++c) {
            if (m_grid[r][c] == Cell::Empty) continue;

            for (int dr = -RADIUS; dr <= RADIUS; ++dr) {
                for (int dc = -RADIUS; dc <= RADIUS; ++dc) {
                    int nr = r + dr, nc = c + dc;
                    if (nr < 0 || nr >= m_size || nc < 0 || nc >= m_size) continue;
                    if (m_grid[nr][nc] != Cell::Empty) continue;

                    int flatIndex = nr * m_size + nc;
                    if (seen[flatIndex]) continue;

                    seen[flatIndex] = true;
                    moves.push_back({nr, nc});
                }
            }
        }
    }
    if (moves.size() > 20) moves.resize(20);
    return moves;
}