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

namespace
{
constexpr f32 CollisionGridCellSize = 200.f;

struct Aabb
{
    f32 minX{};
    f32 maxX{};
    f32 minY{};
    f32 maxY{};
};

struct CellKey
{
    i32 x{};
    i32 y{};

    bool operator==(const CellKey& other) const = default;
};

struct CellKeyHash
{
    usize operator()(const CellKey& key) const noexcept
    {
        return (static_cast<u64>(static_cast<u32>(key.x)) << 32) | static_cast<u32>(key.y);
    }
};

using BucketMap = std::unordered_map<CellKey, std::vector<usize>, CellKeyHash>;

Aabb getBounds(const Polygon& polygon)
{
    Aabb bounds{polygon[0].x, polygon[0].x, polygon[0].y, polygon[0].y};
    for (const auto& point : polygon)
    {
        bounds.minX = std::min(bounds.minX, point.x);
        bounds.maxX = std::max(bounds.maxX, point.x);
        bounds.minY = std::min(bounds.minY, point.y);
        bounds.maxY = std::max(bounds.maxY, point.y);
    }
    return bounds;
}

bool isPotentialCollisionPair(Layer a, Layer b)
{
    return (a == Layer::Asteroid && (b == Layer::Player || b == Layer::Projectile)) ||
           (b == Layer::Asteroid && (a == Layer::Player || a == Layer::Projectile));
}
} // namespace

void GameObjectManager::processCollisions()
{
    std::array<BucketMap, 3> spatialBuckets;
    for (auto& bucketMap : spatialBuckets)
        bucketMap.reserve(gameObjects.size() / 4 + 1);

    std::vector<usize> activeIndices;
    activeIndices.reserve(gameObjects.size());

    for (usize i = 0; i < gameObjects.size(); ++i)
    {
        if (gameObjects[i]->isMarkedForDeletion())
            continue;

        auto* collider = gameObjects[i]->getComponent<CCollider>();
        if (!collider || collider->getPolygon().empty())
            continue;

        const auto& polygon = collider->getPolygon();
        const auto bounds = getBounds(polygon);
        const i32 minCellX = static_cast<i32>(std::floor(bounds.minX / CollisionGridCellSize));
        const i32 maxCellX = static_cast<i32>(std::floor(bounds.maxX / CollisionGridCellSize));
        const i32 minCellY = static_cast<i32>(std::floor(bounds.minY / CollisionGridCellSize));
        const i32 maxCellY = static_cast<i32>(std::floor(bounds.maxY / CollisionGridCellSize));

        activeIndices.push_back(i);

        for (i32 cellY = minCellY; cellY <= maxCellY; ++cellY)
        {
            for (i32 cellX = minCellX; cellX <= maxCellX; ++cellX)
            {
                spatialBuckets[static_cast<usize>(collider->getLayer())][{cellX, cellY}].push_back(i);
            }
        }
    }

    std::vector<std::pair<usize, usize>> collisionsToResolve;
    collisionsToResolve.reserve(activeIndices.size() * 2);

    for (usize currentIndex : activeIndices)
    {
        auto* a = gameObjects[currentIndex]->getComponent<CCollider>();
        if (!a)
            continue;

        const auto& polygon = a->getPolygon();
        if (polygon.empty())
            continue;

        const auto bounds = getBounds(polygon);
        const i32 minCellX = static_cast<i32>(std::floor(bounds.minX / CollisionGridCellSize));
        const i32 maxCellX = static_cast<i32>(std::floor(bounds.maxX / CollisionGridCellSize));
        const i32 minCellY = static_cast<i32>(std::floor(bounds.minY / CollisionGridCellSize));
        const i32 maxCellY = static_cast<i32>(std::floor(bounds.maxY / CollisionGridCellSize));

        std::vector<usize> candidateIndices;
        candidateIndices.reserve(32);
        std::unordered_set<usize> seenCandidates;
        seenCandidates.reserve(32);

        const auto currentLayerIndex = static_cast<usize>(a->getLayer());
        std::array<usize, 2> relevantLayerIndices{};
        usize relevantLayerCount = 0;

        if (currentLayerIndex == static_cast<usize>(Layer::Asteroid))
        {
            relevantLayerIndices[0] = static_cast<usize>(Layer::Player);
            relevantLayerIndices[1] = static_cast<usize>(Layer::Projectile);
            relevantLayerCount = 2;
        }
        else
        {
            relevantLayerIndices[0] = static_cast<usize>(Layer::Asteroid);
            relevantLayerCount = 1;
        }

        for (i32 cellY = minCellY; cellY <= maxCellY; ++cellY)
        {
            for (i32 cellX = minCellX; cellX <= maxCellX; ++cellX)
            {
                for (i32 neighborY = cellY - 1; neighborY <= cellY + 1; ++neighborY)
                {
                    for (i32 neighborX = cellX - 1; neighborX <= cellX + 1; ++neighborX)
                    {
                        for (usize layerIndex = 0; layerIndex < relevantLayerCount; ++layerIndex)
                        {
                            auto it = spatialBuckets[relevantLayerIndices[layerIndex]].find({neighborX, neighborY});
                            if (it == spatialBuckets[relevantLayerIndices[layerIndex]].end())
                                continue;

                            for (usize candidateIndex : it->second)
                            {
                                if (candidateIndex <= currentIndex)
                                    continue;
                                if (!seenCandidates.insert(candidateIndex).second)
                                    continue;
                                candidateIndices.push_back(candidateIndex);
                            }
                        }
                    }
                }
            }
        }

        for (usize candidateIndex : candidateIndices)
        {
            if (gameObjects[candidateIndex]->isMarkedForDeletion())
                continue;

            auto* b = gameObjects[candidateIndex]->getComponent<CCollider>();
            if (!b || !isPotentialCollisionPair(a->getLayer(), b->getLayer()))
                continue;

            if (!a->isTouching(*b))
                continue;

            collisionsToResolve.emplace_back(currentIndex, candidateIndex);
        }
    }

    for (const auto& [i, j] : collisionsToResolve)
    {
        if (gameObjects[i]->isMarkedForDeletion() || gameObjects[j]->isMarkedForDeletion())
            continue;

        auto* a = gameObjects[i]->getComponent<CCollider>();
        auto* b = gameObjects[j]->getComponent<CCollider>();
        if (!a || !b)
            continue;

        if (!isPotentialCollisionPair(a->getLayer(), b->getLayer()) || !a->isTouching(*b))
            continue;

        gameObjects[i]->destroy();
        gameObjects[j]->destroy();
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
