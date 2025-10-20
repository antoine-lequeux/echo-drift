#include "CSprite.hpp"

CSprite::CSprite(GameObject& gameObject) : Component(gameObject) {}

CSprite::CSprite(GameObject& gameObject, std::shared_ptr<sf::Texture> tex, int drawOrder)
    : Component(gameObject), texture(tex), drawOrder(drawOrder)
{
    textureRect = sf::IntRect({0, 0}, (sf::Vector2i)texture->getSize());
    texture->setSmooth(false);
}

void CSprite::update(Context& ctx) { render(ctx.window); }

void CSprite::setTexture(std::shared_ptr<sf::Texture> tex) { texture = tex; }

void CSprite::setTextureRect(sf::IntRect rect) { textureRect = rect; }

sf::IntRect CSprite::getTextureRect() const { return textureRect; }

void CSprite::setColor(const sf::Color& col) { color = col; }

sf::Color CSprite::getColor() const { return color; }

sf::Vector2f CSprite::getSize() const
{
    sf::Vector2f baseSize;

    // If no texture rect was specified...
    if (textureRect.size.x == 0 || textureRect.size.y == 0)
    {
        // ...we use the base texture size...
        baseSize.x = static_cast<float>(texture->getSize().x);
        baseSize.y = static_cast<float>(texture->getSize().y);
    }
    else
    {
        // ...else we use the size of the texture rect.
        baseSize.x = static_cast<float>(textureRect.size.x);
        baseSize.y = static_cast<float>(textureRect.size.y);
    }

    return baseSize;
}

sf::Vector2f CSprite::getWorldSize() const
{
    // Get the base size.
    sf::Vector2f size = getSize();

    // Get the transform data.
    auto transform = gameObject.getComponent<CTransform>();
    if (transform)
    {
        // Apply the scale.
        sf::Vector2f scale = transform->getGlobalScale();
        size.x *= scale.x;
        size.y *= scale.y;
    }

    return size;
}

void CSprite::render(sf::RenderWindow& window)
{
    auto transform = gameObject.getComponent<CTransform>();
    if (!transform)
        return;

    sf::Sprite sprite(*texture);
    sprite.setTextureRect(textureRect);
    sprite.setColor(color);

    // Apply transform data.
    sprite.setPosition(transform->getGlobalPosition());
    sprite.setRotation(sf::degrees(transform->getDisplayRotation()));
    sprite.setScale(transform->getGlobalScale());

    // Center origin.
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});

    window.draw(sprite);
}
