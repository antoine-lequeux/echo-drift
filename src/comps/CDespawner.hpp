#pragma once

#include "../GameObject.hpp"
#include "CSprite.hpp"

// Component that despawns the entity when it goes too far out of the view bounds.
class CDespawner : public Component
{
public:

    CDespawner(GameObject& gameObject) : Component(gameObject) {}

    void update(Context& ctx) override
    {
        sf::Vector2f position = gameObject.getComponent<CSprite>()->getPosition();
        sf::View view = ctx.window.getView();

        sf::Vector2f viewCenter = view.getCenter();

        // Calculate a maximum distance based on the view size.
        float maxDistance = std::max(view.getSize().x, view.getSize().y) * 0.6f;

        // Despawn if the entity is too far from the view center.
        if (std::hypot(position.x - viewCenter.x, position.y - viewCenter.y) > maxDistance)
        {
            gameObject.destroy();
        }
    }
};