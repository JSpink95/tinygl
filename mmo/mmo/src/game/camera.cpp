
#include "game/camera.h"
#include "engine/input.h"

#include "glfw/glfw.h"
#include "gl/window.h"

float3 cGetForward(camera_t* camera)
{
    glm::quat rot = glm::quat(glm::radians(float3(camera->pitch, camera->yaw, 0.0f)));
    return glm::normalize(rot * float3(0.0f, 0.0f, 1.0f));
}

float3 cGetFlatForward(camera_t* camera)
{
    glm::quat rot = glm::quat(glm::radians(float3(0.0f, camera->yaw, 0.0f)));
    return glm::normalize(rot * float3(0.0f, 0.0f, 1.0f));
}

glm::mat4 cGetViewMatrix(camera_t* camera)
{
    float3 direction = cGetForward(camera);
    return glm::lookAt(camera->eye, camera->eye + direction, float3(0.0f, 1.0f, 0.0f));
}

glm::mat4 cGetProjectionMatrix(camera_t* camera, float sw, float sh)
{
    return glm::perspectiveFov(glm::radians(camera->fov), sw, sh, 0.01f, 1000.0f);
}
