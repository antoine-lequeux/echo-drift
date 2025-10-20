#pragma once

#include "../Animation.hpp"
#include "CSprite.hpp"

// Component representing an animated sprite, keeping the same functionalities as a normal Sprite Component, 
// but updating the texture using data from an Animation object.
class CAnimatedSprite : public CSprite
{
public:

    CAnimatedSprite(GameObject& gameObject) : CSprite(gameObject) {}

    void update(Context& ctx) override
    {
        if (!playing || !currentAnim)
            return;

        elapsed += ctx.dt;

        // Update the current texture rect if enough time has passed.
        if (elapsed >= currentAnim->frameTime)
        {
            elapsed -= currentAnim->frameTime;
            currentFrame++;

            if (currentFrame >= currentAnim->frames.size())
            {
                if (currentAnim->loop)
                    currentFrame = 0;
                else
                {
                    playing = false;
                    return;
                }
            }

            setTextureRect(currentAnim->frames[currentFrame]);
        }

        // Use the same render method as in CSprite.
        render(ctx.window);
    }

    void setAnimation(std::shared_ptr<Animation> anim) { currentAnim = anim; }

    void play()
    {
        setTexture(currentAnim->texture);
        setTextureRect(currentAnim->frames[0]);
        currentFrame = 0;
        elapsed = 0.f;
        playing = true;
    }

    void setState(bool isPlaying) { playing = isPlaying; }

private:

    std::shared_ptr<Animation> currentAnim;
    std::size_t currentFrame = 0;
    float elapsed = 0.f;
    bool playing = false;
};