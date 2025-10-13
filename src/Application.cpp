#include "Application.hpp"
#include "comps/CCollider.hpp"
#include "comps/CGameManager.hpp"
#include "comps/CSpaceShip.hpp"
#include "comps/CSpeed.hpp"
#include "comps/CSprite.hpp"

Application::Application() : window(sf::VideoMode({1280, 720}), "Echo Drift")
{
    window.setFramerateLimit(60);
    gameView = sf::View({0.f, 0.f}, {1280.f, 720.f});
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
    auto spaceShip = std::make_unique<GameObject>();
    auto& csprite = spaceShip->addComponent<CSprite>("assets/ship.png", 1);
    csprite.setPosition({0.f, 200.f});
    csprite.setScale({3.f, 3.f});
    spaceShip->addComponent<CSpeed>(sf::Vector2f{0.f, 0.f});
    spaceShip->addComponent<CSpaceShip>();
    spaceShip->addComponent<CCollider>(Layer::Player);
    entityManager.spawn(std::move(spaceShip));

    auto gameManager = std::make_unique<GameObject>();
    gameManager->addComponent<CGameManager>();
    entityManager.spawn(std::move(gameManager));

    auto background1 = std::make_unique<GameObject>();
    auto& csprite1 = background1->addComponent<CSprite>("assets/backgrounds/layer_01_void.png", -10);
    csprite1.setScale({2.f, 2.f});
    entityManager.spawn(std::move(background1));

    auto background2 = std::make_unique<GameObject>();
    auto& csprite2 = background2->addComponent<CSprite>("assets/backgrounds/layer_02_stars.png", -9);
    csprite2.setScale({2.f, 2.f});
    entityManager.spawn(std::move(background2));
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
