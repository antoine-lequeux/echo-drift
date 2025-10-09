#include "Projectile.hpp"

Projectile::Projectile(std::string texturePath, sf::Vector2f position, sf::Vector2f speed, float rotation)
    : GameObject(texturePath), startPosition(position)
{
    setPosition(position);
    setSpeed(speed);
    setRotation(rotation);
    setScale({0.2f, 0.2f});
}

void Projectile::updateBehavior(float dt, Input& input, GameObjectManager& entityManager)
{
    if ((getPosition() - startPosition).length() > maxDistance)
    {
        markForDeletion();
    }
}
