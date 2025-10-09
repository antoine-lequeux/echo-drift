#include "Entity.hpp"

Entity::Entity(std::string texturePath) : texture(texturePath), sprite(texture)
{
    // Center the origin for easier transformations.
    sprite.setOrigin({sprite.getLocalBounds().size.x / 2.f, sprite.getLocalBounds().size.y / 2.f});
}

void Entity::update(float dt, Input& input, EntityManager& entityManager)
{
    updatePhysics(dt);
    updateBehavior(dt, input, entityManager);
}

void Entity::render(sf::RenderWindow& window) { window.draw(sprite); }

void Entity::setPosition(sf::Vector2f position) { sprite.setPosition(position); }

sf::Vector2f Entity::getPosition() const { return sprite.getPosition(); }

sf::Vector2f Entity::getLocalPoint(sf::Vector2f localPoint) const
{
    // Adjust for scaling.
    sf::Vector2f scale = sprite.getScale();
    localPoint.x /= scale.x;
    localPoint.y /= scale.y;

    // Calculate the sprite's half size.
    sf::Vector2f halfSize = {sprite.getLocalBounds().size.x * 0.5f, sprite.getLocalBounds().size.y * 0.5f};

    // Center the local point relative to sprite's origin.
    localPoint = {localPoint.x + halfSize.x, localPoint.y + halfSize.y};

    // Get the sprite's transformation.
    sf::Transform transform = sprite.getTransform();

    // Transform the local point to world space.
    return transform.transformPoint(localPoint);
}

void Entity::move(sf::Vector2f offset) { sprite.move(offset); }

void Entity::setSpeed(sf::Vector2f newSpeed) { speed = newSpeed; }

sf::Vector2f Entity::getSpeed() const { return speed; }

void Entity::setRotation(float angle)
{
    // Store true rotation for physics/gameplay.
    trueRotation = angle;
    // Display quantized rotation.
    sprite.setRotation(sf::degrees(quantizeRotation(angle)));
}

float Entity::getRotation() const { return trueRotation; }

float Entity::getDisplayRotation() const { return sprite.getRotation().asDegrees(); }

void Entity::setRotationQuantization(float step) { rotationStep = step; }

float Entity::getRotationQuantization() const { return rotationStep; }

void Entity::setScale(sf::Vector2f factors) { sprite.setScale(factors); }

sf::Vector2f Entity::getScale() const { return sprite.getScale(); }

sf::Vector2f Entity::getSize() const
{
    return {sprite.getLocalBounds().size.x * getScale().x, sprite.getLocalBounds().size.y * getScale().y};
}

void Entity::updatePhysics(float dt) { move(speed * dt); }

void EntityManager::addEntity(std::unique_ptr<Entity> entity)
{
    pendingEntities.push_back(std::move(entity));
}

const Entities & EntityManager::getEntities() const
{
    return entities;
}

void EntityManager::update(float dt, Input& input)
{
    // Update entities.
    for (auto& entity : entities)
    {
        entity->update(dt, input, *this);
    }

    // Remove marked entities
    entities.erase(std::remove_if(entities.begin(), entities.end(),
                                  [](const std::unique_ptr<Entity>& entity) { return entity->isMarkedForDeletion(); }),
                   entities.end());

    // Add pending entities.
    entities.insert(entities.end(), std::make_move_iterator(pendingEntities.begin()),
                    std::make_move_iterator(pendingEntities.end()));

    pendingEntities.clear();
}

void EntityManager::render(sf::RenderWindow& window)
{
    for (const auto& entity : entities)
    {
        entity->render(window);
    }
}
