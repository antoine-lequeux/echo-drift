#pragma once

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

    // Add a resource of T type to the manager, using the arguments to create a shared_ptr.
    // The id is a string that can be used to later retrieve the resource.
    template <typename T, typename... Args>
    T& add(const std::string& id, Args&&... args)
    {
        auto& map = getMap<T>();
        auto obj = std::make_shared<T>(std::forward<Args>(args)...);
        map[id] = obj;
        return *obj;
    }

    // Use the id of a stored resource to return the shared_ptr pointing to it.
    template <typename T>
    std::shared_ptr<T> get(const std::string& id)
    {
        auto& map = getMap<T>();
        if (auto it = map.find(id); it != map.end())
            return it->second;
        throw std::runtime_error("Resource not found: " + id);
    }

private:

    // "resources" maps each resource type (via std::type_index) to its corresponding resource map.
    // Each resource map stores pairs of (string -> std::shared_ptr<T>).
    // std::any is used to hold maps of different types in a single container.
    std::unordered_map<std::type_index, std::any> resources;

    // Get the map corresponding to the right resource type.
    template <typename T>
    std::unordered_map<std::string, std::shared_ptr<T>>& getMap()
    {
        auto type = std::type_index(typeid(T));

        // Add a new map if it does not exist yet.
        if (!resources.contains(type))
            resources[type] = std::unordered_map<std::string, std::shared_ptr<T>>{};

        // Return the existing map.
        return std::any_cast<std::unordered_map<std::string, std::shared_ptr<T>>&>(resources[type]);
    }
};