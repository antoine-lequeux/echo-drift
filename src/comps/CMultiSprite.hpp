#pragma once

#include "../Animation.hpp"
#include "CSprite.hpp"

// Component representing a spritesheet containing multiple sprites.
// Switching sprites is made easy, simply using an index.
class CMultiSprite : public CSprite
{
public:

    CMultiSprite(GameObject& gameObject, std::shared_ptr<sf::Texture> tex, usize frameCount, i32 drawOrder = 0)
        : CSprite(gameObject, tex, drawOrder)
    {
        i32 frameWidth = tex->getSize().x / frameCount;
        i32 frameHeight = tex->getSize().y;

        // Fill the vector of rects automatically, using the frame count and the texture dimensions.
        for (i32 i = 0; i < frameCount; i++)
        {
            rects.push_back(sf::IntRect{{frameWidth * i, 0}, {frameWidth, frameHeight}});
        }
        setTextureRect(rects[0]);
    }

    void setCurrentSpriteIndex(usize index)
    {
        currentSpriteIndex = index;
        setTextureRect(rects[index]);
    }
    usize getCurrentSpriteIndex() const { return currentSpriteIndex; }

private:

    usize currentSpriteIndex = 0;
    std::vector<sf::IntRect> rects;
};