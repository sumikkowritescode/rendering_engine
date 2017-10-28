#include "time.h"
#include <SDL.h>

namespace Framework {
    Time::Time() :
        deltaTime(0.0f),
        lastFrame(0.0f)
    {
    }

    void Time::Update() {
        float currentFrame = SDL_GetTicks() * 0.001f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
}
