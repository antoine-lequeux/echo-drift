#include "CSpaceShip.hpp"
#include "CCollider.hpp"
#include "CDespawner.hpp"
#include "CMultiSprite.hpp"
#include "CProjectile.hpp"
#include "CSpeed.hpp"
#include "CSprite.hpp"

#include <algorithm>
#include <cmath>

CSpaceShip::CSpaceShip(GameObject& gameObject) : Component(gameObject) {}

void CSpaceShip::update(Context& ctx)
{
    auto transform = gameObject.getComponent<CTransform>();
    if (!transform) return;

    auto speedComp = gameObject.getComponent<CSpeed>();
    if (!speedComp) return;

    auto spriteComp = gameObject.getComponent<CMultiSprite>();
    if (!spriteComp) return;

    sf::Vector2f direction{0.f, 0.f};

    if (ctx.input.isActionActive(Action::MoveUp)) direction.y -= 1.f;
    if (ctx.input.isActionActive(Action::MoveDown)) direction.y += 1.f;
    if (ctx.input.isActionActive(Action::MoveLeft)) direction.x -= 1.f;
    if (ctx.input.isActionActive(Action::MoveRight)) direction.x += 1.f;

    if (direction.length() > 0)
    {
        // Normalize and apply movement speed.
        sf::Vector2f speed = direction.normalized() * moveSpeed;
        speedComp->setSpeed(speed);

        // Calculate target angle based on horizontal movement.
        f32 horizontalFactor = direction.normalized().x;
        targetAngle = horizontalFactor * maxTiltAngle;
    }
    else
    {
        speedComp->setSpeed({0.f, 0.f});
        targetAngle = 0.f;
    }

    if (abs(speedComp->getSpeed().x) > 0.7f * moveSpeed)
        spriteComp->setCurrentSpriteIndex(2);
    else if (abs(speedComp->getSpeed().x) > 0.2f * moveSpeed)
        spriteComp->setCurrentSpriteIndex(1);
    else
        spriteComp->setCurrentSpriteIndex(0);

    spriteComp->setInversionX(speedComp->getSpeed().x > 0.01f);

    timeSinceLastShot += ctx.dt;
    if (ctx.input.isActionActive(Action::Shoot) && timeSinceLastShot >= shootCooldown) shootProjectile(ctx);

    // Smoothly interpolate the rotation towards the target angle.
    f32 currentAngle = transform->getLocalRotation();
    f32 angleDiff = targetAngle - currentAngle;

    // Normalize angle difference to [-180, 180] range.
    angleDiff = std::fmod(angleDiff + 180.f, 360.f) - 180.f;

    // Calculate smooth rotation with easing.
    f32 rotationAmount = rotationSpeed * ctx.dt;
    if (std::abs(angleDiff) > 0.1f)
    {
        // Use sine easing for smooth acceleration and deceleration.
        f32 easeFactor = std::sin(std::min(std::abs(angleDiff) / maxTiltAngle, 1.f) * PI / 2.f);
        f32 angleChange = std::clamp(angleDiff, -rotationAmount, rotationAmount) * easeFactor;
        transform->setRotation(currentAngle + angleChange);
    }
    else
    {
        transform->setRotation(targetAngle);
    }
}

void CSpaceShip::shootProjectile(Context& ctx)
{
    timeSinceLastShot = 0.f;

    auto shipTransform = gameObject.getComponent<CTransform>();
    if (!shipTransform) return;

    auto shipSprite = gameObject.getComponent<CSprite>();
    if (!shipSprite) return;

    auto shipSpeed = gameObject.getComponent<CSpeed>();
    if (!shipSpeed) return;

    // Get the world position of the point located at the front of the ship.
    const sf::Vector2f shipFront = shipTransform->getGlobalPoint({0.f, -shipSprite->getSize().y / 2.f});
    const sf::Vector2f baseDirection = (shipFront - shipTransform->getGlobalPosition()).normalized();

    const u32 burstCount = std::max<u32>(1u, projectileBurstCount);
    const f32 halfSpread = projectileSpreadAngle / 2.f;
    const f32 angleStep = burstCount > 1 ? (halfSpread * 2.f) / static_cast<f32>(burstCount - 1) : 0.f;

    for (u32 i = 0; i < burstCount; ++i)
    {
        const f32 angleOffset = burstCount > 1 ? -halfSpread + static_cast<f32>(i) * angleStep : 0.f;
        const f32 angleRad = angleOffset * PI / 180.f;
        const sf::Vector2f rotatedDirection = {
            baseDirection.x * std::cos(angleRad) - baseDirection.y * std::sin(angleRad),
            baseDirection.x * std::sin(angleRad) + baseDirection.y * std::cos(angleRad),
        };

        auto& projectile = ctx.manager.spawn();

        auto& transform = projectile.addComponent<CTransform>();
        transform.setPosition(shipFront + shipSpeed->getSpeed() * ctx.dt * 3.f);
        transform.setRotation(shipTransform->getLocalRotation() + angleOffset);

        auto& sprite = projectile.addComponent<CSprite>(ctx.manager.resources.get<sf::Texture>(ResourceID::Projectile), 6);

        projectile.addComponent<CSpeed>(rotatedDirection.normalized() * 800.f);
        projectile.addComponent<CDespawner>();

        auto& col = projectile.addComponent<CEllipseCollider>(Layer::Projectile);
        col.rx = sprite.getSize().x;
        col.ry = sprite.getSize().x / 2.f;
        col.offset.y = -5.f;

        projectile.addComponent<CProjectile>();
    }
}
