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

    void setRotation(f32 angle);
    f32 getLocalRotation() const;

    void setRotationQuantization(f32 step);
    f32 getRotationQuantization() const;

    void setScale(sf::Vector2f factors);
    sf::Vector2f getLocalScale() const;

    // Global getters.
    sf::Vector2f getGlobalPosition() const;
    f32 getGlobalRotation() const;
    f32 getDisplayRotation() const;
    sf::Vector2f getGlobalScale() const;

    // Convert a local point to world space.
    sf::Vector2f getGlobalPoint(sf::Vector2f localPoint) const;

private:

    sf::Vector2f position{0.f, 0.f};
    f32 trueRotation = 0.f;
    f32 rotationStep = 6.f;
    sf::Vector2f scale{1.f, 1.f};

    f32 quantizeRotation(f32 angle) const { return std::round(angle / rotationStep) * rotationStep; }

    // Helper to get parent transform if exists.
    const CTransform* getParentTransform() const;
};
