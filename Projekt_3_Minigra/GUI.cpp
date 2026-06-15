#include "GUI.h"
#include <array>
#include <filesystem>

GUI::GUI(Game& game, sf::RenderWindow& window)//konstruktor, inicjalizuje GUI z referencja do gry i okna
    : m_game(game), m_window(window)
{
    const std::array<const char*, 4> fontPaths = {
        "assets/font.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };

    for (const char* path : fontPaths) {
        if (!std::filesystem::exists(path)) {
            continue;
        }
        if (m_font.openFromFile(path)) {
            break;
        }
    }
}

sf::Vector2f GUI::cellTopLeft(int r, int c) const {//zwraca współrzędne lewego górnego rogu pola (r,c) w pikselach
    const float margin = 40.f;
    float w = static_cast<float>(m_window.getSize().x) - margin*2.f;
    float cell = w / m_game.board().size();
    return {margin + c * cell, margin + r * cell};
}

sf::Vector2f GUI::cellCenter(int r, int c) const {//zwraca współrzędne środka pola (r,c) w pikselach
    auto tl = cellTopLeft(r, c);
    float w = (static_cast<float>(m_window.getSize().x) - 80.f) / m_game.board().size();
    return {tl.x + w/2.f, tl.y + w/2.f};
}

void GUI::drawBoard() {//rysuje plansze
    const int n = m_game.board().size();
    const float margin = 40.f;
    float w = static_cast<float>(m_window.getSize().x) - margin*2.f;
    float cell = w / n;

    sf::RectangleShape bg(sf::Vector2f(w, w));
    bg.setPosition({margin, margin});
    bg.setFillColor(sf::Color(24,24,30));
    m_window.draw(bg);

    sf::RectangleShape line;
    line.setFillColor(sf::Color(200,200,220));

    for (int i = 1; i < n; ++i) {//rysuje linie oddzielajace pola
        // vertical
        line.setSize({2.f, w});
        line.setPosition({margin + i*cell - 1.f, margin});
        m_window.draw(line);
        // horizontal
        line.setSize({w, 2.f});
        line.setPosition({margin, margin + i*cell - 1.f});
        m_window.draw(line);
    }
}

void GUI::drawMarks() {//rysuje symbole
    const Board& b = m_game.board();
    int n = b.size();
    float w = (static_cast<float>(m_window.getSize().x) - 80.f) / n;

    for (int r = 0; r < n; ++r) for (int c = 0; c < n; ++c) {
        Cell cell = b.get(r, c);
        if (cell == Cell::Empty) continue;
        auto center = cellCenter(r, c);

        if (cell == Cell::X) {//rysuje krzyzyk
            sf::Vertex line1[] = { sf::Vertex(), sf::Vertex() };
            line1[0].position = {center.x - w*0.3f, center.y - w*0.3f};
            line1[0].color = sf::Color::Red;
            line1[1].position = {center.x + w*0.3f, center.y + w*0.3f};
            line1[1].color = sf::Color::Red;
            sf::Vertex line2[] = { sf::Vertex(), sf::Vertex() };
            line2[0].position = {center.x + w*0.3f, center.y - w*0.3f};
            line2[0].color = sf::Color::Red;
            line2[1].position = {center.x - w*0.3f, center.y + w*0.3f};
            line2[1].color = sf::Color::Red;
            m_window.draw(line1, 2, sf::PrimitiveType::Lines);
            m_window.draw(line2, 2, sf::PrimitiveType::Lines);
        } else {//rysuje kolko
            sf::CircleShape circ(w*0.28f);
            circ.setOrigin({w*0.28f, w*0.28f});
            circ.setPosition({center.x, center.y});
            circ.setOutlineColor(sf::Color::Cyan);
            circ.setOutlineThickness(3.f);
            circ.setFillColor(sf::Color::Transparent);
            m_window.draw(circ);
        }
    }
}

void GUI::drawHUD() {//rysuje informacje o aktualnym stanie gry, wyniki itp.
    sf::Text stats(m_font, "X: " + std::to_string(m_game.scoreX()) + "  O: " + std::to_string(m_game.scoreO()), 18);
    stats.setFillColor(sf::Color(200,200,220));
    stats.setPosition({20.f, static_cast<float>(m_window.getSize().y) - 40.f});
    m_window.draw(stats);

    sf::Text turn(m_font, (m_game.currentTurn() == Cell::X ? "Tura: X" : "Tura: O"), 18);
    turn.setFillColor(sf::Color(200,200,220));
    turn.setPosition({static_cast<float>(m_window.getSize().x) - 140.f, static_cast<float>(m_window.getSize().y) - 40.f});
    m_window.draw(turn);

    // (no extra AI counters shown)
}

void GUI::drawResultMessage() {
    if (m_game.state() != GameState::WinX && m_game.state() != GameState::WinO) {
        if (m_game.state() != GameState::Draw) {
            return;
        }
    }
//koniec, komunikat o wyniku
    const std::string message = (m_game.state() == GameState::WinX)
        ? "Wygrywa krzyzyk"
        : (m_game.state() == GameState::WinO)
            ? "Wygrywa kolko"
            : "Remis";

    sf::Text result(m_font, message, 28);
    result.setFillColor(sf::Color::Yellow);

    auto bounds = result.getLocalBounds();
    result.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
    result.setPosition({static_cast<float>(m_window.getSize().x) / 2.f, 20.f});
    m_window.draw(result);
}

void GUI::drawWinLine(const WinInfo& win) {//rysuje linie wskazujaca zwycieska linie
    auto a = cellCenter(win.startRow, win.startCol);
    auto b = cellCenter(win.endRow, win.endCol);
    sf::Vertex line[] = { sf::Vertex(), sf::Vertex() };
    line[0].position = a;
    line[0].color = sf::Color::Yellow;
    line[1].position = b;
    line[1].color = sf::Color::Yellow;
    m_window.draw(line, 2, sf::PrimitiveType::Lines);
}

void GUI::render() {//glowna funkcja rysujaca cala plansze i HUD
    drawBoard();
    drawMarks();
    if (m_game.state() == GameState::WinX || m_game.state() == GameState::WinO) {
        auto win = m_game.board().getWinInfo();
        if (win) drawWinLine(*win);
    }
    drawResultMessage();
    drawHUD();
}

std::pair<int,int> GUI::askHumanMove() {//nie uzywane, ruchy sa obslugiwane prze mmousetocell w main.cpp
    // zczytanie ruchu gracza z klikniecia myszka
    while (m_window.isOpen()) {
        while (auto ev = m_window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) m_window.close();
            if (const auto* mousePressed = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    int n = m_game.board().size();
                    const float margin = 40.f;
                    float w = static_cast<float>(m_window.getSize().x) - margin*2.f;
                    float cell = w / n;
                    float mx = static_cast<float>(mousePressed->position.x) - margin;
                    float my = static_cast<float>(mousePressed->position.y) - margin;
                    if (mx >= 0 && my >= 0 && mx < w && my < w) {
                        int c = static_cast<int>(mx / cell);
                        int r = static_cast<int>(my / cell);
                        return {r, c};
                    }
                }
            }
        }
        
        sf::sleep(sf::milliseconds(10));
    }
    return {-1, -1};
}
