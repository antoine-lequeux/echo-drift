#include "CSprite.hpp"

CSprite::CSprite(GameObject& gameObject) : Component(gameObject) {}

CSprite::CSprite(GameObject& gameObject, std::shared_ptr<sf::Texture> tex, i32 drawOrder)
    : Component(gameObject), texture(tex), drawOrder(drawOrder)
{
    textureRect = sf::IntRect({0, 0}, (sf::Vector2i)texture->getSize());
    texture->setSmooth(false);
}

void CSprite::update(Context& ctx) { render(ctx.window); }

void CSprite::setTexture(std::shared_ptr<sf::Texture> tex)
{
    texture = tex;
    markDirty();
}

void CSprite::setTextureRect(sf::IntRect rect)
{
    textureRect = rect;
    markDirty();
}

sf::IntRect CSprite::getTextureRect() const { return textureRect; }

void CSprite::setColor(const sf::Color& col)
{
    color = col;
    markDirty();
}

sf::Color CSprite::getColor() const { return color; }

void CSprite::setInversionX(bool state)
{
    if (invertedX != state)
    {
        invertedX = state;
        markDirty();
    }
}

void CSprite::setInversionY(bool state)
{
    if (invertedY != state)
    {
        invertedY = state;
        markDirty();
    }
}

sf::Vector2f CSprite::getSize() const
{
    if (!texture)
        return {0.f, 0.f};

    if (textureRect.size.x == 0 || textureRect.size.y == 0)
        return {static_cast<f32>(texture->getSize().x), static_cast<f32>(texture->getSize().y)};

    return {static_cast<f32>(textureRect.size.x), static_cast<f32>(textureRect.size.y)};
}

sf::Vector2f CSprite::getWorldSize() const
{
    sf::Vector2f size = getSize();
    auto* transform = gameObject.getComponent<CTransform>();
    if (transform)
    {
        sf::Vector2f scale = transform->getGlobalScale();
        size.x *= scale.x;
        size.y *= scale.y;
    }
    return size;
}

void CSprite::rebuildSprite()
{
    if (!texture)
        return;
    cachedSprite.emplace(*texture);
    cachedSprite->setTextureRect(textureRect);
    cachedSprite->setColor(color);
    sf::FloatRect bounds = cachedSprite->getLocalBounds();
    cachedSprite->setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    spriteDirty = false;
}

void CSprite::render(sf::RenderWindow& window)
{
    if (!texture)
        return;

    auto* transform = gameObject.getComponent<CTransform>();
    if (!transform)
        return;

    if (spriteDirty)
        rebuildSprite();

    if (!cachedSprite)
        return;

    cachedSprite->setPosition(transform->getGlobalPosition());
    cachedSprite->setRotation(sf::degrees(transform->getDisplayRotation()));
    cachedSprite->setScale({transform->getGlobalScale().x * (invertedX ? -1.f : 1.f),
                            transform->getGlobalScale().y * (invertedY ? -1.f : 1.f)});

    window.draw(*cachedSprite);
}
