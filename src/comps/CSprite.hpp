#pragma once

#include "../GameObject.hpp"
#include "CTransform.hpp"

// Component that stores a texture and additional members to manage the rendering of a sprite.
class CSprite : public Component
{
public:

    CSprite(GameObject& gameObject, std::shared_ptr<sf::Texture> tex, int drawOrder = 0);

    void update(Context& ctx) override;

    void setTextureRect(const sf::IntRect& rect);
    void setColor(const sf::Color& color);

    void setDrawOrder(int order) { drawOrder = order; }
    int getDrawOrder() const { return drawOrder; }

    sf::Vector2f getSize() const;
    sf::Vector2f getWorldSize() const;

private:

    std::shared_ptr<sf::Texture> texture;
    sf::IntRect textureRect;
    sf::Color color = sf::Color::White;
    int drawOrder;
};