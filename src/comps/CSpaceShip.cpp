#include "CSpaceShip.hpp"
#include "CCollider.hpp"
#include "CDespawner.hpp"
#include "CProjectile.hpp"
#include "CSpeed.hpp"
#include "CSprite.hpp"

CSpaceShip::CSpaceShip(GameObject& gameObject) : Component(gameObject) {}

void CSpaceShip::update(Context& ctx)
{
    auto transform = gameObject.getComponent<CTransform>();
    if (!transform)
        return;

    auto speedComp = gameObject.getComponent<CSpeed>();
    if (!speedComp)
        return;

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
        speedComp->setSpeed(speed);

        // Calculate target angle based on horizontal movement.
        float horizontalFactor = direction.normalized().x;
        targetAngle = horizontalFactor * maxTiltAngle;
    }
    else
    {
        speedComp->setSpeed({0.f, 0.f});
        targetAngle = 0.f;
    }

    timeSinceLastShot += ctx.dt;
    if (ctx.input.isActionTriggered(Action::Shoot) && timeSinceLastShot >= shootCooldown)
        shootProjectile(ctx);

    // Smoothly interpolate the rotation towards the target angle.
    float currentAngle = transform->getLocalRotation();
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
        transform->setRotation(currentAngle + angleChange);
    }
    else
    {
        transform->setRotation(targetAngle);
    }

    // Check for collisions with asteroids and destroy both on hit.
    auto thisCollider = gameObject.getComponent<CCollider>();
    for (auto& obj : ctx.manager.getAll())
    {
        if (obj.get() == &gameObject)
            continue;

        auto otherCollider = obj->getComponent<CCollider>();
        if (otherCollider && thisCollider && thisCollider->isTouching(*otherCollider))
        {
            if (otherCollider->getLayer() == Layer::Asteroid && thisCollider->getLayer() == Layer::Player)
            {
                std::cout << "Player hit by asteroid!\n";
                gameObject.destroy();
                obj->destroy();
            }
        }
    }
}

void CSpaceShip::shootProjectile(Context& ctx)
{
    timeSinceLastShot = 0.f;

    auto shipTransform = gameObject.getComponent<CTransform>();
    if (!shipTransform)
        return;

    auto shipSprite = gameObject.getComponent<CSprite>();
    if (!shipSprite)
        return;

    // Get the world position of the point located at the front of the ship.
    sf::Vector2f shipFront = shipTransform->getGlobalPoint({0.f, -shipSprite->getSize().y / 2.f});

    // Add a new projectile gameObject.
    auto& projectile = ctx.manager.spawn();

    auto& transform = projectile.addComponent<CTransform>();
    transform.setPosition(shipFront);
    transform.setRotation(shipTransform->getLocalRotation());

    auto& sprite = projectile.addComponent<CSprite>(ctx.manager.resources.get<sf::Texture>("projectile"), 6);

    auto& speedComp = projectile.addComponent<CSpeed>(
        sf::Vector2f{gameObject.getComponent<CSpeed>()->getSpeed().x * 0.8f, 0.f} + sf::Vector2f{0.f, -800.f});

    projectile.addComponent<CDespawner>();

    auto& col = projectile.addComponent<CEllipseCollider>(Layer::Projectile);
    col.rx = 6.f;
    col.ry = 6.f;
    col.offset.y = -15.f;

    projectile.addComponent<CProjectile>();
}
