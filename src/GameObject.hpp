#pragma once

#include "GameSettings.hpp"
#include "Input.hpp"
#include "ResourceManager.hpp"
#include "Tools.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <vector>

// Forward declarations to avoid circular dependencies.
class GameObjectManager;
class GameObject;

// Forward declarations for intermediate component base classes.
class CSprite;
class CCollider;

// Context passed to components during update.
struct Context
{
    Input& input;
    sf::RenderWindow& window;
    GameObjectManager& manager;
    f32 dt;
};

// Base abstract class for all components.
class Component
{
public:

    Component(GameObject& gameObject) : gameObject(gameObject) {}
    virtual ~Component() = default;

    // Update method to be implemented by derived components.
    virtual void update(Context& ctx) = 0;

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
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        auto component = std::make_unique<T>(*this, std::forward<Args>(args)...);
        T& ref = *component;

        // If replacing an existing component, remove the old aliases first.
        auto concreteKey = std::type_index(typeid(T));
        if (ownedComponents.contains(concreteKey))
            removeAliasesFor(ownedComponents[concreteKey].get());

        // Transfer ownership.
        ownedComponents[concreteKey] = std::move(component);

        // Register the concrete key.
        aliasComponents[concreteKey] = &ref;

        // Register alias keys for every base class between T and Component.
        registerBaseAliases<T>(ref);

        return ref;
    }

    // Remove the component of type T (identified by its concrete type).
    template <typename T>
    void removeComponent()
    {
        auto it = ownedComponents.find(std::type_index(typeid(T)));
        if (it == ownedComponents.end())
            return;
        removeAliasesFor(it->second.get());
        ownedComponents.erase(it);
    }

    // Returns true if the gameObject has a component of type T (or a subclass of T).
    template <typename T>
    bool hasComponent() const
    {
        return getComponent<T>() != nullptr;
    }

    // Get the component of type T (or a registered subclass).
    template <typename T>
    T* getComponent()
    {
        auto key = std::type_index(typeid(T));
        auto it = aliasComponents.find(key);
        if (it != aliasComponents.end())
            return static_cast<T*>(it->second);

        // Fallback: scan ownedComponents with dynamic_cast and cache the hit.
        for (auto& [k, comp] : ownedComponents)
        {
            if (auto* c = dynamic_cast<T*>(comp.get()))
            {
                aliasComponents[key] = c;
                return c;
            }
        }
        return nullptr;
    }

    template <typename T>
    const T* getComponent() const
    {
        auto it = aliasComponents.find(std::type_index(typeid(T)));
        if (it != aliasComponents.end())
            return static_cast<const T*>(it->second);

        for (auto& [k, comp] : ownedComponents)
            if (auto* c = dynamic_cast<const T*>(comp.get()))
                return c;
        return nullptr;
    }

    // Call fn(Component&) for every component on this object.
    template <typename Fn>
    void eachComponent(Fn&& fn)
    {
        for (auto& [key, comp] : ownedComponents)
            fn(*comp);
    }

    // Update all components of the gameObject.
    void update(Context& ctx)
    {
        for (auto& [key, comp] : ownedComponents)
            comp->update(ctx);
    }

    void addChild(GameObject& gameObject);
    std::vector<GameObject*>& getChildren() { return children; }
    const std::vector<GameObject*>& getChildren() const { return children; }
    GameObject* getParent() const { return parent; }

    // Recursive destruction of all children.
    void destroyChildren()
    {
        for (auto* child : children)
        {
            child->parent = nullptr;
            child->isDead = true;
            child->destroyChildren();
        }
        children.clear();
    }

    // Mark this object (and all its children) for deletion at end of frame.
    void destroy()
    {
        isDead = true;
        destroyChildren();
        if (parent)
        {
            std::erase(parent->children, this);
            parent = nullptr;
        }
    }
    bool isMarkedForDeletion() const { return isDead; }

private:

    std::unordered_map<std::type_index, std::unique_ptr<Component>> ownedComponents;
    std::unordered_map<std::type_index, Component*> aliasComponents;

    // Remove all alias entries that point to the given raw pointer.
    void removeAliasesFor(Component* ptr)
    {
        std::erase_if(aliasComponents, [ptr](const auto& kv) { return kv.second == ptr; });
    }

    // Try to cast the component to each known intermediate base and insert alias entries.
    template <typename T>
    void registerBaseAliases(T& instance)
    {
        // List every class that sits between a concrete component and Component itself.
        tryRegisterAlias<CSprite>(instance);
        tryRegisterAlias<CCollider>(instance);
    }

    template <typename Base, typename T>
    void tryRegisterAlias(T& instance)
    {
        if constexpr (std::is_base_of_v<Base, T> && !std::is_same_v<Base, T>)
            aliasComponents[std::type_index(typeid(Base))] = static_cast<Component*>(static_cast<Base*>(&instance));
    }

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

    GameObjectManager(ResourceManager& rm);

    // Create and add a new gameObject to the manager.
    // Returns a reference to the new gameObject.
    GameObject& spawn();

    // Provide read-only access to all live gameObjects.
    const GameObjects& getAll() const;

    // Call fn(T&) for every live GameObject that has a component of type T.
    template <typename T, typename Fn>
    void forEach(Fn&& fn)
    {
        for (auto& obj : gameObjects)
        {
            if (obj->isMarkedForDeletion())
                continue;
            if (auto* comp = obj->getComponent<T>())
                fn(*comp);
        }
    }

    // Call fn(A&, B&) for every live GameObject that has both components A and B.
    template <typename A, typename B, typename Fn>
    void forEach(Fn&& fn)
    {
        for (auto& obj : gameObjects)
        {
            if (obj->isMarkedForDeletion())
                continue;
            auto* a = obj->getComponent<A>();
            if (!a)
                continue;
            auto* b = obj->getComponent<B>();
            if (!b)
                continue;
            fn(*a, *b);
        }
    }

    // Return the first live GameObject that has a component of type T, or nullptr.
    template <typename T>
    GameObject* findFirst()
    {
        for (auto& obj : gameObjects)
            if (!obj->isMarkedForDeletion() && obj->hasComponent<T>())
                return obj.get();
        return nullptr;
    }

    // Return the number of live GameObjects that have a component of type T.
    template <typename T>
    usize count() const
    {
        usize n = 0;
        for (auto& obj : gameObjects)
            if (!obj->isMarkedForDeletion() && obj->hasComponent<T>())
                ++n;
        return n;
    }

    // Signal that draw order has changed and a re-sort is needed next frame.
    void markDrawOrderDirty() { drawOrderDirty = true; }

    // This reference is always valid, since the resource manager and the gameobject manager
    // are both destroyed at the same time, when the app terminates.
    ResourceManager& resources;

private:

    // Allow Application to access private update() method.
    friend class Application;

    // Update all entities and handle addition/removal.
    void update(f32 dt, Input& input, sf::RenderWindow& window);

    // Detects layer-based collisions and destroys involved entities.
    void processCollisions(Context& ctx);

    // Sort gameObjects by CSprite draw order. Only runs when drawOrderDirty is true.
    void sortGameObjectsBySpriteDrawOrder(GameObjects& gameObjects);

    // GameObjects to be added after the current update cycle.
    GameObjects pendingGameObjects;

    // All current gameObjects.
    GameObjects gameObjects;

    // Set to true whenever a spawn or draw order change requires a re-sort.
    bool drawOrderDirty = true;
};