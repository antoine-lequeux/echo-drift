#pragma once

#include "../GameObject.hpp"
#include "CSprite.hpp"

// Simple collision layers for different entity types.
enum class Layer
{
    Player,
    Projectile,
    Asteroid
};

using Polygon = std::vector<sf::Vector2f>;

// Abstract component that provides a bounding shape for collision detection.
class CCollider : public Component
{
public:

    CCollider(GameObject& gameObject, Layer layer) : Component(gameObject), layer(layer) {}
    virtual ~CCollider() = default;

    void update(Context& ctx) override;

    // Tests collision using the pre-built cached polygons of both colliders.
    bool isTouching(const CCollider& other) const;

    void setLayer(Layer newLayer) { layer = newLayer; }
    Layer getLayer() const { return layer; }

    const Polygon& getPolygon() const { return cachedPolygon; }

    // Draw the shape for debugging purposes.
    virtual void showBounds(sf::RenderWindow& window) const = 0;

protected:

    // Compute and store the world-space polygon into cachedPolygon.
    virtual void rebuildPolygon() = 0;

    Layer layer;
    Polygon cachedPolygon;
};

// Derived component providing an ellipsoidal collider.
class CEllipseCollider : public CCollider
{
public:

    CEllipseCollider(GameObject& gameObject, Layer layer) : CCollider(gameObject, layer) {}

    void showBounds(sf::RenderWindow& window) const override;

    sf::Vector2f offset;
    f32 rx = 0.f;
    f32 ry = 0.f;

protected:

    void rebuildPolygon() override;
};

// Derived component providing a rectangular collider.
class CRectangleCollider : public CCollider
{
public:

    CRectangleCollider(GameObject& gameObject, Layer layer) : CCollider(gameObject, layer) {}

    void showBounds(sf::RenderWindow& window) const override;

    sf::Vector2f offset;
    sf::Vector2f halfSize;

protected:

    void rebuildPolygon() override;
};