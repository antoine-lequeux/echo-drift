#include "CCollider.hpp"

// TODO: use polygon approximation based detection, to improve precision.

bool checkCollision(const EllipseData& a, const EllipseData& b)
{
    // Approximate each ellipse as a circle with radius = average of rx and ry.
    float ra = (a.rx + a.ry) * 0.5f;
    float rb = (b.rx + b.ry) * 0.5f;

    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dist2 = dx * dx + dy * dy;
    float radiusSum = ra + rb;

    return dist2 <= radiusSum * radiusSum;
}

bool checkCollision(const RectangleData& a, const RectangleData& b)
{
    return (std::abs(a.x - b.x) <= (a.w + b.w)) && (std::abs(a.y - b.y) <= (a.h + b.h));
}

bool checkCollision(const EllipseData& e, const RectangleData& r)
{
    // Find closest point on rectangle to ellipse center.
    float closestX = std::clamp(e.x, r.x - r.w, r.x + r.w);
    float closestY = std::clamp(e.y, r.y - r.h, r.y + r.h);

    // Transform this into the ellipse’s local space.
    float dx = (closestX - e.x) / e.rx;
    float dy = (closestY - e.y) / e.ry;

    // Inside ellipse if normalized distance <= 1.
    return (dx * dx + dy * dy) <= 1.0f;
}

void CEllipseCollider::showBounds(sf::RenderWindow& window) const
{
    auto transform = gameObject.getComponent<CTransform>();
    if (!transform)
        return;

    float ratio;
    bool rxBigger = false;
    if (rx > ry)
    {
        rxBigger = true;
        ratio = rx / ry;
    }
    else
    {
        ratio = ry / rx;
    }
    sf::CircleShape ellipse(rxBigger ? rx : ry);

    ellipse.setScale(
        sf::Vector2f{rxBigger ? 1.f : ratio, rxBigger ? ratio : 1.f}.componentWiseMul(transform->getGlobalScale()));
    sf::Vector2f pos;
    pos.x = transform->getGlobalPosition().x + offset.x - rx * transform->getGlobalScale().x;
    pos.y = transform->getGlobalPosition().y + offset.y - ry * transform->getGlobalScale().y;
    ellipse.setPosition(pos);
    ellipse.setFillColor(sf::Color::Transparent);
    ellipse.setOutlineColor(sf::Color::Green);
    ellipse.setOutlineThickness(1.f);

    window.draw(ellipse);
}

bool CEllipseCollider::isTouching(const CCollider& other) const
{
    return other.isTouchingWith(*this);
}

bool CEllipseCollider::isTouchingWith(const CEllipseCollider& other) const
{
    auto transform1 = gameObject.getComponent<CTransform>();
    if (!transform1)
        return false;
    EllipseData data1;
    sf::Vector2f s1 = transform1->getGlobalScale();
    data1.x = offset.x + transform1->getGlobalPosition().x;
    data1.y = offset.y + transform1->getGlobalPosition().y;
    data1.rx = rx * s1.x;
    data1.ry = ry * s1.y;

    auto transform2 = other.gameObject.getComponent<CTransform>();
    if (!transform2)
        return false;
    EllipseData data2;
    sf::Vector2f s2 = transform2->getGlobalScale();
    data2.x = other.offset.x + transform2->getGlobalPosition().x;
    data2.y = other.offset.y + transform2->getGlobalPosition().y;
    data2.rx = other.rx * s2.x;
    data2.ry = other.ry * s2.y;

    return checkCollision(data1, data2);
}

bool CEllipseCollider::isTouchingWith(const CRectangleCollider& other) const
{
    auto transform1 = gameObject.getComponent<CTransform>();
    if (!transform1)
        return false;
    EllipseData data1;
    sf::Vector2f s1 = transform1->getGlobalScale();
    data1.x = offset.x + transform1->getGlobalPosition().x;
    data1.y = offset.y + transform1->getGlobalPosition().y;
    data1.rx = rx * s1.x;
    data1.ry = ry * s1.y;

    auto transform2 = other.gameObject.getComponent<CTransform>();
    if (!transform2)
        return false;
    RectangleData data2;
    sf::Vector2f s2 = transform2->getGlobalScale();
    data2.x = other.offset.x + transform2->getGlobalPosition().x;
    data2.y = other.offset.y + transform2->getGlobalPosition().y;
    data2.w = other.halfSize.x * s2.x;
    data2.h = other.halfSize.y * s2.y;

    return checkCollision(data1, data2);
}

void CRectangleCollider::showBounds(sf::RenderWindow& window) const
{
    auto transform = gameObject.getComponent<CTransform>();
    if (!transform)
        return;

    sf::RectangleShape rect;
    rect.setPosition(transform->getGlobalPosition() + offset - halfSize.componentWiseMul(transform->getGlobalScale()));
    rect.setSize((halfSize * 2.f).componentWiseMul(transform->getGlobalScale()));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(1.f);

    window.draw(rect);
}

bool CRectangleCollider::isTouching(const CCollider& other) const
{
    return other.isTouchingWith(*this);
}

bool CRectangleCollider::isTouchingWith(const CEllipseCollider& other) const
{
    auto transform1 = other.gameObject.getComponent<CTransform>();
    if (!transform1)
        return false;
    EllipseData data1;
    sf::Vector2f s1 = transform1->getGlobalScale();
    data1.x = other.offset.x + transform1->getGlobalPosition().x;
    data1.y = other.offset.y + transform1->getGlobalPosition().y;
    data1.rx = other.rx * s1.x;
    data1.ry = other.ry * s1.y;

    auto transform2 = gameObject.getComponent<CTransform>();
    if (!transform2)
        return false;
    RectangleData data2;
    sf::Vector2f s2 = transform2->getGlobalScale();
    data2.x = offset.x + transform2->getGlobalPosition().x;
    data2.y = offset.y + transform2->getGlobalPosition().y;
    data2.w = halfSize.x * s2.x;
    data2.h = halfSize.y * s2.y;

    return checkCollision(data1, data2);
}

bool CRectangleCollider::isTouchingWith(const CRectangleCollider& other) const
{
    auto transform1 = gameObject.getComponent<CTransform>();
    if (!transform1)
        return false;
    RectangleData data1;
    sf::Vector2f s1 = transform1->getGlobalScale();
    data1.x = offset.x + transform1->getGlobalPosition().x;
    data1.y = offset.y + transform1->getGlobalPosition().y;
    data1.w = halfSize.x * s1.x;
    data1.h = halfSize.y * s1.y;

    auto transform2 = other.gameObject.getComponent<CTransform>();
    if (!transform2)
        return false;
    RectangleData data2;
    sf::Vector2f s2 = transform2->getGlobalScale();
    data2.x = other.offset.x + transform2->getGlobalPosition().x;
    data2.y = other.offset.y + transform2->getGlobalPosition().y;
    data2.w = other.halfSize.x * s2.x;
    data2.h = other.halfSize.y * s2.y;

    return checkCollision(data1, data2);
}
