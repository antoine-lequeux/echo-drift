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

#include <array>
#include <cmath>
#include <filesystem>
#include <string>

Application::Application()
    : window(sf::VideoMode({1280, 720}), "Echo Drift"), entityManager(resourceManager), uiFont(), fpsText(uiFont, "FPS: 0", 30),
      entityCountText(uiFont, "Entities: 0", 30)
{
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(0);
    gameView = sf::View({0.f, 0.f}, {1280.f, 720.f});
    gameView.zoom(0.75f);
    window.setView(gameView);

    lastFpsUpdateTime = std::chrono::steady_clock::now();
    const std::array<std::filesystem::path, 3> fontCandidates = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
    };

    for (const auto& fontPath : fontCandidates)
    {
        if (uiFont.openFromFile(fontPath)) { break; }
    }

    if (!uiFont.openFromFile("assets/fonts/quantico.ttf")) std::cerr << "Font not found!\n";

    fpsText.setFont(uiFont);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition({10.f, 10.f});

    entityCountText.setFont(uiFont);
    entityCountText.setFillColor(sf::Color::White);
    entityCountText.setPosition({10.f, 60.f});
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

    resourceManager.add<Animation>(ResourceID::PlayerBlueAnim, resourceManager.get<sf::Texture>(ResourceID::PlayerBlueAnimSpritesheet), frames, 0.2f, true);

    auto& spaceShip = entityManager.spawn();

    auto& ctransform = spaceShip.addComponent<CTransform>();
    ctransform.setPosition({0.f, 200.f});

    // Add an animated sprite to the spaceship just to test the system.
    // auto& csprite = spaceShip.addComponent<CAnimatedSprite>();
    // csprite.setAnimation(resourceManager.get<Animation>(ResourceID::PlayerBlueAnim));
    // csprite.setDrawOrder(1);
    // csprite.play();

    auto& csprite = spaceShip.addComponent<CMultiSprite>(resourceManager.get<sf::Texture>(ResourceID::PlayerBlueAnimSpritesheet), 3, 1);

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
        if (event->is<sf::Event::Closed>()) window.close();
        input.handleEvent(*event);
    }
}

void Application::update(f32 dt)
{
    ++frameCounter;
    const auto now = std::chrono::steady_clock::now();
    const auto elapsedMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - lastFpsUpdateTime).count();

    if (elapsedMicroseconds >= 500000)
    {
        const double fps = static_cast<double>(frameCounter) / (static_cast<double>(elapsedMicroseconds) / 1'000'000.0);
        fpsValue = static_cast<u32>(std::llround(fps));
        fpsText.setString("FPS: " + std::to_string(fpsValue));

        entityCountValue = static_cast<u32>(entityManager.count());
        entityCountText.setString("Entities: " + std::to_string(entityCountValue));

        frameCounter = 0;
        lastFpsUpdateTime = now;
    }

    window.clear(sf::Color::Black);
    entityManager.update(dt, input, window);

    const auto previousView = window.getView();
    window.setView(window.getDefaultView());
    window.draw(fpsText);
    window.draw(entityCountText);
    window.setView(previousView);

    window.display();
}
