
#include "physics/ray.h"
#include "physics/intersection.h"

namespace physics
{
    ray CreateRay(const float3& origin, const float3& direction)
    {
        return { origin, direction };
    }

    bool rayIntersectsPlane(const ray& r, const float3& pn, float d, float3& hit)
    {
        return intersectRayWithPlane(r.origin, r.direction, pn, d, hit);
    }
}