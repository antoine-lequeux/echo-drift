cmake_minimum_required(VERSION 3.16)
project(echo-drift LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG master
)
FetchContent_MakeAvailable(SFML)

add_executable(echo-drift 
    main.cpp
    src/SpaceShip.hpp
    src/SpaceShip.cpp
    src/Input.hpp
    src/Input.cpp
    src/Application.hpp
    src/Application.cpp
    src/GameObject.hpp
    src/GameObject.cpp
    src/Projectile.hpp
    src/Projectile.cpp
    src/GameManager.hpp
    src/GameManager.cpp
    src/Background.hpp
    src/Background.cpp
)
target_link_libraries(echo-drift PRIVATE sfml-graphics sfml-window sfml-system)