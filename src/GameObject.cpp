#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

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

    // Remove marked gameObjects and colliders.
    std::erase_if(gameObjects, [](const std::unique_ptr<GameObject>& e) { return e->isMarkedForDeletion(); });
    std::erase_if(asteroidColliders, [](CCollider* c) { return c->gameObject.isMarkedForDeletion(); });
    std::erase_if(dynamicColliders, [](CCollider* c) { return c->gameObject.isMarkedForDeletion(); });

    // Add pending gameObjects. Re-mark dirty so the draw order sort runs on the next frame.
    if (!pendingGameObjects.empty())
    {
        drawOrderDirty = true;
        for (auto& obj : pendingGameObjects)
        {
            if (auto* col = obj->getComponent<CCollider>())
            {
                if (col->getLayer() == Layer::Asteroid)
                    asteroidColliders.push_back(col);
                else
                    dynamicColliders.push_back(col);
            }
        }
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
                     { return a->drawOrder < b->drawOrder; });
}

void GameObjectManager::processCollisions()
{
    for (auto& cell : asteroidGrid)
        cell.clear();

    const f32 halfWorldWidth = (GridSizeX * CollisionGridCellSize) / 2.f;
    const f32 halfWorldHeight = (GridSizeY * CollisionGridCellSize) / 2.f;

    auto getCellCoords = [&](f32 x, f32 y, i32& cx, i32& cy)
    {
        cx = static_cast<i32>(std::floor((x + halfWorldWidth) / CollisionGridCellSize));
        cy = static_cast<i32>(std::floor((y + halfWorldHeight) / CollisionGridCellSize));
    };

    // Populate grid with Asteroids.
    for (CCollider* collider : asteroidColliders)
    {
        if (collider->gameObject.isMarkedForDeletion() || collider->getPolygon().empty())
            continue;

        const auto& bounds = collider->getBounds();

        i32 minX, maxX, minY, maxY;
        getCellCoords(bounds.minX, bounds.minY, minX, minY);
        getCellCoords(bounds.maxX, bounds.maxY, maxX, maxY);

        minX = std::max(0, std::min(GridSizeX - 1, minX));
        maxX = std::max(0, std::min(GridSizeX - 1, maxX));
        minY = std::max(0, std::min(GridSizeY - 1, minY));
        maxY = std::max(0, std::min(GridSizeY - 1, maxY));

        for (i32 y = minY; y <= maxY; ++y)
        {
            for (i32 x = minX; x <= maxX; ++x)
            {
                asteroidGrid[y * GridSizeX + x].push_back(collider);
            }
        }
    }

    std::vector<std::pair<CCollider*, CCollider*>> collisionsToResolve;
    collisionsToResolve.reserve(100);

    // Check Players and Projectiles within the grid.
    for (CCollider* a : dynamicColliders)
    {
        if (a->gameObject.isMarkedForDeletion() || a->getPolygon().empty())
            continue;

        const auto& bounds = a->getBounds();

        i32 minX, maxX, minY, maxY;
        getCellCoords(bounds.minX, bounds.minY, minX, minY);
        getCellCoords(bounds.maxX, bounds.maxY, maxX, maxY);

        // If completely outside the grid, no asteroid could possibly hit it.
        if (maxX < 0 || minX >= GridSizeX || maxY < 0 || minY >= GridSizeY)
            continue;

        minX = std::max(0, std::min(GridSizeX - 1, minX));
        maxX = std::max(0, std::min(GridSizeX - 1, maxX));
        minY = std::max(0, std::min(GridSizeY - 1, minY));
        maxY = std::max(0, std::min(GridSizeY - 1, maxY));

        for (i32 y = minY; y <= maxY; ++y)
        {
            for (i32 x = minX; x <= maxX; ++x)
            {
                for (CCollider* b : asteroidGrid[y * GridSizeX + x])
                {
                    if (b->gameObject.isMarkedForDeletion() || b->getPolygon().empty())
                        continue;

                    if (a->isTouching(*b))
                    {
                        collisionsToResolve.emplace_back(a, b);
                    }
                }
            }
        }
    }

    // Resolve collisions
    for (const auto& [a, b] : collisionsToResolve)
    {
        if (!a->gameObject.isMarkedForDeletion() && !b->gameObject.isMarkedForDeletion())
        {
            a->gameObject.destroy();
            b->gameObject.destroy();
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
