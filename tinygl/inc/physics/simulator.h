
#pragma once

#include "maths/maths.h"

namespace physics
{
    struct state
    {
        float3 p;
        float3 v;
    };

    struct derivative
    {
        float3 dp;
        float3 dv;
    };

    void integrate(state& st, double t, float dt);
}
