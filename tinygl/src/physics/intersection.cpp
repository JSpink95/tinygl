
#include "physics/intersection.h"

namespace physics
{
    float pointToPlaneDistance(const float3& point, const float4& plane)
    {
        // making the assumption that plane.xyz is normalised
        return math::dot(float3(plane), point - (float3(plane) * plane.a));
    }

    bool intersectRayWithPlane(const float3& p, const float3& v, const float3& n, float d, float3& hit)
    {
        float denom = math::dot(n, v);

        // Prevent divide by zero:
        if (math::abs(denom) <= 1e-4f)
            return false;

        // If you want to ensure the ray reflects off only
        // the "top" half of the plane, use this instead:
        if (-denom <= 1e-4f)
            return false;

        float t = -(math::dot(n, p) + d) / math::dot(n, v);

        // Use pointy end of the ray.
        // It is technically correct to compare t < 0,
        // but that may be undesirable in a raytracer.
        if (t <= 1e-4)
            return false;

        hit = p + t * v;
        return true;
    }
}