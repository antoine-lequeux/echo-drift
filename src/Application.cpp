#include "Application.hpp"

Application::Application() : window(sf::VideoMode({800, 600}), "Echo Drift"), player("assets/ship.png", 200.0f)
{
    window.setFramerateLimit(60);
}

void Application::run()
{
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

void Application::processEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
        input.handleEvent(*event);
    }
}

void Application::update(float dt) { player.update(dt, input); }

void Application::render()
{
    window.clear(sf::Color::Black);

    player.render(window);

    window.display();
}