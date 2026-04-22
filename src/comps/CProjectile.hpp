#pragma once

#include "../GameObject.hpp"

// Component representing a projectile fired by the player.
class CProjectile : public Component
{
public:

    CProjectile(GameObject& gameObject) : Component(gameObject) {}

    void update(Context& ctx) override {}
};
