#pragma once

#include "maths/maths.h"

namespace glfw
{
    struct window;
}

struct camera
{
    float3 eye;
    float yaw = 0.0f, pitch = 0.0f;
    float fov = 60.0f;
};

float3 cGetForward(camera* cam);
float3 cGetFlatForward(camera* cam);
glm::mat4 cGetViewMatrix(camera* cam);
glm::mat4 cGetProjectionMatrix(camera* cam, float sw, float sh);

void cHandleInput(camera* cam, glfw::window* window, float dt);
