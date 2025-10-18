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
        float x = Tools::getRandom(-540.f, 540.f);
        float y = -400.f;
        sf::Vector2f position = {x, y};

        // Random downward speed.
        sf::Vector2f speed = {Tools::getRandom(-40.f, 40.f), Tools::getRandom(200.f, 500.f)};

        // Create the asteroid entity.
        auto& asteroid = ctx.manager.spawn();

        auto& transform = asteroid.addComponent<CTransform>();
        transform.setPosition(position);
        transform.setRotation(Tools::getRandom(0.f, 360.f));

        static std::string root = "assets/asteroids/";
        static std::string types[4] = {"Asteroid 01_png_processed.png", "Asteroid 02_png_processed.png",
                                       "Asteroid 03_png_processed.png", "Asteroid 04_png_processed.png"};

        auto& sprite = asteroid.addComponent<CSprite>(root + types[Tools::getRandom(0, 3)], 5);

        auto& col = asteroid.addComponent<CEllipseCollider>(Layer::Asteroid);
        col.rx = sprite.getWorldSize().x / 2.f;
        col.ry = sprite.getWorldSize().x / 2.f;

        auto& speedComp = asteroid.addComponent<CSpeed>(speed);
        speedComp.setSpeed(speed);

        asteroid.addComponent<CDespawner>();
    }
}