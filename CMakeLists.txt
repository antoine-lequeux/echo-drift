cmake_minimum_required(VERSION 3.16)
project(echo-drift LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static" FORCE)
set(SFML_USE_STATIC_STD_LIBS ON CACHE BOOL "Static CRT" FORCE)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG master
)
FetchContent_MakeAvailable(SFML)

add_executable(echo-drift 
    main.cpp
    src/Input.hpp
    src/Input.cpp
    src/Application.hpp
    src/Application.cpp
    src/GameObject.hpp
    src/GameObject.cpp
    src/comps/CSprite.hpp
    src/comps/CSprite.cpp
    src/comps/CSpaceShip.hpp
    src/comps/CSpaceShip.cpp
    src/comps/CSpeed.hpp
    src/comps/CDespawner.hpp
    src/comps/CGameManager.hpp
    src/comps/CGameManager.cpp
    src/comps/CCollider.hpp
    src/comps/CCollider.cpp
    src/comps/CProjectile.hpp
    src/comps/CTransform.hpp
    src/comps/Ctransform.cpp
    src/Tools.hpp
    src/ResourceManager.hpp
    src/Animation.hpp
    src/comps/CAnimatedSprite.hpp
    src/comps/CMultiSprite.hpp
    src/Defines.hpp
)

target_compile_definitions(echo-drift PRIVATE SFML_STATIC)
target_link_libraries(echo-drift PRIVATE sfml-graphics sfml-window sfml-system)

if(MINGW)
    target_link_options(echo-drift PRIVATE -static -static-libstdc++ -static-libgcc)
endif()