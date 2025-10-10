#pragma once
#include "GameObject.hpp"
#include "Projectile.hpp"

// Class representing the player's spaceship.
class SpaceShip : public GameObject
{
public:

    SpaceShip(std::string texturePath, int drawOrder);

    // Update the spaceship's state based on input and elapsed time.
    void updateBehavior(float dt, Input& input, GameObjectManager& entityManager) override;

private:

    void shootProjectile(GameObjectManager& entityManager);

    // Movement speed in pixels per second.
    float moveSpeed = 400.f;

    // Maximum rotation speed in degrees per second.
    float rotationSpeed = 120.f;

    // Maximum tilt angle when moving sideways.
    float maxTiltAngle = 25.f;

    float targetAngle = 0.f;
};