#pragma once

#include "../SpriteEntity/spriteentity.h"
#include <map>
#include <vector>

struct Frame {
    int x, y;
    int width, height;
};

struct AnimationData_t {
    std::vector<Frame> frames;
    double frameInterval;
};

class AnimatedEntity: public SpriteEntity {
    protected:
    std::map<std::string, AnimationData_t> animations;
    std::string currentAnimation = "idle";
    int currentFrame = 0;
    double timeCount = 0.0;

    public:
    virtual void InitializeAnimations() {
        SDL_Log("Implement animations!");
    }

    virtual void UpdateAnimation(double dt, std::string newAnimation) {
        timeCount += dt;
        
        if (newAnimation != currentAnimation) {
            currentAnimation = newAnimation;
            currentFrame = 0;
            timeCount = 0.0;
        }
        
        AnimationData_t& anim = animations[currentAnimation];
        
        if (timeCount >= anim.frameInterval) {
            timeCount = 0.0;
            currentFrame++;
            if (currentFrame >= anim.frames.size()) {
                currentFrame = 0;
            }

            //SDL_Log("Current frame: %i", currentFrame);
            
            Frame& frame = anim.frames[currentFrame];
            SetRegion(frame.x, frame.y, frame.width, frame.height);
        }
    }
};