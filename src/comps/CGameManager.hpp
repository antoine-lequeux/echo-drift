#pragma once

#include "../GameObject.hpp"

// Component that manages game state, for now spawning asteroids.
class CGameManager : public Component
{
public:

    CGameManager(GameObject& gameObject) : Component(gameObject) {}

    void update(Context& ctx) override;

private:

    // Utility to get the angle between a given vector and the X axis.
    f32 getAngleDegrees(const sf::Vector2f& v)
    {
        f32 angleRad = std::atan2(v.y, v.x);
        f32 angleDeg = angleRad * 180.f / PI;
        return angleDeg;
    }

    f32 spawnTimer = 0.f;
    f32 spawnInterval = 0.75f;
};