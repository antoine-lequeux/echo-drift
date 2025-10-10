#pragma once
#include "Input.hpp"
#include <cmath>

class GameObjectManager;
class GameObject;

using GameObjects = std::vector<std::unique_ptr<GameObject>>;

// Base class for all game entities.
class GameObject
{
public:

    // Create a GameObject with no texture.
    GameObject();

    // Create a GameObject with a texture and optional draw order.
    GameObject(std::string texturePath, int drawOrder = 0);

    // Update the entity's state based on elapsed time and input.
    void update(float dt, Input& input, GameObjectManager& entityManager);

    // Render the entity to the given window.
    void render(sf::RenderWindow& window);

    // Setters and getters for position, speed, rotation, and scale:
    // =========
    void setPosition(sf::Vector2f position);
    sf::Vector2f getPosition() const;

    // Convert a local point to world space.
    sf::Vector2f getLocalPoint(sf::Vector2f localPoint) const;

    void move(sf::Vector2f offset);

    void setSpeed(sf::Vector2f newSpeed);
    sf::Vector2f getSpeed() const;

    void setRotation(float angle);

    float getRotation() const;
    float getDisplayRotation() const;

    void setRotationQuantization(float step);
    float getRotationQuantization() const;

    void setScale(sf::Vector2f factors);
    sf::Vector2f getScale() const;

    sf::Vector2f getSize() const;

    void setDrawOrder(int order) { drawOrder = order; }
    int getDrawOrder() const { return drawOrder; }
    // =========

    // Setter and getter for the deletion flag.
    void markForDeletion() { isDead = true; }
    bool isMarkedForDeletion() const { return isDead; }

protected:

    // Specific update method for derived classes.
    virtual void updateBehavior(float dt, Input& input, GameObjectManager& entityManager) = 0;

    // Basic physics update (movement based on speed).
    void updatePhysics(float dt);

private:

    sf::Vector2f speed;

    sf::Texture texture;
    sf::Sprite sprite;

    int drawOrder;

    // Flag to mark the entity for deletion.
    bool isDead = false;

    // Rotation handling.
    float trueRotation = 0.f; // Actual rotation used for gameplay
    float rotationStep = 6.f; // Rotation quantization step (in degrees)

    // Quantize a rotation angle to the nearest step.
    float quantizeRotation(float angle) const { return std::round(angle / rotationStep) * rotationStep; }
};

// Manager class to handle all entities in the game.
class GameObjectManager
{
public:

    // Add a new entity to the manager.
    void addEntity(std::unique_ptr<GameObject> entity);

    // Provide read-only access to entities for game objects
    const GameObjects& getEntities() const;

private:

    // Allow Application to access private update() and render() methods.
    friend class Application;

    // Update all entities and handle addition/removal.
    void update(float dt, Input& input);

    // Render all entities.
    void render(sf::RenderWindow& window);

    // Entities to be added after the current update cycle.
    GameObjects pendingEntities;

    // All current game entities.
    GameObjects entities;
};