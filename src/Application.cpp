#include "Application.hpp"

Application::Application() : window(sf::VideoMode({800, 600}), "Echo Drift") { window.setFramerateLimit(60); }

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

void Application::setup() { entityManager.addEntity(std::make_unique<SpaceShip>("assets/ship.png")); }

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