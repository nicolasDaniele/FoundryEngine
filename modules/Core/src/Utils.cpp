#include "Core/Utils.h"

namespace Utils
{
    float Lerp(float start, float end, float t)
    {
        return start + t * (end - start);
    }

    CoreMath::Vec3 LerpVec3(CoreMath::Vec3 start, CoreMath::Vec3 end, float t)
    {
        return CoreMath::Vec3(
            Lerp(start.x, end.x, t),
            Lerp(start.y, end.y, t),
            Lerp(start.z, end.z, t)
        );
    }
}