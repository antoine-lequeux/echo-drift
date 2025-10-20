#include <algorithm>
#include <iostream>
#include <ranges>

#include "GameObject.hpp"
#include "comps/CSprite.hpp"
#include "comps/CTransform.hpp"

GameObjectManager::GameObjectManager(ResourceManager& rm) : resources(rm) {}

GameObject& GameObjectManager::spawn()
{
    auto newGameObject = std::make_unique<GameObject>();
    GameObject& ref = *newGameObject;
    pendingGameObjects.push_back(std::move(newGameObject));
    return ref;
}

const GameObjects& GameObjectManager::getAll() const { return gameObjects; }

void GameObjectManager::update(float dt, Input& input, sf::RenderWindow& window)
{
    // Create a context for this update cycle.
    Context ctx{input, window, *this, dt};

    sortGameObjectsBySpriteDrawOrder(gameObjects);

    // Update gameObjects.
    for (auto& gameObject : gameObjects)
    {
        gameObject->update(ctx);
    }

    // Remove marked gameObjects.
    std::erase_if(gameObjects, [](const std::unique_ptr<GameObject>& e) { return e->isMarkedForDeletion(); });

    // Add pending gameObjects.
    std::ranges::move(pendingGameObjects, std::back_inserter(gameObjects));

    pendingGameObjects.clear();
}

// Sort gameObjects by their CSprite draw order, skipping those without a CSprite.
void GameObjectManager::sortGameObjectsBySpriteDrawOrder(GameObjects& gameObjects)
{
    // Collect positions of gameObjects that have a CSprite component.
    std::vector<size_t> positions;
    positions.reserve(gameObjects.size());
    for (size_t i = 0; i < gameObjects.size(); ++i)
        if (gameObjects[i] && gameObjects[i]->template getComponent<CSprite>())
            positions.push_back(i);

    if (positions.empty())
        return;

    // Move the selected gameObjects out to a temporary vector.
    GameObjects tmp;
    tmp.reserve(positions.size());
    for (size_t pos : positions)
        tmp.push_back(std::move(gameObjects[pos]));

    // Sort the temporary vector by CSprite draw order.
    std::sort(tmp.begin(), tmp.end(),
              [](const std::unique_ptr<GameObject>& a, const std::unique_ptr<GameObject>& b)
              {
                  auto sa = a->template getComponent<CSprite>();
                  auto sb = b->template getComponent<CSprite>();
                  return sa->getDrawOrder() < sb->getDrawOrder();
              });

    // Move the sorted gameObjects back to their original positions.
    for (size_t i = 0; i < positions.size(); ++i)
        gameObjects[positions[i]] = std::move(tmp[i]);
}

void GameObject::addChild(GameObject& gameObject)
{
    if (!getComponent<CTransform>())
    {
        std::cerr << "A GameObject without a Transform cannot have children.\n";
        return;
    }
    children.push_back(&gameObject);
    gameObject.parent = this;
}
