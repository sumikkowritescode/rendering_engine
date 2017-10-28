#include "random_generator.h"
#include <random>

namespace Utils
{
    float GetRandomFloat(int min, int max) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> distr(static_cast<float>(min), static_cast<float>(max));
    
        return distr(rng);
    }
    
    float GetRandomFloat(float min, float max) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<float> distr(min, max);
    
        return distr(rng);
    }
}
