#pragma once

#include "GameObject.hpp"
#include "Projectile.hpp"

// Class managing game state, spawning entities, and overall game logic.
class GameManager : public GameObject
{
public:

    GameManager();

private:

    void updateBehavior(float dt, Input& input, GameObjectManager& entityManager) override;

    // Utility to get a random float in [min, max].
    float getRandomFloat(float min, float max);

    float spawnTimer = 0.f;
    float spawnInterval = 0.75f;
};