#pragma once

#include "../GameObject.hpp"

// Component that handles rendering a sprite with position, rotation, scaling, and draw order.
// This also serves as a Transform component since Sprites inherently have transform capabilities.
struct CSprite : Component
{
public:

    CSprite(GameObject& gameObject, std::string texturePath, int drawOrder);

    void update(Context& ctx) override;

    void setPosition(sf::Vector2f position);
    sf::Vector2f getPosition() const;

    // Convert a local point to world space.
    sf::Vector2f getLocalPoint(sf::Vector2f localPoint) const;

    void move(sf::Vector2f offset);

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

private:

    sf::Texture texture;
    sf::Sprite sprite;

    int drawOrder;

    // Rotation handling.
    float trueRotation = 0.f; // Actual rotation used for gameplay
    float rotationStep = 6.f; // Rotation quantization step (in degrees)

    // Quantize a rotation angle to the nearest step.
    float quantizeRotation(float angle) const { return std::round(angle / rotationStep) * rotationStep; }
};
