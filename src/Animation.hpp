#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct Animation
{
    std::shared_ptr<sf::Texture> texture;
    std::vector<sf::IntRect> frames;
    float frameTime = 0.2f;
    bool loop = true;
};