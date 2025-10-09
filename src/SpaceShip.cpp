#include "SpaceShip.hpp"
#include <iostream>

SpaceShip::SpaceShip(std::string texturePath) : Entity(texturePath)
{
    setPosition({400.f, 300.f});
    setScale({3.f, 3.f});
}

void SpaceShip::updateBehavior(float dt, Input& input)
{
    float distance = moveSpeed * dt;
    sf::Vector2f movement{0.f, 0.f};

    if (input.isActionActive(Action::MoveUp))
        movement.y -= distance;
    if (input.isActionActive(Action::MoveDown))
        movement.y += distance;
    if (input.isActionActive(Action::MoveLeft))
        movement.x -= distance;
    if (input.isActionActive(Action::MoveRight))
        movement.x += distance;

    if (movement.length() > 0)
    {
        movement = movement.normalized() * distance;
        move(movement);
    }

    if (input.isActionTriggered(Action::Shoot))
        std::cout << "Pew!" << "\n";
}