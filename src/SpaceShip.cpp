#include "SpaceShip.hpp"
#include <cmath>
#include <iostream>

SpaceShip::SpaceShip(std::string texturePath) : GameObject(texturePath)
{
    setPosition({400.f, 300.f});
    setScale({3.f, 3.f});
}

void SpaceShip::updateBehavior(float dt, Input& input, GameObjectManager& entityManager)
{
    sf::Vector2f direction{0.f, 0.f};

    if (input.isActionActive(Action::MoveUp))
        direction.y -= 1.f;
    if (input.isActionActive(Action::MoveDown))
        direction.y += 1.f;
    if (input.isActionActive(Action::MoveLeft))
        direction.x -= 1.f;
    if (input.isActionActive(Action::MoveRight))
        direction.x += 1.f;

    if (direction.length() > 0)
    {
        // Normalize and apply movement speed.
        sf::Vector2f speed = direction.normalized() * moveSpeed;
        setSpeed(speed);

        // Calculate target angle based on horizontal movement.
        float horizontalFactor = direction.normalized().x;
        targetAngle = horizontalFactor * maxTiltAngle;
    }
    else
    {
        setSpeed({0.f, 0.f});
        targetAngle = 0.f;
    }

    if (input.isActionTriggered(Action::Shoot))
        shootProjectile(entityManager);

    // Smoothly interpolate the rotation towards the target angle.
    float currentAngle = getRotation();
    float angleDiff = targetAngle - currentAngle;

    // Normalize angle difference to [-180, 180] range.
    while (angleDiff > 180.f)
        angleDiff -= 360.f;
    while (angleDiff < -180.f)
        angleDiff += 360.f;

    // Calculate smooth rotation with easing.
    float rotationAmount = rotationSpeed * dt;
    if (std::abs(angleDiff) > 0.1f)
    {
        // Use sine easing for smooth acceleration and deceleration.
        float easeFactor = std::sin(std::min(std::abs(angleDiff) / maxTiltAngle, 1.f) * 3.14159f / 2.f);
        float angleChange = std::clamp(angleDiff, -rotationAmount, rotationAmount) * easeFactor;
        setRotation(currentAngle + angleChange);
    }
    else
    {
        setRotation(targetAngle);
    }
}

void SpaceShip::shootProjectile(GameObjectManager& entityManager)
{
    sf::Vector2f shipFront = getLocalPoint({0.f, -getSize().y / 2.f});

    // Add a new projectile entity.
    entityManager.addEntity(std::make_unique<Projectile>(
        "assets/laser/12.png", shipFront, getSpeed() * 0.7f + sf::Vector2f{0.f, -800.f}, getRotation() - 90.f, 0.2f));
}
