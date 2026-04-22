#pragma once

#include <functional>

#include "Defines.hpp"

enum class ResourceID : u8
{
    PlayerBlue1,
    Background,
    Projectile,
    Asteroid1,
    Asteroid2,
    Asteroid3,
    Asteroid4,
    Settings,
    PlayerBlueAnimSpritesheet,
    PlayerBlueAnim,
};

// Hash support so ResourceID can be used as an unordered_map key.
template <>
struct std::hash<ResourceID>
{
    usize operator()(ResourceID id) const noexcept { return std::hash<u8>{}(static_cast<u8>(id)); }
};
