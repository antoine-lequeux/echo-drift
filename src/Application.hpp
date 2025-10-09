#pragma once
#include "GameManager.hpp"
#include "SpaceShip.hpp"

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

    // Render the current frame.
    void render();

    sf::RenderWindow window;
    Input input;

    GameObjectManager entityManager;
};