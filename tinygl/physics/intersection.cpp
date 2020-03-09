
#include "physics/intersection.h"

namespace physics
{
    float pointToPlaneDistance(const float3& point, const float4& plane)
    {
        // making the assumption that plane.xyz is normalised
        return math::dot(float3(plane), point - (float3(plane) * plane.a));
    }
}