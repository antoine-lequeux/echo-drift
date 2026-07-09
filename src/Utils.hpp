#pragma once

#include <random>
#include <type_traits>

#include "Defines.hpp"

namespace utils
{
// Utility to get a random number of type T in [min, max].
template <typename T>
T getRandom(T min, T max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    if constexpr (std::is_integral_v<T>)
    {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(gen);
    }
    else
    {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(gen);
    }
}
} // namespace utils
