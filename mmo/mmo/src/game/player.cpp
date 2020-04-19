
#include "game/player.h"
#include "game/camera.h"

#include "game/chunk.h"
#include "game/chunk_manager.h"

#include "engine/input.h"

#include "gl/window.h"
#include "glfw/glfw.h"

player_t* CreatePlayer()
{
    player_t* player = new player_t;

    player->camera = new camera_t;
    player->camera->fov = 70.0f;
    
    return player;
}

void DestroyPlayer(player_t* player)
{
    if (player)
    {
        delete player->camera;
        delete player;
    }
}

void pHandleInput(player_t* player, chunk_manager* cm, glfw::window* window, float dt)
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

    bool bSpacePressed = false;
    bool bSpaceReleased = false;

    QueryKey(GLFW_KEY_SPACE, bSpacePressed, bSpaceReleased);

    if (bSpacePressed && player->grounded)
    {
        player->gravityAcceleration = 5.0f;
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
        input_t::instance->cursorMode = cursor_mode::free;
    }

    if (bAltJustReleased)
    {
        glfw::SetCursorVisible(window, false);
        input_t::instance->cursorMode = cursor_mode::locked;
    }

    float2 cursorMove = GetCursorDelta();
    player->camera->yaw += cursorMove.x * rotateSpeed * dt;
    player->camera->pitch += -cursorMove.y * rotateSpeed * dt;

    float3 forwardDirection = cGetFlatForward(player->camera);
    float3 strafeVector = glm::normalize(glm::cross(forwardDirection, float3(0.0f, 1.0f, 0.0f)));

    player->position += (forwardDirection * forward + strafeVector * strafe) * speed * movementModifier * dt;
    player->camera->pitch = glm::clamp(player->camera->pitch, -89.0f, 89.0f);

    player->gravityAcceleration += -9.81f * dt;
    player->position.y += player->gravityAcceleration * dt;

    // make sure we don't go below the terrain
    
    float3 a, b, c;
    float height = tgl::GetBarycentricHeightAtWorldPosition(cm, player->position);

    if (player->position.y < height)
    {
        player->gravityAcceleration = 0.0f;
        player->position.y = height;
    }

    player->grounded = ((player->position.y - 0.1f) <= height);

    // finally, set the camera.
    player->camera->eye = player->position + float3(0.0f, 1.0f, 0.0f);
}
