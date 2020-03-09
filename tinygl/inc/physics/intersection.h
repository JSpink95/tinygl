
#pragma once

#include "maths/maths.h"

namespace physics
{
    float pointToPlaneDistance(const float3& point, const float4& plane);

    bool intersectRayWithPlane(
        const float3& p, const float3& v,   // ray
        const float3& n, float d,           // plane
        float3& hit                         // hit point
    );
}