# Echo Drift

<img src="docs/gameplay.webp" width="100%" />

Echo Drift is a 2D space shooter project developed in C++ using SFML3. Its primary purpose is an **educational exploration of high-performance software architecture, memory management, and engine optimization**. 

> **Current State:** The core engine architecture is complete and highly optimized. However, the game itself currently features only basic player controls and collision testing rather than complete gameplay loops.

---

## Performance: 200+ FPS with 3000+ colliding Entities on a low-end laptop
The engine is designed from the ground up to handle massive amounts of entities on screen without frame drops. Achieving a stable 200 FPS while simulating and rendering 3200 active game objects with collision detection requires a combination of strict architectural decisions and algorithmic optimizations.

> Note: This benchmark was captured on a heavily thermally-throttled 2020 HP laptop with an 8-core Intel i7 CPU and a low-end GPU (MX450 with 2Gb VRAM), which demonstrates the engine's efficiency under severe hardware constraints.

### Architecture & ECS-Lite
The engine utilizes a Component-Based architecture inspired by the **Entity-Component-System (ECS)** pattern. 
*   **Data-Oriented approach:** GameObjects are lightweight containers that aggregate Components. 
*   **Coupled Behavior:** Unlike a pure ECS where data (components) and logic (systems) are strictly decoupled, Echo Drift implements an ECS-Lite structure. Components in this engine encapsulate both their data and their `update()` behavior. This reduces boilerplate while still providing excellent composition and flexibility.

### Memory Management & Cache Locality
To maintain high frame rates, avoiding CPU cache misses is crucial.
*   **[Flat Vector Component Storage](src/GameObject.hpp#L188):** Instead of using slow, heap-allocating structures like `std::unordered_map` for component retrieval, `GameObject` component lists are stored in contiguous `std::vector` arrays. Because the number of components per entity is small, a flat linear scan (`std::find_if`) through contiguous memory vastly outperforms hashing, thanks to L1/L2 CPU cache predictability.
*   **[Object Lifecycle Management](src/GameObject.hpp#L227):** Entities are managed in bulk by the `GameObjectManager`, allowing for deterministic creation and destruction at the end of the frame, avoiding mid-loop pointer invalidation.

### Algorithmic Optimizations
Collision detection is generally the biggest bottleneck in 2D engines with high entity counts. Echo Drift solves this through multiple optimization layers:

1.  **[Spatial Partitioning Grid](src/GameObject.cpp#L72):** 
    The world is divided into a static spatial grid. Instead of testing every entity against every other entity $O(N^2)$, entities only check for collisions against objects residing in their local grid cells, drastically reducing collision pairs. Furthermore, the grid coordinate mapping utilizes fast integer truncation (`static_cast<i32>`) rather than slow floating-point operations like `std::floor`.
2.  **[AABB Early-Outs](src/comps/CCollider.cpp#L83):**
    Before performing complex mathematical intersection tests, the engine runs an extremely cheap Axis-Aligned Bounding Box (AABB) intersection check. If the bounding boxes don't overlap, the expensive math is skipped entirely.
3.  **[Separating Axis Theorem (SAT) Caching](src/comps/CCollider.cpp#L70-L76):**
    For precise polygonal collisions, the engine uses SAT. Traditionally, calculating the normal axes for polygons is mathematically heavy. Echo Drift **caches polygon axes** per collider; they are calculated exactly once per frame during the polygon rebuild phase. If an asteroid collides with 20 different bullets, its axes are reused instantly instead of being recalculated 20 times.

---

## Building the Project

Ensure you have CMake and a compatible C++ compiler installed.

```bash
# Configure the project in Release mode
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build
```

## Credits
*   **Pixel Space Shooter Assets** by [Ravenmore](https://ravenmore.itch.io/pixel-space-shooter-assets)