#include "CCollider.hpp"

Polygon ellipseToPolygon(EllipseData& e, unsigned int n)
{
    Polygon p;
    for (unsigned int i = 0; i < n; ++i)
    {
        float angle = i * 2 * M_PI / n;
        float x = e.x + e.rx * std::cos(angle);
        float y = e.y + e.ry * std::sin(angle);
        p.push_back(sf::Vector2f({x, y}));
    }
    if (e.rotation != 0.f)
        for (auto& pt : p)
            pt = rotatePoint(pt, {e.x, e.y}, e.rotation);
    return p;
}

Polygon rectangleToPolygon(RectangleData& r)
{
    Polygon p;
    p.push_back(sf::Vector2f{r.x - r.w, r.y - r.h});
    p.push_back(sf::Vector2f{r.x + r.w, r.y - r.h});
    p.push_back(sf::Vector2f{r.x + r.w, r.y + r.h});
    p.push_back(sf::Vector2f{r.x - r.w, r.y + r.h});
    if (r.rotation != 0.f)
        for (auto& pt : p)
            pt = rotatePoint(pt, {r.x, r.y}, r.rotation);
    return p;
}

bool overlapOnAxis(const Polygon& shapeA, const Polygon& shapeB, const sf::Vector2f& axis)
{
    auto project = [&](const Polygon& shape)
    {
        float min = (shape[0].x * axis.x + shape[0].y * axis.y);
        float max = min;
        for (auto& p : shape)
        {
            float proj = p.x * axis.x + p.y * axis.y;
            if (proj < min)
                min = proj;
            if (proj > max)
                max = proj;
        }
        return std::pair<float, float>(min, max);
    };

    auto [minA, maxA] = project(shapeA);
    auto [minB, maxB] = project(shapeB);

    return !(maxA < minB || maxB < minA);
}

bool polygonsCollide(const Polygon& a, const Polygon& b)
{
    auto addAxes = [](const Polygon& shape, std::vector<sf::Vector2f>& axes)
    {
        for (size_t i = 0; i < shape.size(); ++i)
        {
            sf::Vector2f p1 = shape[i];
            sf::Vector2f p2 = shape[(i + 1) % shape.size()];
            sf::Vector2f edge = p2 - p1;
            sf::Vector2f normal(-edge.y, edge.x);
            float len = std::sqrt(normal.x * normal.x + normal.y * normal.y);
            normal /= len;
            axes.push_back(normal);
        }
    };

    std::vector<sf::Vector2f> axes;
    addAxes(a, axes);
    addAxes(b, axes);

    for (auto& axis : axes)
    {
        if (!overlapOnAxis(a, b, axis))
            // Found a separating axis.
            return false;
    }
    return true;
}

sf::ConvexShape makeConvexShape(const Polygon& p)
{
    sf::ConvexShape shape;
    shape.setPointCount(p.size());
    for (size_t i = 0; i < p.size(); ++i)
        shape.setPoint(i, p[i]);

    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Green);
    shape.setOutlineThickness(1.f);
    return shape;
}

EllipseData makeEllipseData(const CEllipseCollider& e)
{
    auto transform = e.gameObject.getComponent<CTransform>();
    if (!transform)
        return EllipseData();
    EllipseData data;
    sf::Vector2f s = transform->getGlobalScale();
    data.x = e.offset.x + transform->getGlobalPosition().x;
    data.y = e.offset.y + transform->getGlobalPosition().y;
    data.rx = e.rx * s.x;
    data.ry = e.ry * s.y;
    data.rotation = transform->getDisplayRotation();
    return data;
}

RectangleData makeRectangleData(const CRectangleCollider& r)
{
    auto transform = r.gameObject.getComponent<CTransform>();
    if (!transform)
        return RectangleData();
    RectangleData data;
    sf::Vector2f s = transform->getGlobalScale();
    data.x = r.offset.x + transform->getGlobalPosition().x;
    data.y = r.offset.y + transform->getGlobalPosition().y;
    data.w = r.halfSize.x * s.x;
    data.h = r.halfSize.y * s.y;
    data.rotation = transform->getDisplayRotation();
    return data;
}

sf::Vector2f rotatePoint(const sf::Vector2f& p, const sf::Vector2f& center, float angleDeg)
{
    float angleRad = angleDeg * (std::numbers::pi / 180.f);
    float s = std::sin(angleRad);
    float c = std::cos(angleRad);

    sf::Vector2f translated = p - center;
    sf::Vector2f rotated(translated.x * c - translated.y * s, translated.x * s + translated.y * c);
    return rotated + center;
}

void CEllipseCollider::showBounds(sf::RenderWindow& window) const
{
    EllipseData data = makeEllipseData(*this);
    sf::ConvexShape shape = makeConvexShape(ellipseToPolygon(data));
    window.draw(shape);
}

bool CEllipseCollider::isTouching(const CCollider& other) const { return other.isTouchingWith(*this); }

bool CEllipseCollider::isTouchingWith(const CEllipseCollider& other) const
{
    EllipseData data1 = makeEllipseData(*this);
    EllipseData data2 = makeEllipseData(other);

    return polygonsCollide(ellipseToPolygon(data1), ellipseToPolygon(data2));
}

bool CEllipseCollider::isTouchingWith(const CRectangleCollider& other) const
{
    EllipseData data1 = makeEllipseData(*this);
    RectangleData data2 = makeRectangleData(other);

    return polygonsCollide(ellipseToPolygon(data1), rectangleToPolygon(data2));
}

void CRectangleCollider::showBounds(sf::RenderWindow& window) const
{
    RectangleData data = makeRectangleData(*this);
    sf::ConvexShape shape = makeConvexShape(rectangleToPolygon(data));
    window.draw(shape);
}

bool CRectangleCollider::isTouching(const CCollider& other) const { return other.isTouchingWith(*this); }

bool CRectangleCollider::isTouchingWith(const CEllipseCollider& other) const
{
    EllipseData data1 = makeEllipseData(other);
    RectangleData data2 = makeRectangleData(*this);

    return polygonsCollide(ellipseToPolygon(data1), rectangleToPolygon(data2));
}

bool CRectangleCollider::isTouchingWith(const CRectangleCollider& other) const
{
    RectangleData data1 = makeRectangleData(*this);
    RectangleData data2 = makeRectangleData(other);

    return polygonsCollide(rectangleToPolygon(data1), rectangleToPolygon(data2));
}
