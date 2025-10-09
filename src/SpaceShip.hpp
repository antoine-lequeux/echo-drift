#pragma once
#include "Entity.hpp"
#include "Projectile.hpp"

// Class representing the player's spaceship.
class SpaceShip : public Entity
{
public:

    SpaceShip(std::string texturePath);

    // Update the spaceship's state based on input and elapsed time.
    void updateBehavior(float dt, Input& input, EntityManager& entityManager) override;

private:

    void shootProjectile(EntityManager& entityManager);

    // Movement speed in pixels per second.
    float moveSpeed = 400.f;
};