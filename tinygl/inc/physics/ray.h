
#pragma once

#include "maths/maths.h"

namespace physics
{
    struct ray
    {
        float3 origin, direction;
    };

    ray CreateRay(const float3& origin, const float3& direction);

    // intersection helpers
    bool rayIntersectsPlane(const ray& r, const float3& pn, float d, float3& hit);
}