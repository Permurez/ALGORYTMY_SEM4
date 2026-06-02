#include <SFML/Graphics.hpp>
#include "Game.h"
#include "GUI.h"
#include "Menu.h"

namespace {
std::pair<int, int> mouseToCell(const sf::Vector2i& position, int boardSize, const sf::Vector2u& windowSize) {
    const float margin = 40.f;
    float boardWidth = static_cast<float>(windowSize.x) - margin * 2.f;
    float cellSize = boardWidth / boardSize;

    float localX = static_cast<float>(position.x) - margin;
    float localY = static_cast<float>(position.y) - margin;
    if (localX < 0.f || localY < 0.f || localX >= boardWidth || localY >= boardWidth) {
        return {-1, -1};
    }

    return {
        static_cast<int>(localY / cellSize),
        static_cast<int>(localX / cellSize)
    };
}
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 800}), "Kolko i krzyzyk");
    window.setFramerateLimit(60);

    Menu menu(window);
    GameConfig cfg = menu.run();
    if (!window.isOpen()) return 0;

    Game game(cfg);
    GUI gui(game, window);

    while (window.isOpen()) {
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            if (const auto* keyPressed = ev->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    GameConfig newCfg = menu.run();
                    if (!window.isOpen()) break;
                    game = Game(newCfg);
                }
                if (keyPressed->code == sf::Keyboard::Key::R) {
                    game.reset();
                }
            }

            if (const auto* mousePressed = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left &&
                    game.state() == GameState::Playing && !game.isAITurn()) {
                    auto [r, c] = mouseToCell(mousePressed->position, game.board().size(), window.getSize());
                    if (r >= 0) {
                        game.handleMove(r, c);
                    }
                }
            }
        }

        if (game.state() == GameState::Playing && game.isAITurn()) {
            game.triggerAIMove();
        }

        window.clear(sf::Color(18,18,24));
        gui.render();
        window.display();

        sf::sleep(sf::milliseconds(10));
    }

    return 0;
}
