#include "CGameManager.hpp"
#include "CCollider.hpp"
#include "CDespawner.hpp"
#include "CSpeed.hpp"
#include "CSprite.hpp"

#include <array>

void CGameManager::update(Context& ctx)
{
    if (ctx.input.isActionTriggered(Action::Debug))
    {
        auto settings = ctx.manager.resources.get<GameSettings>(ResourceID::Settings);
        settings->showColliders = !settings->showColliders;
    }

    spawnAccumulator += ctx.dt;
    const f32 spawnInterval = 1.f / std::max(asteroidsPerSecond, 0.0001f);

    while (spawnAccumulator >= spawnInterval)
    {
        spawnAccumulator -= spawnInterval;

        // Random position within window bounds.
        f32 x = utils::getRandom(-540.f, 540.f);
        f32 y = -400.f;
        sf::Vector2f position = {x, y};

        // Random downward speed.
        sf::Vector2f speed = {utils::getRandom(-40.f, 40.f), utils::getRandom(200.f, 500.f)};

        // Create the asteroid entity.
        auto& asteroid = ctx.manager.spawn();

        auto& transform = asteroid.addComponent<CTransform>();
        transform.setPosition(position);
        transform.setRotation(utils::getRandom(0.f, 360.f));

        static constexpr std::array<ResourceID, 4> asteroidTypes = {ResourceID::Asteroid1, ResourceID::Asteroid2,
                                                                    ResourceID::Asteroid3, ResourceID::Asteroid4};

        // Create a Sprite component with an asteroid texture chosen randomly among four.
        auto& sprite = asteroid.addComponent<CSprite>(
            ctx.manager.resources.get<sf::Texture>(asteroidTypes[utils::getRandom(0, 3)]), 5);

        auto& col = asteroid.addComponent<CEllipseCollider>(Layer::Asteroid);
        col.rx = sprite.getWorldSize().x / 2.f;
        col.ry = sprite.getWorldSize().x / 2.f;

        asteroid.addComponent<CSpeed>(speed);

        asteroid.addComponent<CDespawner>();
    }
}