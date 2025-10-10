#pragma once

#include "GameObject.hpp"

class Background : public GameObject
{
public:

    Background(std::string texturePath, int drawOrder);

private:

    void updateBehavior(float dt, Input& input, GameObjectManager& entityManager) override;
};