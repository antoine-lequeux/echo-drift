#pragma once

#include "../GameObject.hpp"
#include "CSprite.hpp"

// Component that manages the entity's speed and movement.
class CSpeed : public Component
{
public:

    CSpeed(GameObject& gameObject, sf::Vector2f initialSpeed) : Component(gameObject), speed(initialSpeed) {}

    void update(Context& ctx) override { gameObject.getComponent<CSprite>()->move(speed * ctx.dt); }

    void setSpeed(sf::Vector2f newSpeed) { speed = newSpeed; }
    sf::Vector2f getSpeed() const { return speed; }

private:

    sf::Vector2f speed{0.f, 0.f};
};