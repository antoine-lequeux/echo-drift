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
    f32 moveSpeed = 400.f;

    // Maximum rotation speed in degrees per second.
    f32 rotationSpeed = 120.f;

    // Maximum tilt angle when moving sideways.
    f32 maxTiltAngle = 25.f;

    f32 targetAngle = 0.f;

    // Seconds between shots.
    f32 shootCooldown = 0.15f;
    f32 timeSinceLastShot = 0.f;
};