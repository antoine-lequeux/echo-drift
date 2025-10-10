#include "CSprite.hpp"

CSprite::CSprite(GameObject& gameObject, std::string texturePath, int drawOrder)
    : Component(gameObject), texture(texturePath), sprite(texture), drawOrder(drawOrder)
{
    // Center the origin for easier transformations.
    sprite.setOrigin({sprite.getLocalBounds().size.x / 2.f, sprite.getLocalBounds().size.y / 2.f});
}

void CSprite::update(Context& ctx) { ctx.window.draw(sprite); }

void CSprite::setPosition(sf::Vector2f position) { sprite.setPosition(position); }

sf::Vector2f CSprite::getPosition() const { return sprite.getPosition(); }

sf::Vector2f CSprite::getLocalPoint(sf::Vector2f localPoint) const
{
    // Adjust for scaling.
    sf::Vector2f scale = sprite.getScale();
    localPoint.x /= scale.x;
    localPoint.y /= scale.y;

    // Calculate the sprite's half size.
    sf::Vector2f halfSize = {sprite.getLocalBounds().size.x * 0.5f, sprite.getLocalBounds().size.y * 0.5f};

    // Center the local point relative to sprite's origin.
    localPoint = {localPoint.x + halfSize.x, localPoint.y + halfSize.y};

    // Get the sprite's transformation.
    sf::Transform transform = sprite.getTransform();

    // Transform the local point to world space.
    return transform.transformPoint(localPoint);
}

void CSprite::move(sf::Vector2f offset) { sprite.move(offset); }

void CSprite::setRotation(float angle)
{
    // Store true rotation for physics/gameplay.
    trueRotation = angle;
    // Display quantized rotation.
    sprite.setRotation(sf::degrees(quantizeRotation(angle)));
}

float CSprite::getRotation() const { return trueRotation; }

float CSprite::getDisplayRotation() const { return sprite.getRotation().asDegrees(); }

void CSprite::setRotationQuantization(float step) { rotationStep = step; }

float CSprite::getRotationQuantization() const { return rotationStep; }

void CSprite::setScale(sf::Vector2f factors) { sprite.setScale(factors); }

sf::Vector2f CSprite::getScale() const { return sprite.getScale(); }

sf::Vector2f CSprite::getSize() const
{
    return {sprite.getLocalBounds().size.x * getScale().x, sprite.getLocalBounds().size.y * getScale().y};
}