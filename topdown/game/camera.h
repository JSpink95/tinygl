
#pragma once

#include "maths/maths.h"

namespace glfw
{
    struct window;
}

namespace physics
{
    struct ray;
}

namespace gf
{
    struct camera
    {
        float fov;
        float znear, zfar;
        float3 eye, target;
        float3 up;
    };

    camera* CreateCamera(float fov, const float3& eye, const float3& target, const float3& up = float3(0.0f, 1.0f, 0.0f));
    void DestroyCamera(camera* cam);

    float4x4 getViewMatrix(camera* cam);
    float4x4 getProjectionMatrix(camera* cam, float w, float h);
    float4x4 getProjectionMatrix(glfw::window* window, camera* cam);
    float3 getViewDirection(camera* cam);

    physics::ray rayFromCamera(glfw::window* window, camera* cam);

    void move(camera* cam, const float3& amount, bool eyeOnly);
}