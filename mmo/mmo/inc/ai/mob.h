#pragma once

#include "maths/maths.h"

#include <queue>

namespace gl
{
    struct texture;
    struct program;
    struct vertex_array;
}

struct mob_t;
struct ai_task;
struct camera_t;
struct chunk_manager;

struct ai_behaviour {
    ai_task* currentTask = nullptr;
    std::queue<ai_task*> tasks;
};

bool HasPendingTasks(ai_behaviour* behaviour);
bool HasActiveTask(ai_behaviour* behaviour);
void UpdateBehaviour(ai_behaviour* behaviour, float dt);

struct ai_task {
    bool finished = false;
    virtual void update(float dt) = 0;
};

struct ai_wait: public ai_task {
    float timeToWait = 1.0f;

    ai_wait(const float time);
    virtual void update(float dt) override;
};

struct ai_moveto: public ai_task {
    mob_t* mob = nullptr;

    // target is in the XZ plane, as we're moving on a heightmap
    float2 target = float2(0.0f);
    float acceptedDistance = 1.0f;

    ai_moveto(mob_t* mob, const float2& target, float accepetedDistance = 1.0f);
    virtual void update(float dt) override;
};

struct mob_t {
    chunk_manager* cm = nullptr;
    float3 position;
    float gravityAcceleration = 0.0f;

    // ai
    ai_behaviour* behaviour = nullptr;

    // render data
    gl::vertex_array* mesh = nullptr;
    gl::texture* sprite = nullptr;
};

namespace tg
{
    mob_t* CreateMob(chunk_manager* cm, gl::texture* texture, const float3& position);
    void DestroyMob(mob_t* mob);

    void UpdateMob(mob_t* mob, float dt);
    void SubmitDrawBatch(mob_t* mob, camera_t* camera, gl::program* shader);
}
