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

// Abstract component that provides a bounding shape for collision detection.
class CCollider : public Component
{
public:

    CCollider(GameObject& gameObject, Layer layer) : Component(gameObject), layer(layer) {}
    virtual ~CCollider() = default;

    void update(Context& ctx) override { showBounds(ctx.window); }

    void setLayer(Layer newLayer) { layer = newLayer; }
    Layer getLayer() const { return layer; }

    // Draw the shape for debugging purposes.
    virtual void showBounds(sf::RenderWindow& window) const = 0;

    // Visitor pattern (to handle every combination of collider types).
    virtual bool isTouching(const CCollider& other) const = 0;
    virtual bool isTouchingWith(const class CEllipseCollider& other) const = 0;
    virtual bool isTouchingWith(const class CRectangleCollider& other) const = 0;

protected:

    Layer layer;
};

// Data used to compute collisions between an ellispe collider and another collider.
struct EllipseData
{
    float x, y;
    float rx, ry;
};

// Data used to compute collisions between a rectangle collider and another collider.
struct RectangleData
{
    float x, y;
    float w, h;
};

// Helper functions to handle collision computation.
bool checkCollision(const EllipseData& a, const EllipseData& b);
bool checkCollision(const RectangleData& a, const RectangleData& b);
bool checkCollision(const EllipseData& e, const RectangleData& r);

// Derived component providing an ellipsoidal collider.
class CEllipseCollider : public CCollider
{
public:

    CEllipseCollider(GameObject& gameObject, Layer layer) : CCollider(gameObject, layer) {}

    void showBounds(sf::RenderWindow& window) const override;

    bool isTouching(const CCollider& other) const override;

    bool isTouchingWith(const CEllipseCollider& other) const override;
    bool isTouchingWith(const CRectangleCollider& other) const override;

    sf::Vector2f offset;
    float rx, ry;
};

// Derived component providing a rectangular collider.
class CRectangleCollider : public CCollider
{
public:

    CRectangleCollider(GameObject& gameObject, Layer layer) : CCollider(gameObject, layer) {}

    void showBounds(sf::RenderWindow& window) const override;

    bool isTouching(const CCollider& other) const override;

    bool isTouchingWith(const CEllipseCollider& other) const override;
    bool isTouchingWith(const CRectangleCollider& other) const override;

    sf::Vector2f offset;
    sf::Vector2f halfSize;
};