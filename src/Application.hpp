#pragma once

#include "GameObject.hpp"
#include "Input.hpp"

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
    void update(float dt);

    sf::RenderWindow window;
    Input input;

    ResourceManager resourceManager;
    GameObjectManager entityManager;

    sf::View gameView;
};