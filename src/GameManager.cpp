#include "GameManager.hpp"

GameManager::GameManager() : GameObject() {}

void GameManager::updateBehavior(float dt, Input& input, GameObjectManager& entityManager)
{
    spawnTimer += dt;
    if (spawnTimer >= spawnInterval)
    {
        spawnTimer -= spawnInterval;

        // Random position within window bounds.
        float x = getRandomFloat(-540.f, 540.f);
        float y = -400.f;
        sf::Vector2f position = {x, y};

        // Random downward speed.
        sf::Vector2f speed = {getRandomFloat(-40.f, 40.f), getRandomFloat(200.f, 500.f)};

        // Create and add a new entity.
        auto asteroid = std::make_unique<Projectile>("assets/asteroids/asteroid.png", position, speed, 0.f, 2.f, 5);
        entityManager.addEntity(std::move(asteroid));
    }
}

float GameManager::getRandomFloat(float min, float max)
{
    return min + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}
