#pragma once

#include "Core/Vectors.h"

namespace Utils
{
    template <typename T>
    void Clamp(T& value, T min, T max)
    {
        if (value < min)
            value = min;
        if (value > max)
            value = max;
    }

    float Lerp(float start, float end, float t);
    CoreMath::Vec3 LerpVec3(CoreMath::Vec3 start, CoreMath::Vec3 end, float t);

    enum Direction
    {
        NONE        = 0,
        FORWARD     = 1,
        BACKWARD    = 2,
        UP          = 3,
        DOWN        = 4,
        LEFT        = 5,
        RIGHT       = 6
    };
};