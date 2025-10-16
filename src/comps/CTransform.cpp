#include "CTransform.hpp"

CTransform::CTransform(GameObject& gameObject) : Component(gameObject) {}

void CTransform::setPosition(sf::Vector2f pos) { position = pos; }
sf::Vector2f CTransform::getLocalPosition() const { return position; }
void CTransform::move(sf::Vector2f offset) { position += offset; }

void CTransform::setRotation(float angle) { trueRotation = angle; }
float CTransform::getLocalRotation() const { return trueRotation; }

void CTransform::setRotationQuantization(float step) { rotationStep = step; }
float CTransform::getRotationQuantization() const { return rotationStep; }

void CTransform::setScale(sf::Vector2f factors) { scale = factors; }
sf::Vector2f CTransform::getLocalScale() const { return scale; }

// Get parent transform if it exists.
const CTransform* CTransform::getParentTransform() const
{
    if (gameObject.getParent())
        return gameObject.getParent()->getComponent<CTransform>();
    return nullptr;
}

sf::Vector2f CTransform::getGlobalPosition() const
{
    const CTransform* parentTransform = getParentTransform();
    if (!parentTransform)
        return position;

    // Apply parent's transform to local position.
    sf::Transform t;
    t.translate(parentTransform->getGlobalPosition());
    t.rotate(sf::degrees(parentTransform->getGlobalRotation()));
    t.scale(parentTransform->getGlobalScale());

    return t.transformPoint(position);
}

float CTransform::getGlobalRotation() const
{
    const CTransform* parentTransform = getParentTransform();
    if (!parentTransform)
        return trueRotation;
    return trueRotation + parentTransform->getGlobalRotation();
}

float CTransform::getDisplayRotation() const { return quantizeRotation(getGlobalRotation()); }

sf::Vector2f CTransform::getGlobalScale() const
{
    const CTransform* parentTransform = getParentTransform();
    if (!parentTransform)
        return scale;

    sf::Vector2f parentScale = parentTransform->getGlobalScale();
    return sf::Vector2f(scale.x * parentScale.x, scale.y * parentScale.y);
}

sf::Vector2f CTransform::getGlobalPoint(sf::Vector2f localPoint) const
{
    sf::Transform t;
    t.translate(position);
    t.rotate(sf::degrees(getDisplayRotation()));
    t.scale(scale);
    return t.transformPoint(localPoint);
}
