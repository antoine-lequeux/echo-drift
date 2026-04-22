#pragma once

#include "Resources.hpp"

#include <any>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>

// This class provides a resource manager, that can be filled with new resources,
// which in turn can be accessed multiple times rather than being copied.
class ResourceManager
{
public:

    // Add a resource of type T identified by a ResourceID.
    template <typename T, typename... Args>
    T& add(ResourceID id, Args&&... args)
    {
        auto obj = std::make_shared<T>(std::forward<Args>(args)...);
        getMap<T>()[id] = obj;
        return *obj;
    }

    // Retrieve a stored resource by its ResourceID.
    template <typename T>
    std::shared_ptr<T> get(ResourceID id)
    {
        auto& map = getMap<T>();
        if (auto it = map.find(id); it != map.end())
            return it->second;
        throw std::runtime_error("Resource not found: id=" + std::to_string(static_cast<i32>(id)));
    }

private:

    // Maps each resource type (via std::type_index) to its own id->resource map.
    std::unordered_map<std::type_index, std::any> resources;

    // Returns (or inserts) the inner map for resource type T.
    template <typename T>
    std::unordered_map<ResourceID, std::shared_ptr<T>>& getMap()
    {
        auto type = std::type_index(typeid(T));
        auto it = resources.try_emplace(type, std::unordered_map<ResourceID, std::shared_ptr<T>>{}).first;
        return std::any_cast<std::unordered_map<ResourceID, std::shared_ptr<T>>&>(it->second);
    }
};