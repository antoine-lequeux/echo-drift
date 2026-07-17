#include "CTransform.hpp"

CTransform::CTransform(GameObject& gameObject) : Component(gameObject) {}

void CTransform::setPosition(sf::Vector2f pos)
{
    position = pos;
    markDirty();
}

sf::Vector2f CTransform::getLocalPosition() const { return position; }

void CTransform::move(sf::Vector2f offset)
{
    position += offset;
    markDirty();
}

void CTransform::setRotation(f32 angle)
{
    trueRotation = angle;
    markDirty();
}

f32 CTransform::getLocalRotation() const { return trueRotation; }

void CTransform::setRotationQuantization(f32 step) { rotationStep = step; }

f32 CTransform::getRotationQuantization() const { return rotationStep; }

void CTransform::setScale(sf::Vector2f factors)
{
    scale = factors;
    markDirty();
}

sf::Vector2f CTransform::getLocalScale() const { return scale; }

// Get parent transform if it exists.
const CTransform* CTransform::getParentTransform() const
{
    if (gameObject.getParent()) return gameObject.getParent()->getComponent<CTransform>();
    return nullptr;
}

void CTransform::recomputeGlobal() const
{
    const CTransform* parentTransform = getParentTransform();
    if (!parentTransform)
    {
        cachedGlobalPosition = position;
        cachedGlobalRotation = trueRotation;
        cachedGlobalScale = scale;
    }
    else
    {
        sf::Transform t;
        t.translate(parentTransform->getGlobalPosition());
        t.rotate(sf::degrees(parentTransform->getGlobalRotation()));
        t.scale(parentTransform->getGlobalScale());

        cachedGlobalPosition = t.transformPoint(position);
        cachedGlobalRotation = trueRotation + parentTransform->getGlobalRotation();
        cachedGlobalScale = sf::Vector2f(scale.x * parentTransform->getGlobalScale().x, scale.y * parentTransform->getGlobalScale().y);
    }

    cachedDisplayRotation = quantizeRotation(cachedGlobalRotation);
    transformDirty = false;
}

sf::Vector2f CTransform::getGlobalPosition() const
{
    if (transformDirty) recomputeGlobal();
    return cachedGlobalPosition;
}

f32 CTransform::getGlobalRotation() const
{
    if (transformDirty) recomputeGlobal();
    return cachedGlobalRotation;
}

f32 CTransform::getDisplayRotation() const
{
    if (transformDirty) recomputeGlobal();
    return cachedDisplayRotation;
}

sf::Vector2f CTransform::getGlobalScale() const
{
    if (transformDirty) recomputeGlobal();
    return cachedGlobalScale;
}

sf::Vector2f CTransform::getGlobalPoint(sf::Vector2f localPoint) const
{
    sf::Transform t;
    t.translate(getGlobalPosition());
    t.rotate(sf::degrees(getGlobalRotation()));
    t.scale(getGlobalScale());
    return t.transformPoint(localPoint);
}
