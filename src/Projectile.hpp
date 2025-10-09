#pragma once
#include "Entity.hpp"

// Class representing a projectile fired by the spaceship.
class Projectile : public Entity
{
public:

    Projectile(std::string texturePath, sf::Vector2f position, sf::Vector2f speed, float rotation);

private:

    void updateBehavior(float dt, Input& input, EntityManager& entityManager) override;

    // Max distance before disappearing.
    float maxDistance = 800.f;

    sf::Vector2f startPosition;
};