#include "GameObject.hpp"
#include <algorithm>
#include <iostream>
#include <ranges>

GameObject::GameObject() : texture(), sprite(texture) {}

GameObject::GameObject(std::string texturePath, int drawOrder) : texture(texturePath), sprite(texture), drawOrder(drawOrder)
{
    // Center the origin for easier transformations.
    sprite.setOrigin({sprite.getLocalBounds().size.x / 2.f, sprite.getLocalBounds().size.y / 2.f});
}

void GameObject::update(float dt, Input& input, GameObjectManager& entityManager)
{
    updatePhysics(dt);
    updateBehavior(dt, input, entityManager);
}

void GameObject::render(sf::RenderWindow& window) { window.draw(sprite); }

void GameObject::setPosition(sf::Vector2f position) { sprite.setPosition(position); }

sf::Vector2f GameObject::getPosition() const { return sprite.getPosition(); }

sf::Vector2f GameObject::getLocalPoint(sf::Vector2f localPoint) const
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

void GameObject::move(sf::Vector2f offset) { sprite.move(offset); }

void GameObject::setSpeed(sf::Vector2f newSpeed) { speed = newSpeed; }

sf::Vector2f GameObject::getSpeed() const { return speed; }

void GameObject::setRotation(float angle)
{
    // Store true rotation for physics/gameplay.
    trueRotation = angle;
    // Display quantized rotation.
    sprite.setRotation(sf::degrees(quantizeRotation(angle)));
}

float GameObject::getRotation() const { return trueRotation; }

float GameObject::getDisplayRotation() const { return sprite.getRotation().asDegrees(); }

void GameObject::setRotationQuantization(float step) { rotationStep = step; }

float GameObject::getRotationQuantization() const { return rotationStep; }

void GameObject::setScale(sf::Vector2f factors) { sprite.setScale(factors); }

sf::Vector2f GameObject::getScale() const { return sprite.getScale(); }

sf::Vector2f GameObject::getSize() const
{
    return {sprite.getLocalBounds().size.x * getScale().x, sprite.getLocalBounds().size.y * getScale().y};
}

void GameObject::updatePhysics(float dt) { move(speed * dt); }

void GameObjectManager::addEntity(std::unique_ptr<GameObject> entity) { pendingEntities.push_back(std::move(entity)); }

const GameObjects& GameObjectManager::getEntities() const { return entities; }

void GameObjectManager::update(float dt, Input& input)
{
    // Update entities.
    for (auto& entity : entities)
    {
        entity->update(dt, input, *this);
    }

    std::cout << "Entity count: " << entities.size() << "\n";

    // Remove marked entities.
    std::erase_if(entities, [](const std::unique_ptr<GameObject>& e) { return e->isMarkedForDeletion(); });

    // Add pending entities.
    std::ranges::move(pendingEntities, std::back_inserter(entities));

    pendingEntities.clear();
}

void GameObjectManager::render(sf::RenderWindow& window)
{
    // Sort entities by draw order before rendering.
    std::ranges::sort(entities, std::less{}, &GameObject::getDrawOrder);

    for (const auto& entity : entities)
    {
        entity->render(window);
    }
}
