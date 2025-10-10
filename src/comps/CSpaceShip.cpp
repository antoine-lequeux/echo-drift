#include "CSpaceShip.hpp"
#include "CSpeed.hpp"
#include "CSprite.hpp"

CSpaceShip::CSpaceShip(GameObject& gameObject) : Component(gameObject) {}

void CSpaceShip::update(Context& ctx)
{
    sf::Vector2f direction{0.f, 0.f};

    if (ctx.input.isActionActive(Action::MoveUp))
        direction.y -= 1.f;
    if (ctx.input.isActionActive(Action::MoveDown))
        direction.y += 1.f;
    if (ctx.input.isActionActive(Action::MoveLeft))
        direction.x -= 1.f;
    if (ctx.input.isActionActive(Action::MoveRight))
        direction.x += 1.f;

    if (direction.length() > 0)
    {
        // Normalize and apply movement speed.
        sf::Vector2f speed = direction.normalized() * moveSpeed;
        gameObject.getComponent<CSpeed>()->setSpeed(speed);

        // Calculate target angle based on horizontal movement.
        float horizontalFactor = direction.normalized().x;
        targetAngle = horizontalFactor * maxTiltAngle;
    }
    else
    {
        gameObject.getComponent<CSpeed>()->setSpeed({0.f, 0.f});
        targetAngle = 0.f;
    }

    timeSinceLastShot += ctx.dt;
    if (ctx.input.isActionTriggered(Action::Shoot) && timeSinceLastShot >= shootCooldown)
        shootProjectile(ctx);

    // Smoothly interpolate the rotation towards the target angle.
    float currentAngle = gameObject.getComponent<CSprite>()->getRotation();
    float angleDiff = targetAngle - currentAngle;

    // Normalize angle difference to [-180, 180] range.
    while (angleDiff > 180.f)
        angleDiff -= 360.f;
    while (angleDiff < -180.f)
        angleDiff += 360.f;

    // Calculate smooth rotation with easing.
    float rotationAmount = rotationSpeed * ctx.dt;
    if (std::abs(angleDiff) > 0.1f)
    {
        // Use sine easing for smooth acceleration and deceleration.
        float easeFactor = std::sin(std::min(std::abs(angleDiff) / maxTiltAngle, 1.f) * 3.14159f / 2.f);
        float angleChange = std::clamp(angleDiff, -rotationAmount, rotationAmount) * easeFactor;
        gameObject.getComponent<CSprite>()->setRotation(currentAngle + angleChange);
    }
    else
    {
        gameObject.getComponent<CSprite>()->setRotation(targetAngle);
    }
}

void CSpaceShip::shootProjectile(Context& ctx)
{
    timeSinceLastShot = 0.f;

    auto shipSprite = gameObject.getComponent<CSprite>();

    sf::Vector2f shipFront = shipSprite->getLocalPoint({0.f, -shipSprite->getSize().y / 2.f});

    // Add a new projectile gameObject.
    auto projectile = std::make_unique<GameObject>();

    auto& sprite = projectile->addComponent<CSprite>("assets/laser/12.png", 6);
    sprite.setPosition(shipFront);
    sprite.setScale({0.2f, 0.2f});
    sprite.setRotation(gameObject.getComponent<CSprite>()->getRotation() - 90.f);

    auto& speedComp = projectile->addComponent<CSpeed>(
        sf::Vector2f{gameObject.getComponent<CSpeed>()->getSpeed().x * 0.8f, 0.f} + sf::Vector2f{0.f, -800.f});

    ctx.manager.spawn(std::move(projectile));
}
