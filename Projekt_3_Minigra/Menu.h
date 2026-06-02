#pragma once
#include <SFML/Graphics.hpp>
#include "Game.h"
#include <string>

class Menu {
public:
    explicit Menu(sf::RenderWindow& window);

    GameConfig run();

private:
    sf::RenderWindow& m_window;
    sf::Font          m_font;

    GameConfig m_cfg;

    int  m_boardSize = 3;
    int  m_winLen    = 3;
    int  m_modeIndex  = 1;
    int  m_depthIndex = 1;

    void handleEvent(const sf::Event& event);
    void render();

    void drawTitle();
    void drawOption(const std::string& label, const std::string& value,
                    float y, bool selected);
    void drawInstructions();

    int  m_selectedOption = 0;
    bool m_done = false;
};
