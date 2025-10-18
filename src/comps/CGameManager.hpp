#pragma once

#include "../GameObject.hpp"
#include <numbers>

// Component that manages game state, for now spawning asteroids.
class CGameManager : public Component
{
public:

    CGameManager(GameObject& gameObject) : Component(gameObject) {}

    void update(Context& ctx) override;

private:

    // Utility to get the angle between a given vector and the X axis.
    float getAngleDegrees(const sf::Vector2f& v)
    {
        float angleRad = std::atan2(v.y, v.x);
        float angleDeg = angleRad * 180.f / std::numbers::pi;
        return angleDeg;
    }

    float spawnTimer = 0.f;
    float spawnInterval = 0.75f;
};