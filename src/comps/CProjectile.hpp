#pragma once

#include "../GameObject.hpp"
#include "CCollider.hpp"

// Component representing a projectile fired by the player.
class CProjectile : public Component
{
public:

    CProjectile(GameObject& gameObject) : Component(gameObject) {}

    // Check for collisions with asteroids and destroy both on hit.
    void update(Context& ctx) override
    {
        auto thisCollider = gameObject.getComponent<CCollider>();
        for (auto& obj : ctx.manager.getAll())
        {
            if (obj.get() == &gameObject)
                continue;

            auto otherCollider = obj->getComponent<CCollider>();
            if (otherCollider && thisCollider && thisCollider->isTouching(*otherCollider))
            {
                if (otherCollider->getLayer() == Layer::Asteroid && thisCollider->getLayer() == Layer::Projectile)
                {
                    std::cout << "Asteroid hit by projectile!\n";
                    obj->destroy();
                    gameObject.destroy();
                }
            }
        }
    }
};
