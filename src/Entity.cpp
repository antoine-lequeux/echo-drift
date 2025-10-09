#include "Entity.hpp"

Entity::Entity(std::string texturePath) : texture(texturePath), sprite(texture)
{
    // Center the origin for easier transformations.
    sprite.setOrigin({sprite.getLocalBounds().size.x / 2, sprite.getLocalBounds().size.y / 2});
}

void Entity::update(float dt, Input& input, EntityManager& entityManager)
{
    updatePhysics(dt);
    updateBehavior(dt, input, entityManager);
}

void Entity::render(sf::RenderWindow& window) { window.draw(sprite); }

void Entity::updatePhysics(float dt) { move(speed * dt); }
