#pragma once

#include "GameObject.hpp"
#include "Input.hpp"

#include <chrono>

// Main class managing the game loop and overall application state.
class Application
{
public:

    Application();

    // Start the main application loop.
    void run();

private:

    // Initial setup of the game.
    void setup();

    // Handle key pressed events.
    void processEvents();

    // Update game state.
    void update(f32 dt);

    sf::RenderWindow window;
    Input input;

    ResourceManager resourceManager;
    GameObjectManager entityManager;

    sf::View gameView;
    sf::Font uiFont;
    sf::Text fpsText;
    sf::Text entityCountText;
    std::chrono::steady_clock::time_point lastFpsUpdateTime;
    usize frameCounter = 0;
    u32 fpsValue = 0;
    u32 entityCountValue = 0;
};