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
        std::cout << "Spawning new asteroid\n";
        spawnTimer -= spawnInterval;

        // Random position within window bounds.
        float x = getRandomFloat(-540.f, 540.f);
        float y = -400.f;
        sf::Vector2f position = {x, y};

        // Random downward speed.
        sf::Vector2f speed = {getRandomFloat(-40.f, 40.f), getRandomFloat(200.f, 500.f)};

        // Create and add a new entity.
        auto asteroid = std::make_unique<GameObject>();

        auto& transform = asteroid->addComponent<CTransform>();
        transform.setPosition(position);
        transform.setScale({2.f, 2.f});
        transform.setRotation(getRandomFloat(0.f, 360.f));

        auto& sprite = asteroid->addComponent<CSprite>("assets/asteroids/asteroid.png", 5);

        auto& speedComp = asteroid->addComponent<CSpeed>(speed);
        speedComp.setSpeed(speed);
        asteroid->addComponent<CDespawner>();

        asteroid->addComponent<CCollider>(Layer::Asteroid);

        ctx.manager.spawn(std::move(asteroid));
    }
}

float CGameManager::getRandomFloat(float min, float max)
{
    return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}