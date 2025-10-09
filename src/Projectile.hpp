#pragma once
#include "GameObject.hpp"

// Class representing a projectile moving in a straight line.
class Projectile : public GameObject
{
public:

    Projectile(std::string texturePath, sf::Vector2f position, sf::Vector2f speed, float rotation);

private:

    void updateBehavior(float dt, Input& input, GameObjectManager& entityManager) override;

    // Max distance before disappearing.
    float maxDistance = 800.f;

    sf::Vector2f startPosition;
};