#include "Menu.h"
#include <string>
#include <array>
#include <algorithm>
#include <filesystem>


Menu::Menu(sf::RenderWindow& window) : m_window(window) {
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


GameConfig Menu::run() {
    m_done = false;
    m_selectedOption = 0;

    while (m_window.isOpen() && !m_done) {
        while (auto event = m_window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                m_window.close();
            handleEvent(*event);
        }
        m_window.clear(sf::Color(18, 18, 24));
        render();
        m_window.display();
    }

    // Build config from selections
    GameConfig cfg;
    cfg.boardSize = m_boardSize;
    cfg.winLen    = std::min(m_winLen, m_boardSize);

    const std::array<GameMode, 2> modes = {GameMode::HumanVsHuman, GameMode::HumanVsAI};
    cfg.mode = modes[m_modeIndex];

    const std::array<int, 3> depths = {3, 6, 9};
    cfg.aiCfg.maxDepth = depths[m_depthIndex];

    return cfg;
}


void Menu::handleEvent(const sf::Event& event) {
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (!keyPressed) return;

    const int optionCount = 4;

    switch (keyPressed->code) {
        case sf::Keyboard::Key::Up:
            m_selectedOption = (m_selectedOption - 1 + optionCount) % optionCount;
            break;
        case sf::Keyboard::Key::Down:
            m_selectedOption = (m_selectedOption + 1) % optionCount;
            break;
        case sf::Keyboard::Key::Left:
            switch (m_selectedOption) {
                case 0:
                    m_boardSize = std::max(3, m_boardSize - 1);
                    m_winLen = std::min(m_winLen, m_boardSize);
                    break;
                case 1: m_winLen     = std::max(3, m_winLen - 1); break;
                case 2: m_modeIndex  = (m_modeIndex - 1 + 2) % 2; break;
                case 3: m_depthIndex = std::max(0, m_depthIndex - 1); break;
            }
            break;
        case sf::Keyboard::Key::Right:
            switch (m_selectedOption) {
                case 0: m_boardSize = std::min(10, m_boardSize + 1); break;
                case 1: m_winLen    = std::min(m_boardSize, m_winLen + 1); break;
                case 2: m_modeIndex  = (m_modeIndex + 1) % 2; break;
                case 3: m_depthIndex = std::min(2, m_depthIndex + 1); break;
            }
            break;
        case sf::Keyboard::Key::Enter:
        case sf::Keyboard::Key::Space:
            m_done = true;
            break;
        default: break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void Menu::render() {
    drawTitle();

    const std::array<std::string, 2> modeNames  = {"Gracz vs Gracz", "Gracz vs AI"};
    const std::array<std::string, 3> diffNames   = {"Latwy (3)", "Sredni (6)", "Trudny (9)"};

    float startY = 240.f;
    float step   = 80.f;

    drawOption("Rozmiar planszy",  std::to_string(m_boardSize) + " x " + std::to_string(m_boardSize),
               startY + 0 * step, m_selectedOption == 0);
    drawOption("Znaki do wygranej", std::to_string(m_winLen),
               startY + 1 * step, m_selectedOption == 1);
    drawOption("Tryb gry", modeNames[m_modeIndex],
               startY + 2 * step, m_selectedOption == 2);
    drawOption("Poziom AI", diffNames[m_depthIndex],
               startY + 3 * step, m_selectedOption == 3);

    drawInstructions();
}

void Menu::drawTitle() {
    sf::Text title(m_font, "KOLKO I KRZYZYK", 42);
    title.setFillColor(sf::Color(220, 220, 255));
    title.setStyle(sf::Text::Bold);
    auto bounds = title.getLocalBounds();
    title.setPosition({(m_window.getSize().x - bounds.size.x) / 2.f, 60.f});
    m_window.draw(title);

    sf::Text sub(m_font, "MinMax AI z Alpha-Beta Pruning", 18);
    sub.setFillColor(sf::Color(120, 120, 180));
    auto sb = sub.getLocalBounds();
    sub.setPosition({(m_window.getSize().x - sb.size.x) / 2.f, 120.f});
    m_window.draw(sub);
}

void Menu::drawOption(const std::string& label, const std::string& value,
                      float y, bool selected) {
    float cx = m_window.getSize().x / 2.f;

    if (selected) {
        sf::RectangleShape bg(sf::Vector2f(500.f, 56.f));
        bg.setFillColor(sf::Color(40, 40, 80));
        bg.setOutlineColor(sf::Color(100, 100, 220));
        bg.setOutlineThickness(1.5f);
        bg.setPosition({cx - 250.f, y - 8.f});
        m_window.draw(bg);
    }

    sf::Text lbl(m_font, label, 20);
    lbl.setFillColor(selected ? sf::Color(200, 200, 255) : sf::Color(140, 140, 170));
    lbl.setPosition({cx - 240.f, y});
    m_window.draw(lbl);

    sf::Text val(m_font, (selected ? "< " : "  ") + value + (selected ? " >" : "  "), 20);
    val.setFillColor(selected ? sf::Color(255, 220, 80) : sf::Color(180, 180, 180));
    auto vb = val.getLocalBounds();
    val.setPosition({cx + 240.f - vb.size.x, y});
    m_window.draw(val);
}

void Menu::drawInstructions() {
    float y = m_window.getSize().y - 180.f; // Podniosłem tekst wyżej, żeby się zmieścił
    const std::vector<std::string> lines = {
        "^ | Wybierz opcje  <-> Zmien wartosc",
        "Enter / Spacja: Rozpocznij gre",
        "Esc: Powrot do menu    R: Restart rundy",
        "B: Benchmark ",
        "N: Nowa gra "
    };

    for (size_t i = 0; i < lines.size(); ++i) {
        sf::Text t(m_font, lines[i], 16);
        t.setFillColor(sf::Color(90, 90, 110));
        auto b = t.getLocalBounds();
        // Wyśrodkowanie tekstu i odstęp 25 pikseli między liniami
        t.setPosition({(m_window.getSize().x - b.size.x) / 2.f, y + i * 25.f});
        m_window.draw(t);
    }
}
