#include "CGameManager.hpp"
#include "CCollider.hpp"
#include "CDespawner.hpp"
#include "CSpeed.hpp"
#include "CSprite.hpp"

void CGameManager::update(Context& ctx)
{
    spawnTimer += ctx.dt;
    if (spawnTimer >= spawnInterval)
    {
        spawnTimer -= spawnInterval;

        // Random position within window bounds.
        float x = getRandomFloat(-540.f, 540.f);
        float y = -400.f;
        sf::Vector2f position = {x, y};

        // Random downward speed.
        sf::Vector2f speed = {getRandomFloat(-40.f, 40.f), getRandomFloat(200.f, 500.f)};

        // Create the flame entity.
        auto& flame = ctx.manager.spawn();

        auto& transform = flame.addComponent<CTransform>();
        transform.setPosition(position);
        transform.setScale({-3.f, 3.f});
        transform.setRotation(getAngleDegrees(speed));

        auto& speedComp = flame.addComponent<CSpeed>(speed);
        speedComp.setSpeed(speed);

        flame.addComponent<CDespawner>();

        flame.addComponent<CSprite>("assets/effects/flame01.png", 4);

        flame.addComponent<CCollider>(Layer::Asteroid);

        // Create the asteroid entity.
        auto& asteroid = ctx.manager.spawn();

        auto& transform1 = asteroid.addComponent<CTransform>();
        transform1.setPosition({-10.f, 0.f});
        transform1.setRotation(getRandomFloat(0.f, 360.f));
        transform1.setScale({0.66f, 0.66f});

        asteroid.addComponent<CSprite>("assets/asteroids/asteroid.png", 5);

        // Add the asteroid as a child of the flame.
        flame.addChild(asteroid);
    }
}

float CGameManager::getRandomFloat(float min, float max)
{
    return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}