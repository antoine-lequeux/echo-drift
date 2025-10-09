#include "SpaceShip.hpp"
#include <iostream>

SpaceShip::SpaceShip(std::string texturePath) : Entity(texturePath)
{
    setPosition({400.f, 300.f});
    setScale({3.f, 3.f});
}

void SpaceShip::updateBehavior(float dt, Input& input, EntityManager& entityManager)
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
        sf::Vector2f speed = direction.normalized() * moveSpeed;
        setSpeed(speed);
    }
    else
    {
        setSpeed({0.f, 0.f});
    }

    if (input.isActionTriggered(Action::Shoot))
        shootProjectile(entityManager);
}

void SpaceShip::shootProjectile(EntityManager& entityManager)
{
    // Add a new projectile entity.
    entityManager.addEntity(
        std::make_unique<Projectile>("assets/laser/12.png", getPosition() - sf::Vector2f{0.f, getSize().y / 2.f},
                                     getSpeed() * 0.5f + sf::Vector2f{0.f, -600.f}, getRotation() - 90.f));
}
