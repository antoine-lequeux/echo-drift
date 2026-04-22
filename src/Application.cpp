#include "Application.hpp"
#include "Animation.hpp"
#include "comps/CAnimatedSprite.hpp"
#include "comps/CCollider.hpp"
#include "comps/CGameManager.hpp"
#include "comps/CMultiSprite.hpp"
#include "comps/CSpaceShip.hpp"
#include "comps/CSpeed.hpp"
#include "comps/CSprite.hpp"
#include "comps/CTransform.hpp"

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
        f32 dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        input.clear();
    }
}

void Application::setup()
{
    resourceManager.add<sf::Texture>(ResourceID::PlayerBlue1, "assets/player/player_blue_frame01.png");

    resourceManager.add<sf::Texture>(ResourceID::Background, "assets/backgrounds/background_vertical.png");

    resourceManager.add<sf::Texture>(ResourceID::Projectile, "assets/projectiles/laser_medium.png");

    resourceManager.add<sf::Texture>(ResourceID::Asteroid1, "assets/asteroids/asteroid01.png");
    resourceManager.add<sf::Texture>(ResourceID::Asteroid2, "assets/asteroids/asteroid02.png");
    resourceManager.add<sf::Texture>(ResourceID::Asteroid3, "assets/asteroids/asteroid03.png");
    resourceManager.add<sf::Texture>(ResourceID::Asteroid4, "assets/asteroids/asteroid04.png");

    resourceManager.add<GameSettings>(ResourceID::Settings);

    resourceManager.add<sf::Texture>(ResourceID::PlayerBlueAnimSpritesheet, "assets/player/player_blue_anim.png");
    std::vector<sf::IntRect> frames = {
        {{0, 0}, {64, 64}},
        {{64, 0}, {64, 64}},
        {{128, 0}, {64, 64}},
    };

    resourceManager.add<Animation>(ResourceID::PlayerBlueAnim,
                                   resourceManager.get<sf::Texture>(ResourceID::PlayerBlueAnimSpritesheet), frames,
                                   0.2f, true);

    auto& spaceShip = entityManager.spawn();

    auto& ctransform = spaceShip.addComponent<CTransform>();
    ctransform.setPosition({0.f, 200.f});

    // Add an animated sprite to the spaceship just to test the system.
    // auto& csprite = spaceShip.addComponent<CAnimatedSprite>();
    // csprite.setAnimation(resourceManager.get<Animation>(ResourceID::PlayerBlueAnim));
    // csprite.setDrawOrder(1);
    // csprite.play();

    auto& csprite = spaceShip.addComponent<CMultiSprite>(
        resourceManager.get<sf::Texture>(ResourceID::PlayerBlueAnimSpritesheet), 3, 1);

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
    background.addComponent<CSprite>(resourceManager.get<sf::Texture>(ResourceID::Background), -10);
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

void Application::update(f32 dt)
{
    window.clear(sf::Color::Black);
    entityManager.update(dt, input, window);
    window.display();
}
