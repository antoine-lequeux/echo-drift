#pragma once

#include "../GameObject.hpp"
#include "CTransform.hpp"

// Component that stores a texture and additional members to manage the rendering of a sprite.
class CSprite : public Component
{
public:

    CSprite(GameObject& gameObject);
    CSprite(GameObject& gameObject, std::shared_ptr<sf::Texture> tex, i32 drawOrder = 0);

    void update(Context& ctx) override;

    void setTexture(std::shared_ptr<sf::Texture> tex);

    void setTextureRect(sf::IntRect rect);
    sf::IntRect getTextureRect() const;

    void setColor(const sf::Color& color);
    sf::Color getColor() const;

    void setDrawOrder(i32 order);
    i32 getDrawOrder() const { return drawOrder; }

    void setInversionX(bool state);
    void setInversionY(bool state);

    sf::Vector2f getSize() const;
    sf::Vector2f getWorldSize() const;

protected:

    void render(sf::RenderWindow& window);

    // Mark the cached sprite as needing a rebuild on next render.
    void markDirty() { spriteDirty = true; }

    std::shared_ptr<sf::Texture> texture;
    sf::IntRect textureRect;
    sf::Color color = sf::Color::White;
    i32 drawOrder = 0;
    bool invertedX = false;
    bool invertedY = false;

private:

    std::optional<sf::Sprite> cachedSprite;
    bool spriteDirty = true;

    void rebuildSprite();
};