
#include "game/camera.h"

#include "gl/window.h"
#include "physics/ray.h"

namespace gf
{
    camera* CreateCamera(float fov, const float3& eye, const float3& target, const float3& up/* = float3(0.0f, 1.0f, 0.0f)*/)
    {
        camera* result = new camera;

        result->fov = fov;
        result->eye = eye;
        result->target = target;
        result->up = up;
        result->zfar = 0.01f;
        result->znear = 100.0f;

        return result;
    }

    void DestroyCamera(camera* cam)
    {
        if (cam)
        {
            delete cam;
        }
    }

    float4x4 getViewMatrix(camera* cam)
    {
        return math::lookAt<float>(cam->eye, cam->target, cam->up);
    }

    float4x4 getProjectionMatrix(camera* cam, float w, float h)
    {
        return math::perspectiveFov<float>(math::radians(cam->fov), w, h, cam->znear, cam->zfar);
    }

    float4x4 getProjectionMatrix(glfw::window* window, camera* cam)
    {
        int w, h;
        glfw::GetWindowSize(window, w, h);

        return getProjectionMatrix(cam, (float)w, (float)h);
    }

    float3 getViewDirection(camera* cam)
    {
        return math::normalize(cam->target - cam->eye);
    }

    physics::ray rayFromCamera(glfw::window* window, camera* cam)
    {
        int w, h;
        glfw::GetWindowSize(window, w, h);

        float mx, my;
        glfw::GetMousePosition(window, mx, my);

        mx = ((mx / (float)w) * 2.0f - 1.0f) * 0.5f;
        my = ((my / (float)h) * 2.0f - 1.0f) * 0.5f;
        my *= -1.0f;

        float4x4 proj = getProjectionMatrix(cam, (float)w, (float)h);
        float4x4 view = getViewMatrix(cam);

        float4x4 invVP = math::inverse(proj * view);
        float4 screenPos = float4(mx, my, 1.0f, 1.0f);
        float4 worldPos = invVP * screenPos;

        physics::ray ray;
        ray.origin = cam->eye;
        ray.direction = math::normalize(float3(worldPos));

        return ray;
    }

    void move(camera* cam, const float3& amount, bool eyeOnly)
    {
        cam->eye += amount;
        if (!eyeOnly)
        {
            cam->target += amount;
        }
    }
}