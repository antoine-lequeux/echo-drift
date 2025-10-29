#pragma once

#include "../Animation.hpp"
#include "CSprite.hpp"

// Component representing a spritesheet containing multiple sprites.
// Switching sprites is made easy, simply using an index.
class CMultiSprite : public CSprite
{
public:

    CMultiSprite(GameObject& gameObject, std::shared_ptr<sf::Texture> tex, std::size_t frameCount, int drawOrder = 0)
        : CSprite(gameObject, tex, drawOrder)
    {
        int frameWidth = tex->getSize().x / frameCount;
        int frameHeight = tex->getSize().y;

        // Fill the vector of rects automatically, using the frame count and the texture dimensions.
        for (int i = 0; i < frameCount; i++)
        {
            rects.push_back(sf::IntRect{{frameWidth * i, 0}, {frameWidth, frameHeight}});
        }
        setTextureRect(rects[0]);
    }

    void setCurrentSpriteIndex(std::size_t index)
    {
        currentSpriteIndex = index;
        setTextureRect(rects[index]);
    }
    std::size_t getCurrentSpriteIndex() const { return currentSpriteIndex; }

private:

    std::size_t currentSpriteIndex = 0;
    std::vector<sf::IntRect> rects;
};