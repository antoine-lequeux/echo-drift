#pragma once

#include "../GameObject.hpp"

#include <cmath>

// Component that stores a position/rotation/scale to manage the transformation data of a gameObject.
class CTransform : public Component
{
public:

    CTransform(GameObject& gameObject);

    void update(Context& ctx) override {}

    void setPosition(sf::Vector2f position);
    sf::Vector2f getPosition() const;

    void move(sf::Vector2f offset);

    void setRotation(float angle);
    float getRotation() const;
    float getDisplayRotation() const;

    void setRotationQuantization(float step);
    float getRotationQuantization() const;

    void setScale(sf::Vector2f factors);
    sf::Vector2f getScale() const;

    // Convert a local point to world space.
    sf::Vector2f getLocalPoint(sf::Vector2f localPoint) const;

private:

    sf::Vector2f position{0.f, 0.f};
    float trueRotation = 0.f;
    float rotationStep = 6.f;
    sf::Vector2f scale{1.f, 1.f};

    float quantizeRotation(float angle) const { return std::round(angle / rotationStep) * rotationStep; }
};
