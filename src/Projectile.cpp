#include "Projectile.hpp"

Projectile::Projectile(std::string texturePath, sf::Vector2f position, sf::Vector2f speed, float rotation, float scale, int drawOrder)
    : GameObject(texturePath, drawOrder), startPosition(position)
{
    setPosition(position);
    setSpeed(speed);
    setRotation(rotation);
    setScale({scale, scale});
}

void Projectile::updateBehavior(float dt, Input& input, GameObjectManager& entityManager)
{
    if ((getPosition() - startPosition).length() > maxDistance)
    {
        markForDeletion();
    }
}
