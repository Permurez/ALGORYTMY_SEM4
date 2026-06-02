#pragma once
#include <SFML/Graphics.hpp>
#include "Game.h"
#include <string>

class GUI {
public:
    explicit GUI(Game& game, sf::RenderWindow& window);//konstruktor, inicjalizuje GUI z referencja do gry i okna

    void render();
    std::pair<int,int> askHumanMove();//nie uzywane, ruchy sa obslugiwane prze mmousetocell w main.cpp

private:
    Game&             m_game;
    sf::RenderWindow& m_window;
    sf::Font          m_font;

    sf::Vector2f cellTopLeft(int r, int c) const;
    sf::Vector2f cellCenter(int r, int c) const;

    void drawBoard();
    void drawMarks();//rysuje kółka i krzyżyki
    void drawHUD();//rysuje informacje o aktualnym stanie gry, wyniki itp.
    void drawResultMessage();
    void drawWinLine(const WinInfo& win);//rysuje linie wskazujaca zwycieska linie
};
