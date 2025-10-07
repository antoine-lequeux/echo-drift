#include "Input.hpp"

void Input::handleEvent(const sf::Event& event)
{
    if (event.is<sf::Event::KeyPressed>())
    {
        auto code = event.getIf<sf::Event::KeyPressed>()->code;
        for (auto& [action, key] : m_keyBindings)
        {
            if (code == key)
            {
                if (!m_actionActive[action])
                    m_actionTriggered[action] = true;
                m_actionActive[action] = true;
            }
        }
    }

    if (event.is<sf::Event::KeyReleased>())
    {
        auto code = event.getIf<sf::Event::KeyReleased>()->code;
        for (auto& [action, key] : m_keyBindings)
        {
            if (code == key)
            {
                m_actionActive[action] = false;
                m_actionTriggered[action] = false;
            }
        }
    }
}

bool Input::isActionActive(Action action) const
{
    auto it = m_actionActive.find(action);
    return it != m_actionActive.end() && it->second;
}

bool Input::isActionTriggered(Action action) const
{
    auto it = m_actionTriggered.find(action);
    return it != m_actionTriggered.end() && it->second;
}

void Input::clear() { m_actionTriggered.clear(); }
