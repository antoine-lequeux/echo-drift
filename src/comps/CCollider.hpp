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
    float rotation = 0.f;
};

// Data used to compute collisions between a rectangle collider and another collider.
struct RectangleData
{
    float x, y;
    float w, h;
    float rotation = 0.f;
};

using Polygon = std::vector<sf::Vector2f>;

// Helpers that turn ellipses and rectangles to Polygons for unified logic.
Polygon ellipseToPolygon(EllipseData& e, unsigned int n = 32);
Polygon rectangleToPolygon(RectangleData& r);

// Helper that returns true if the projection of polygons A and B onto the axis overlap.
bool overlapOnAxis(const Polygon& shapeA, const Polygon& shapeB, const sf::Vector2f& axis);

// Helper that returns true if there is no axis separating polygons A and B (SAT collision check).
bool polygonsCollide(const Polygon& a, const Polygon& b);

// Turn a polygon into a sf::ConvexShape so it can be rendered.
sf::ConvexShape makeConvexShape(const Polygon& points);

// Helpers that create a EllipseData or a RectangleData based on a Collider.
EllipseData makeEllipseData(const CEllipseCollider& e);
RectangleData makeRectangleData(const CRectangleCollider& r);

// Helper that rotates a point around another point.
sf::Vector2f rotatePoint(const sf::Vector2f& p, const sf::Vector2f& center, float angleDeg);

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