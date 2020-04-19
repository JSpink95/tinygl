#pragma once

#include "maths/maths.h"

namespace glfw
{
    struct window;
}

struct camera_t
{
    float3 eye;
    float yaw = 0.0f, pitch = 0.0f;
    float fov = 60.0f;
};

float3 cGetForward(camera_t* cam);
float3 cGetFlatForward(camera_t* cam);
glm::mat4 cGetViewMatrix(camera_t* cam);
glm::mat4 cGetProjectionMatrix(camera_t* cam, float sw, float sh);
