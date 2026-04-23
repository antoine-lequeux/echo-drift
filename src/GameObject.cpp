#include <algorithm>
#include <iostream>
#include <ranges>

#include "GameObject.hpp"
#include "comps/CCollider.hpp"
#include "comps/CSprite.hpp"
#include "comps/CTransform.hpp"

GameObjectManager::GameObjectManager(ResourceManager& rm) : resources(rm) {}

GameObject& GameObjectManager::spawn()
{
    auto newGameObject = std::make_unique<GameObject>();
    GameObject& ref = *newGameObject;
    pendingGameObjects.push_back(std::move(newGameObject));
    drawOrderDirty = true;
    return ref;
}

const GameObjects& GameObjectManager::getAll() const { return gameObjects; }

void GameObjectManager::update(f32 dt, Input& input, sf::RenderWindow& window)
{
    // Create a context for this update cycle.
    Context ctx{input, window, *this, dt};

    sortGameObjectsBySpriteDrawOrder(gameObjects);

    // Update gameObjects.
    for (auto& gameObject : gameObjects)
    {
        gameObject->update(ctx);
    }

    processCollisions();

    // Remove marked gameObjects.
    std::erase_if(gameObjects, [](const std::unique_ptr<GameObject>& e) { return e->isMarkedForDeletion(); });

    // Add pending gameObjects. Re-mark dirty so the draw order sort runs on the next frame.
    if (!pendingGameObjects.empty())
    {
        drawOrderDirty = true;
        std::ranges::move(pendingGameObjects, std::back_inserter(gameObjects));
        pendingGameObjects.clear();
    }
}

// Sort gameObjects by their CSprite draw order. Only runs when drawOrderDirty is true.
void GameObjectManager::sortGameObjectsBySpriteDrawOrder(GameObjects& gameObjects)
{
    if (!drawOrderDirty)
        return;
    drawOrderDirty = false;

    std::stable_sort(gameObjects.begin(), gameObjects.end(),
                     [](const std::unique_ptr<GameObject>& a, const std::unique_ptr<GameObject>& b)
                     {
                         auto* sa = a->getComponent<CSprite>();
                         auto* sb = b->getComponent<CSprite>();
                         i32 oa = sa ? sa->getDrawOrder() : 0;
                         i32 ob = sb ? sb->getDrawOrder() : 0;
                         return oa < ob;
                     });
}

void GameObjectManager::processCollisions()
{
    for (size_t i = 0; i < gameObjects.size(); ++i)
    {
        if (gameObjects[i]->isMarkedForDeletion())
            continue;

        auto* a = gameObjects[i]->getComponent<CCollider>();
        if (!a)
            continue;

        for (size_t j = i + 1; j < gameObjects.size(); ++j)
        {
            if (gameObjects[j]->isMarkedForDeletion())
                continue;

            auto* b = gameObjects[j]->getComponent<CCollider>();
            if (!b || !a->isTouching(*b))
                continue;

            Layer la = a->getLayer();
            Layer lb = b->getLayer();

            // Destroy both objects when a player or projectile collides with an asteroid.
            bool involvesAsteroid = (la == Layer::Asteroid || lb == Layer::Asteroid);
            bool involvesAttacker =
                (la == Layer::Player || la == Layer::Projectile || lb == Layer::Player || lb == Layer::Projectile);

            if (involvesAsteroid && involvesAttacker)
            {
                gameObjects[i]->destroy();
                gameObjects[j]->destroy();
            }
        }
    }
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
