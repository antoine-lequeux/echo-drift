#pragma once

#include "../GameObject.hpp"
#include "CSpaceShip.hpp"
#include "CSprite.hpp"

// Simple collision layers for different entity types.
enum class Layer
{
    Player,
    Projectile,
    Asteroid
};

// Component that provides a bounding shape for collision detection.
// For now the shape is just a rectangle based on the sprite's position and size.
class CCollider : public Component
{
public:

    CCollider(GameObject& gameObject, Layer layer) : Component(gameObject), layer(layer) {}

    void update(Context& ctx) override {}

    // Get the bounding rectangle of the collider in world space.
    sf::FloatRect getBounds() const
    {
        auto transform = gameObject.getComponent<CTransform>();
        if (!transform)
            return sf::FloatRect();

        auto sprite = gameObject.getComponent<CSprite>();
        if (!sprite)
            return sf::FloatRect();

        return sf::FloatRect{transform->getPosition(), sprite->getWorldSize()};
    }

    Layer getLayer() const { return layer; }

    // Check for collision with another collider using axis-aligned bounding box (AABB) method.
    bool isCollidingWith(const CCollider& other) const
    {
        return getBounds().findIntersection(other.getBounds()).has_value();
    }

private:

    Layer layer;
};