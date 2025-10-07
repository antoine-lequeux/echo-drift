#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

// Enum representing possible player actions.
enum class Action
{
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot,
    Pause
};

// Class handling input mapping and state.
class Input
{
public:

    // Process an SFML event to update action states.
    void handleEvent(const sf::Event& event);

    // Check if an action is currently active (key held down).
    bool isActionActive(Action action) const;

    // Check if an action was triggered (key pressed this frame).
    bool isActionTriggered(Action action) const;

    // Clear the input state; should be called once per frame.
    void clear();

private:

    std::unordered_map<Action, sf::Keyboard::Key> m_keyBindings = {
        {Action::MoveUp, sf::Keyboard::Key::Up},     {Action::MoveDown, sf::Keyboard::Key::Down},
        {Action::MoveLeft, sf::Keyboard::Key::Left}, {Action::MoveRight, sf::Keyboard::Key::Right},
        {Action::Shoot, sf::Keyboard::Key::Space},   {Action::Pause, sf::Keyboard::Key::Escape}};

    std::unordered_map<Action, bool> m_actionTriggered;
    std::unordered_map<Action, bool> m_actionActive;
};
