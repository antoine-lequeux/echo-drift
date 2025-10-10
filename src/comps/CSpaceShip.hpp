#pragma once

#include "../GameObject.hpp"

// Component representing the player's spaceship.
class CSpaceShip : public Component
{
public:

    CSpaceShip(GameObject& gameObject);

    void update(Context& ctx) override;

private:

    void shootProjectile(Context& ctx);

    // Movement speed in pixels per second.
    float moveSpeed = 400.f;

    // Maximum rotation speed in degrees per second.
    float rotationSpeed = 120.f;

    // Maximum tilt angle when moving sideways.
    float maxTiltAngle = 25.f;

    float targetAngle = 0.f;

    // Seconds between shots.
    float shootCooldown = 0.15f;
    float timeSinceLastShot = 0.f;
};