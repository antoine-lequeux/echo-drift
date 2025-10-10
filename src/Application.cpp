#include "Application.hpp"

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
        render();
        input.clear();
    }
}

void Application::setup()
{
    entityManager.addEntity(std::make_unique<SpaceShip>("assets/ship.png", 1));
    entityManager.addEntity(std::make_unique<GameManager>());
    entityManager.addEntity(std::make_unique<Background>("assets/backgrounds/layer_01_void.png", -10));
    entityManager.addEntity(std::make_unique<Background>("assets/backgrounds/layer_02_stars.png", -9));
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

void Application::update(float dt) { entityManager.update(dt, input); }

void Application::render()
{
    window.clear(sf::Color::Black);
    entityManager.render(window);
    window.display();
}