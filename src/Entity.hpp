#pragma once
#include "Input.hpp"
#include <cmath>

class EntityManager;
class Entity;

using Entities = std::vector<std::unique_ptr<Entity>>;

// Base class for all game entities.
class Entity
{
public:

    Entity(std::string texturePath);

    // Update the entity's state based on elapsed time and input.
    void update(float dt, Input& input, EntityManager& entityManager);

    // Render the entity to the given window.
    void render(sf::RenderWindow& window);

    // Setters and getters for position, speed, rotation, and scale:
    // =========
    void setPosition(sf::Vector2f position) { sprite.setPosition(position); }
    sf::Vector2f getPosition() const { return sprite.getPosition(); }

    // Convert a local point to world space.
    sf::Vector2f getLocalPoint(sf::Vector2f localPoint) const
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

    void move(sf::Vector2f offset) { sprite.move(offset); }

    void setSpeed(sf::Vector2f newSpeed) { speed = newSpeed; }
    sf::Vector2f getSpeed() const { return speed; }

    void setRotation(float angle)
    {
        // Store true rotation for physics/gameplay.
        trueRotation = angle;
        // Display quantized rotation.
        sprite.setRotation(sf::degrees(quantizeRotation(angle)));
    }

    float getRotation() const { return trueRotation; }
    float getDisplayRotation() const { return sprite.getRotation().asDegrees(); }

    void setRotationQuantization(float step) { rotationStep = step; }
    float getRotationQuantization() const { return rotationStep; }

    void setScale(sf::Vector2f factors) { sprite.setScale(factors); }
    sf::Vector2f getScale() const { return sprite.getScale(); }

    sf::Vector2f getSize() const
    {
        return {sprite.getLocalBounds().size.x * getScale().x, sprite.getLocalBounds().size.y * getScale().y};
    }
    // =========

    // Setter and getter for the deletion flag.
    void markForDeletion() { isDead = true; }
    bool isMarkedForDeletion() const { return isDead; }

protected:

    // Specific update method for derived classes.
    virtual void updateBehavior(float dt, Input& input, EntityManager& entityManager) = 0;

    // Basic physics update (movement based on speed).
    void updatePhysics(float dt);

private:

    sf::Vector2f speed;

    sf::Texture texture;
    sf::Sprite sprite;

    // Flag to mark the entity for deletion.
    bool isDead = false;

    // Rotation handling.
    float trueRotation = 0.f; // Actual rotation used for gameplay
    float rotationStep = 6.f; // Rotation quantization step (in degrees)

    // Quantize a rotation angle to the nearest step.
    float quantizeRotation(float angle) const { return std::round(angle / rotationStep) * rotationStep; }
};

// Manager class to handle all entities in the game.
class EntityManager
{
public:

    // Add a new entity to the manager.
    void addEntity(std::unique_ptr<Entity> entity) { pendingEntities.push_back(std::move(entity)); }

    // Provide read-only access to entities for game objects
    const Entities& getEntities() const { return entities; }

private:

    // Allow Application to access private update() and render() methods.
    friend class Application;

    // Update all entities and handle addition/removal.
    void update(float dt, Input& input)
    {
        // Update entities.
        for (auto& entity : entities)
        {
            entity->update(dt, input, *this);
        }

        // Remove marked entities
        entities.erase(std::remove_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& entity)
                                      { return entity->isMarkedForDeletion(); }),
                       entities.end());

        // Add pending entities.
        entities.insert(entities.end(), std::make_move_iterator(pendingEntities.begin()),
                        std::make_move_iterator(pendingEntities.end()));

        pendingEntities.clear();
    }

    // Render all entities.
    void render(sf::RenderWindow& window)
    {
        for (const auto& entity : entities)
        {
            entity->render(window);
        }
    }

    // Entities to be added after the current update cycle.
    Entities pendingEntities;

    // All current game entities.
    Entities entities;
};