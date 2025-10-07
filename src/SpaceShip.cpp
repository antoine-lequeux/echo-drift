#include "SpaceShip.hpp"
#include <iostream>

SpaceShip::SpaceShip(std::string texturePath, float s) : speed(s), texture(texturePath), sprite(texture)
{
    sprite.setOrigin({sprite.getLocalBounds().size.x / 2, sprite.getLocalBounds().size.y / 2});
    sprite.setPosition({400.f, 300.f});
    sprite.setScale({3.f, 3.f});
}

void SpaceShip::update(float dt, Input& input)
{
    float distance = speed * dt;
    if (input.isActionActive(Action::MoveUp))
        sprite.move({0, -distance});
    if (input.isActionActive(Action::MoveDown))
        sprite.move({0, distance});
    if (input.isActionActive(Action::MoveLeft))
        sprite.move({-distance, 0});
    if (input.isActionActive(Action::MoveRight))
        sprite.move({distance, 0});

    if (input.isActionTriggered(Action::Shoot))
        std::cout << "Pew!" << "\n";
}

void SpaceShip::render(sf::RenderWindow& window) { window.draw(sprite); }