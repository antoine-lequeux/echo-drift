#include "CTransform.hpp"

CTransform::CTransform(GameObject& gameObject) : Component(gameObject) {}

void CTransform::setPosition(sf::Vector2f pos) { position = pos; }
sf::Vector2f CTransform::getPosition() const { return position; }

void CTransform::move(sf::Vector2f offset) { position += offset; }

void CTransform::setRotation(float angle) { trueRotation = angle; }

float CTransform::getRotation() const { return trueRotation; }
float CTransform::getDisplayRotation() const { return quantizeRotation(trueRotation); }

void CTransform::setRotationQuantization(float step) { rotationStep = step; }
float CTransform::getRotationQuantization() const { return rotationStep; }

void CTransform::setScale(sf::Vector2f factors) { scale = factors; }
sf::Vector2f CTransform::getScale() const { return scale; }

sf::Vector2f CTransform::getLocalPoint(sf::Vector2f localPoint) const
{
    // Transform a local point using position, rotation, and scale.
    sf::Transform t;
    t.translate(position);
    t.rotate(sf::degrees(getDisplayRotation()));
    t.scale(scale);
    return t.transformPoint(localPoint);
}
