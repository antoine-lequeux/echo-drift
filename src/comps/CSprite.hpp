#pragma once

#include "../GameObject.hpp"
#include "CTransform.hpp"

// Component that stores a texture and additional members to manage the rendering of a sprite.
class CSprite : public Component
{
public:

    CSprite(GameObject& gameObject);
    CSprite(GameObject& gameObject, std::shared_ptr<sf::Texture> tex, int drawOrder = 0);

    void update(Context& ctx) override;

    void setTexture(std::shared_ptr<sf::Texture> tex);

    void setTextureRect(sf::IntRect rect);
    sf::IntRect getTextureRect() const;

    void setColor(const sf::Color& color);
    sf::Color getColor() const;

    void setDrawOrder(int order) { drawOrder = order; }
    int getDrawOrder() const { return drawOrder; }

    sf::Vector2f getSize() const;
    sf::Vector2f getWorldSize() const;

protected:

    void render(sf::RenderWindow& window);

    std::shared_ptr<sf::Texture> texture;
    sf::IntRect textureRect;
    sf::Color color = sf::Color::White;
    int drawOrder;
};