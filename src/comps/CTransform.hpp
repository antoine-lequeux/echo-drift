#pragma once

#include "../GameObject.hpp"

#include <cmath>

// Component that stores a position/rotation/scale to manage the transformation data of a gameObject.
class CTransform : public Component
{
public:

    CTransform(GameObject& gameObject);

    void update(Context& ctx) override {}

    // Local setters/getters.
    void setPosition(sf::Vector2f pos);
    sf::Vector2f getLocalPosition() const;
    void move(sf::Vector2f offset);

    void setRotation(float angle);
    float getLocalRotation() const;

    void setRotationQuantization(float step);
    float getRotationQuantization() const;

    void setScale(sf::Vector2f factors);
    sf::Vector2f getLocalScale() const;

    // Global getters.
    sf::Vector2f getGlobalPosition() const;
    float getGlobalRotation() const;
    float getDisplayRotation() const;
    sf::Vector2f getGlobalScale() const;

    // Convert a local point to world space.
    sf::Vector2f getGlobalPoint(sf::Vector2f localPoint) const;

private:

    sf::Vector2f position{0.f, 0.f};
    float trueRotation = 0.f;
    float rotationStep = 6.f;
    sf::Vector2f scale{1.f, 1.f};

    float quantizeRotation(float angle) const { return std::round(angle / rotationStep) * rotationStep; }

    // Helper to get parent transform if exists.
    const CTransform* getParentTransform() const;
};
