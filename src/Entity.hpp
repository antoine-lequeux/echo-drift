#pragma once
#include "Input.hpp"

// Base class for all game entities.
class Entity
{
public:

    Entity(std::string texturePath);

    // Update the entity's state based on elapsed time and input.
    void update(float dt, Input& input);

    // Render the entity to the given window.
    void render(sf::RenderWindow& window);

    // Setters and getters for position, speed, rotation, and scale:
    // =========
    void setPosition(sf::Vector2f position) { sprite.setPosition(position); }
    sf::Vector2f getPosition() const { return sprite.getPosition(); }

    void move(sf::Vector2f offset) { sprite.move(offset); }

    void setSpeed(sf::Vector2f newSpeed) { speed = newSpeed; }
    sf::Vector2f getSpeed() const { return speed; }

    void setRotation(float angle) { sprite.setRotation(sf::degrees(angle)); }
    float getRotation() const { return sprite.getRotation().asDegrees(); }

    void setScale(sf::Vector2f factors) { sprite.setScale(factors); }
    sf::Vector2f getScale() const { return sprite.getScale(); }
    // =========

protected:

    // Specific update method for derived classes.
    virtual void updateBehavior(float dt, Input& input) = 0;

    // Basic physics update (movement based on speed).
    void updatePhysics(float dt);

private:

    sf::Vector2f speed;

    sf::Texture texture;
    sf::Sprite sprite;
};