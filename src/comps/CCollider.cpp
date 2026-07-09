#include "CCollider.hpp"

#include <array>

namespace
{

sf::Vector2f rotatePoint(const sf::Vector2f& p, const sf::Vector2f& center, f32 angleDeg)
{
    f32 angleRad = angleDeg * PI / 180.f;
    f32 s = std::sin(angleRad);
    f32 c = std::cos(angleRad);
    sf::Vector2f t = p - center;
    return {t.x * c - t.y * s + center.x, t.x * s + t.y * c + center.y};
}

bool overlapOnAxis(const Polygon& a, const Polygon& b, const sf::Vector2f& axis)
{
    f32 minA = a[0].x * axis.x + a[0].y * axis.y;
    f32 maxA = minA;
    for (auto& p : a)
    {
        f32 proj = p.x * axis.x + p.y * axis.y;
        minA = std::min(minA, proj);
        maxA = std::max(maxA, proj);
    }
    f32 minB = b[0].x * axis.x + b[0].y * axis.y;
    f32 maxB = minB;
    for (auto& p : b)
    {
        f32 proj = p.x * axis.x + p.y * axis.y;
        minB = std::min(minB, proj);
        maxB = std::max(maxB, proj);
    }
    return !(maxA < minB || maxB < minA);
}

bool polygonsCollide(const Polygon& a, const Polygon& b)
{
    std::array<sf::Vector2f, 64> axes{};
    usize axisCount = 0;

    auto addAxes = [&axes, &axisCount](const Polygon& shape)
    {
        for (usize i = 0; i < shape.size(); ++i)
        {
            sf::Vector2f edge = shape[(i + 1) % shape.size()] - shape[i];
            sf::Vector2f normal(-edge.y, edge.x);
            const f32 length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
            if (axisCount < axes.size())
                axes[axisCount++] = normal / length;
        }
    };

    addAxes(a);
    addAxes(b);

    for (usize i = 0; i < axisCount; ++i)
        if (!overlapOnAxis(a, b, axes[i]))
            return false;

    return true;
}

sf::ConvexShape makeConvexShape(const Polygon& p)
{
    sf::ConvexShape shape;
    shape.setPointCount(p.size());
    for (usize i = 0; i < p.size(); ++i)
        shape.setPoint(i, p[i]);
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Green);
    shape.setOutlineThickness(1.f);
    return shape;
}

} // namespace

void CCollider::update(Context& ctx)
{
    rebuildPolygon();
    if (ctx.manager.resources.get<GameSettings>(ResourceID::Settings)->showColliders)
        showBounds(ctx.window);
}

bool CCollider::isTouching(const CCollider& other) const
{
    if (cachedPolygon.empty() || other.cachedPolygon.empty())
        return false;
    return polygonsCollide(cachedPolygon, other.cachedPolygon);
}

void CEllipseCollider::rebuildPolygon()
{
    auto* transform = gameObject.getComponent<CTransform>();
    if (!transform)
    {
        cachedPolygon.clear();
        return;
    }

    constexpr u32 N = 32;
    cachedPolygon.resize(N);

    sf::Vector2f s = transform->getGlobalScale();
    f32 cx = offset.x + transform->getGlobalPosition().x;
    f32 cy = offset.y + transform->getGlobalPosition().y;
    f32 scaledRx = rx * s.x;
    f32 scaledRy = ry * s.y;
    f32 rot = transform->getDisplayRotation();

    for (u32 i = 0; i < N; ++i)
    {
        f32 angle = i * 2.f * PI / N;
        cachedPolygon[i] = {cx + scaledRx * std::cos(angle), cy + scaledRy * std::sin(angle)};
    }

    if (rot != 0.f)
        for (auto& pt : cachedPolygon)
            pt = rotatePoint(pt, {cx, cy}, rot);
}

void CEllipseCollider::showBounds(sf::RenderWindow& window) const
{
    if (!cachedPolygon.empty())
        window.draw(makeConvexShape(cachedPolygon));
}

void CRectangleCollider::rebuildPolygon()
{
    auto* transform = gameObject.getComponent<CTransform>();
    if (!transform)
    {
        cachedPolygon.clear();
        return;
    }

    cachedPolygon.resize(4);

    sf::Vector2f s = transform->getGlobalScale();
    f32 cx = offset.x + transform->getGlobalPosition().x;
    f32 cy = offset.y + transform->getGlobalPosition().y;
    f32 hw = halfSize.x * s.x;
    f32 hh = halfSize.y * s.y;
    f32 rot = transform->getDisplayRotation();

    cachedPolygon[0] = {cx - hw, cy - hh};
    cachedPolygon[1] = {cx + hw, cy - hh};
    cachedPolygon[2] = {cx + hw, cy + hh};
    cachedPolygon[3] = {cx - hw, cy + hh};

    if (rot != 0.f)
        for (auto& pt : cachedPolygon)
            pt = rotatePoint(pt, {cx, cy}, rot);
}

void CRectangleCollider::showBounds(sf::RenderWindow& window) const
{
    if (!cachedPolygon.empty())
        window.draw(makeConvexShape(cachedPolygon));
}
