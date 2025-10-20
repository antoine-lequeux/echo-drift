#include "Application.hpp"
#include "Animation.hpp"
#include "comps/CCollider.hpp"
#include "comps/CGameManager.hpp"
#include "comps/CSpaceShip.hpp"
#include "comps/CSpeed.hpp"
#include "comps/CSprite.hpp"
#include "comps/CTransform.hpp"
#include "comps/CAnimatedSprite.hpp"

Application::Application() : window(sf::VideoMode({1280, 720}), "Echo Drift"), entityManager(resourceManager)
{
    window.setFramerateLimit(60);
    gameView = sf::View({0.f, 0.f}, {1280.f, 720.f});
    gameView.zoom(0.75f);
    window.setView(gameView);
}

void Application::run()
{
    setup();
    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        input.clear();
    }
}

void Application::setup()
{
    resourceManager.add<sf::Texture>("player_blue_1", "assets/player/player_blue_frame01.png");

    resourceManager.add<sf::Texture>("background", "assets/backgrounds/background_vertical.png");

    resourceManager.add<sf::Texture>("projectile", "assets/projectiles/laser_medium.png");

    resourceManager.add<sf::Texture>("asteroid1", "assets/asteroids/asteroid01.png");
    resourceManager.add<sf::Texture>("asteroid2", "assets/asteroids/asteroid02.png");
    resourceManager.add<sf::Texture>("asteroid3", "assets/asteroids/asteroid03.png");
    resourceManager.add<sf::Texture>("asteroid4", "assets/asteroids/asteroid04.png");

    resourceManager.add<GameSettings>("settings");

    resourceManager.add<sf::Texture>("player_blue_anim_spritesheet", "assets/player/player_blue_anim.png");
    std::vector<sf::IntRect> frames;
    frames.push_back(sf::IntRect{{0, 0}, {64, 64}});
    frames.push_back(sf::IntRect{{64, 0}, {64, 64}});
    frames.push_back(sf::IntRect{{128, 0}, {64, 64}});

    resourceManager.add<Animation>("player_blue_anim", resourceManager.get<sf::Texture>("player_blue_anim_spritesheet"),
                                   frames, 0.2f, true);

    auto& spaceShip = entityManager.spawn();

    auto& ctransform = spaceShip.addComponent<CTransform>();
    ctransform.setPosition({0.f, 200.f});

    // Add an animated sprite to the spaceship just to test the system.
    auto& csprite = spaceShip.addComponent<CAnimatedSprite>();
    csprite.setAnimation(resourceManager.get<Animation>("player_blue_anim"));
    csprite.setDrawOrder(1);
    csprite.play();

    spaceShip.addComponent<CSpeed>(sf::Vector2f{0.f, 0.f});
    spaceShip.addComponent<CSpaceShip>();

    auto& col = spaceShip.addComponent<CEllipseCollider>(Layer::Player);
    col.rx = csprite.getSize().x / 2.5f;
    col.ry = csprite.getSize().y / 2.f;

    auto& gameManager = entityManager.spawn();
    gameManager.addComponent<CGameManager>();

    auto& background = entityManager.spawn();
    auto& transform1 = background.addComponent<CTransform>();
    transform1.setScale({4.f, 4.f});
    background.addComponent<CSprite>(resourceManager.get<sf::Texture>("background"), -10);
}

void Application::processEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
        input.handleEvent(*event);
    }
}

void Application::update(float dt)
{
    window.clear(sf::Color::Black);
    entityManager.update(dt, input, window);
    window.display();
}
