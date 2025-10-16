#pragma once

#include "Input.hpp"

#include <cmath>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeindex>

// Forward declarations to avoid circular dependencies.
class GameObjectManager;
class GameObject;

// Context passed to components during update.
struct Context
{
    Input& input;
    sf::RenderWindow& window;
    GameObjectManager& manager;
    float dt;
};

// Base abstract class for all components.
class Component
{
public:

    Component(GameObject& gameObject) : gameObject(gameObject) {}
    virtual ~Component() = default;

    // Update method to be implemented by derived components.
    virtual void update(Context& ctx) = 0;

protected:

    // Reference to the owning GameObject.
    // The Component is destroyed if the GameObject is destroyed, so this reference is always valid.
    GameObject& gameObject;
};

// Class representing a game entity composed of multiple components.
class GameObject
{
public:

    GameObject() = default;
    virtual ~GameObject() = default;

    // Add a component of type T to the gameObject.
    // Args are forwarded to the component's constructor.
    // Returns a reference to the added component.
    template <typename T, typename... Args>
    T& addComponent(Args&&... args)
    {
        auto type = std::type_index(typeid(T));
        auto component = std::make_unique<T>(*this, std::forward<Args>(args)...);
        T& ref = *component;
        components[type] = std::move(component);
        return ref;
    }

    // Get the component of type T. Returns nullptr if not found.
    template <typename T>
    T* getComponent()
    {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end())
            return static_cast<T*>(it->second.get());
        return nullptr;
    }

    // Update all components of the gameObject.
    void update(Context& ctx)
    {
        for (auto& [type, comp] : components)
            comp->update(ctx);
    }

    void addChild(GameObject& gameObject);
    std::vector<GameObject*>& getChildren() { return children; }
    GameObject* getParent() const { return parent; }

    // Recursive destruction of all children.
    void destroyChildren()
    {
        for (auto* child : children)
        {
            child->isDead = true;
            child->destroyChildren();
        }
    }

    // Setter and getter for the deletion flag.
    void destroy()
    {
        isDead = true;
        destroyChildren();
    }
    bool isMarkedForDeletion() const { return isDead; }

private:

    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

    GameObject* parent = nullptr;
    std::vector<GameObject*> children;

    // Flag to mark the gameObject for deletion.
    bool isDead = false;
};

using GameObjects = std::vector<std::unique_ptr<GameObject>>;

// Manager class to handle all gameObjects in the game.
class GameObjectManager
{
public:

    // Add a new gameObject to the manager.
    void spawn(std::unique_ptr<GameObject> gameObject);

    // Provide read-only access to gameObjects for game objects
    const GameObjects& getAll() const;

private:

    // Allow Application to access private update() method.
    friend class Application;

    // Update all entities and handle addition/removal.
    void update(float dt, Input& input, sf::RenderWindow& window);

    // GameObjects to be added after the current update cycle.
    GameObjects pendingGameObjects;

    // All current gameObjects.
    GameObjects gameObjects;

    void sortGameObjectsBySpriteDrawOrder(GameObjects& gameObjects);
};