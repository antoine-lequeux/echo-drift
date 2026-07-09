cmake_minimum_required(VERSION 4.2.1)
project(echo-drift LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_UNITY_BUILD OFF CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
)
FetchContent_MakeAvailable(SFML)

if(TARGET freetype)
    target_compile_definitions(freetype PRIVATE FT2_BUILD_LIBRARY)
    set_target_properties(freetype PROPERTIES UNITY_BUILD OFF)
endif()

add_executable(echo-drift)

target_sources(echo-drift PRIVATE
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
    src/Utils.hpp
    src/ResourceManager.hpp
    src/Resources.hpp
    src/GameSettings.hpp
    src/Animation.hpp
    src/comps/CAnimatedSprite.hpp
    src/comps/CMultiSprite.hpp
    src/Defines.hpp
)

target_compile_features(echo-drift PRIVATE cxx_std_23)
target_compile_definitions(echo-drift PRIVATE SFML_STATIC)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(echo-drift PRIVATE
        -Wall -Wextra -Wpedantic
        -fcolor-diagnostics  
        -Wdocumentation
        $<$<CONFIG:Release>:-march=native -O3 -flto>
    )
endif()

target_link_libraries(echo-drift PRIVATE sfml-graphics sfml-window sfml-system)

add_custom_command(TARGET echo-drift POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)