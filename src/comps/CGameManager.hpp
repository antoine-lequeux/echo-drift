#pragma once

#include "../GameObject.hpp"

// Component that manages game state, for now spawning asteroids.
class CGameManager : public Component
{
public:

    CGameManager(GameObject& gameObject) : Component(gameObject) {}

    void update(Context& ctx) override;

private:

    // Utility to get a random float in [min, max].
    float getRandomFloat(float min, float max);

    float spawnTimer = 0.f;
    float spawnInterval = 0.75f;
};