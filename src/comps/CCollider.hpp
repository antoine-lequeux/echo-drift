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

    void update(Context& ctx) override
    {
        // sf::FloatRect bounds = getBounds();

        // sf::RectangleShape rect;
        // rect.setPosition({bounds.position.x, bounds.position.y});
        // rect.setSize({bounds.size.x, bounds.size.y});
        // rect.setFillColor(sf::Color::Transparent);
        // rect.setOutlineColor(sf::Color::Green);
        // rect.setOutlineThickness(1.f);

        //ctx.window.draw(rect);
    }

    // Get the bounding rectangle of the collider in world space.
    sf::FloatRect getBounds() const
    {
        auto transform = gameObject.getComponent<CTransform>();
        if (!transform)
            return sf::FloatRect();

        auto sprite = gameObject.getComponent<CSprite>();
        if (!sprite)
            return sf::FloatRect();

        sf::Vector2f halfSize = sprite->getSize() / 2.f;

        sf::Vector2f topLeft(-halfSize.x, -halfSize.y);
        sf::Vector2f topRight(halfSize.x, -halfSize.y);
        sf::Vector2f bottomLeft(-halfSize.x, halfSize.y);
        sf::Vector2f bottomRight(halfSize.x, halfSize.y);

        sf::Transform t;
        t.translate(transform->getGlobalPosition());
        t.rotate(sf::degrees(transform->getGlobalRotation()));
        t.scale(transform->getGlobalScale());

        topLeft = t.transformPoint(topLeft);
        topRight = t.transformPoint(topRight);
        bottomLeft = t.transformPoint(bottomLeft);
        bottomRight = t.transformPoint(bottomRight);

        float minX = std::min({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
        float maxX = std::max({topLeft.x, topRight.x, bottomLeft.x, bottomRight.x});
        float minY = std::min({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});
        float maxY = std::max({topLeft.y, topRight.y, bottomLeft.y, bottomRight.y});

        return sf::FloatRect({minX, minY}, {maxX - minX, maxY - minY});
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