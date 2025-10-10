#include "Background.hpp"

Background::Background(std::string texturePath, int drawOrder) : GameObject(texturePath, drawOrder) 
{
    setScale({2.f, 2.f});
}

void Background::updateBehavior(float dt, Input& input, GameObjectManager& entityManager) {}
