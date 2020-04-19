#pragma once

#include "maths/maths.h"

struct camera_t;
struct chunk_manager;

namespace glfw
{
    struct window;
}

struct player_t
{
    camera_t* camera = nullptr;
    float3 position = float3(0.0f);
    float gravityAcceleration = 0.0f;
    bool grounded = false;
};

player_t* CreatePlayer();
void DestroyPlayer(player_t* player);

void pHandleInput(player_t* player, chunk_manager* cm, glfw::window* window, float dt);
