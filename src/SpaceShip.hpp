#pragma once
#include "Input.hpp"

// Class representing the player's spaceship.
class SpaceShip
{
public:

    SpaceShip(std::string texturePath, float s);

    // Update the spaceship's state based on input and elapsed time.
    void update(float dt, Input& input);

    // Render the spaceship to the given window.
    void render(sf::RenderWindow& window);

private:

    float speed;

    sf::Texture texture;
    sf::Sprite sprite;
};