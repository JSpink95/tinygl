
#include "game/camera.h"
#include "engine/input.h"

#include "glfw/glfw.h"
#include "gl/window.h"

float3 cGetForward(camera* cam)
{
    glm::quat rot = glm::quat(glm::radians(float3(cam->pitch, cam->yaw, 0.0f)));
    return glm::normalize(rot * float3(0.0f, 0.0f, 1.0f));
}

float3 cGetFlatForward(camera* cam)
{
    glm::quat rot = glm::quat(glm::radians(float3(0.0f, cam->yaw, 0.0f)));
    return glm::normalize(rot * float3(0.0f, 0.0f, 1.0f));
}

glm::mat4 cGetViewMatrix(camera* cam)
{
    float3 direction = cGetForward(cam);
    return glm::lookAt(cam->eye, cam->eye + direction, float3(0.0f, 1.0f, 0.0f));
}

glm::mat4 cGetProjectionMatrix(camera* cam, float sw, float sh)
{
    return glm::perspectiveFov(glm::radians(cam->fov), sw, sh, 0.01f, 1000.0f);
}

void cHandleInput(camera* cam, glfw::window* window, float dt)
{
    float speed = 4.0f;
    float rotateSpeed = 10.0f;

    float strafe = 0.0f;
    float forward = 0.0f;

    float movementModifier = 1.0f;

    if (IsKeyDown(GLFW_KEY_A))
    {
        strafe -= 1.0f;
    }

    if (IsKeyDown(GLFW_KEY_D))
    {
        strafe += 1.0f;
    }

    if (IsKeyDown(GLFW_KEY_W))
    {
        forward += 1.0f;
    }

    if (IsKeyDown(GLFW_KEY_S))
    {
        forward -= 1.0f;
    }

    if (IsKeyDown(GLFW_KEY_LEFT_CONTROL))
    {
        movementModifier = 0.1f;
    }

    bool bAltJustPressed = false;
    bool bAltJustReleased = false;

    QueryKey(GLFW_KEY_LEFT_ALT, bAltJustPressed, bAltJustReleased);

    if (bAltJustPressed)
    {
        glfw::SetCursorVisible(window, true);
        input::instance->cursorMode = cursor_mode::free;
    }
    
    if (bAltJustReleased)
    {
        glfw::SetCursorVisible(window, false);
        input::instance->cursorMode = cursor_mode::locked;
    }

    float2 cursorMove = GetCursorDelta();
    cam->yaw += cursorMove.x * rotateSpeed * dt;
    cam->pitch += -cursorMove.y * rotateSpeed * dt;

    float3 forwardDirection = cGetFlatForward(cam);
    float3 strafeVector = glm::normalize(glm::cross(forwardDirection, float3(0.0f, 1.0f, 0.0f)));

    cam->eye += (forwardDirection * forward + strafeVector * strafe) * speed * movementModifier * dt;
    cam->pitch = glm::clamp(cam->pitch, -89.0f, 89.0f);
}
