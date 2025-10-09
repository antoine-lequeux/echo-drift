#pragma once
#include "Entity.hpp"

// Class representing the player's spaceship.
class SpaceShip : public Entity
{
public:

    SpaceShip(std::string texturePath);

    // Update the spaceship's state based on input and elapsed time.
    void updateBehavior(float dt, Input& input) override;

private:

    // Movement speed in pixels per second
    float moveSpeed = 300.f;
};