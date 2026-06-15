#include <SFML/Graphics.hpp>
#include "Game.h"
#include "GUI.h"
#include "Menu.h"
#include "SI.h"
#include "BenchmarkSystem.hpp"
#include <chrono>
#include <memory>

namespace {
std::pair<int, int> mouseToCell(const sf::Vector2i& position, int boardSize, const sf::Vector2u& windowSize) {
    const float margin = 40.f;
    float boardWidth = static_cast<float>(windowSize.x) - margin * 2.f;
    float cellSize = boardWidth / boardSize;

    float localX = static_cast<float>(position.x) - margin;
    float localY = static_cast<float>(position.y) - margin;
    if (localX < 0.f || localY < 0.f || localX >= boardWidth || localY >= boardWidth)
        return {-1, -1};

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

    auto game = std::make_unique<Game>(cfg);
    auto gui  = std::make_unique<GUI>(*game, window);

    // Reset- R
    // Menu- Escape
    // Benchmark- B
    while (window.isOpen()) {
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();

            if (const auto* keyPressed = ev->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    GameConfig newCfg = menu.run();
                    if (!window.isOpen()) break;
                    game = std::make_unique<Game>(newCfg);
                    gui  = std::make_unique<GUI>(*game, window);
                }
                if (keyPressed->code == sf::Keyboard::Key::R) {
                    game->reset();
                }
                if (keyPressed->code == sf::Keyboard::Key::F) {
                    std::cout << "Uruchamiam pelny benchmark..." << std::endl;
                    BenchmarkSystem::RunFull("benchmark_full.csv", {3, 5, 7, 9}, 6, 3);
                }
                if (keyPressed->code == sf::Keyboard::Key::B) {
                    std::cout << "Uruchamiam benchmark..." << std::endl;
                    std::vector<BenchmarkResult> results;
                    for (int d = 1; d <= 6; ++d) {
                        AI ai(game->currentTurn(), {d});
                        auto start = std::chrono::high_resolution_clock::now();
                        ai.bestMove(const_cast<Board&>(game->board()));
                        auto end = std::chrono::high_resolution_clock::now();
                        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
                        results.push_back({d, game->board().size(), elapsed, ai.nodesVisited()});
                        std::cout << "Depth " << d << ": " << elapsed << " ms" << std::endl;
                    }
                    BenchmarkSystem::SaveToCSV("benchmark.csv", results);
                }
            }

            if (const auto* mousePressed = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left &&
                    game->state() == GameState::Playing &&
                    !game->isAITurn() &&
                    !game->isAIThinking()) {
                    auto [r, c] = mouseToCell(mousePressed->position, game->board().size(), window.getSize());
                    if (r >= 0)
                        game->handleMove(r, c);
                }
            }
        }

        if (game->state() == GameState::Playing && game->isAITurn() && !game->isAIThinking()) {
            game->triggerAIMove();
        }

        window.clear(sf::Color(18, 18, 24));
        gui->render();
        window.display();

        sf::sleep(sf::milliseconds(10));
    }

    return 0;
}