
#include "ai/mob.h"

#include "engine/random.h"

#include "game/camera.h"
#include "game/chunk_manager.h"

#include "glm/gtx/norm.hpp"

#include "gl/shader.h"
#include "gl/vertex_array.h"

#include "glfw/glfw.h"

ai_wait::ai_wait(const float time) {
    timeToWait = time;
}

void ai_wait::update(float dt) {
    timeToWait -= dt;

    if (timeToWait < 0.0f) {
        finished = true;
    }
}

ai_moveto::ai_moveto(mob_t* owner, const float2& newTarget, float newAcceptedDistance/* = 1.0f*/) {
    mob = owner;
    target = newTarget;
    acceptedDistance = newAcceptedDistance;
}

void ai_moveto::update(float dt) {
    float2 directionToTarget = glm::normalize(target - float2(mob->position.x, mob->position.z));
    mob->position.x += directionToTarget.x * 2.0f * dt;
    mob->position.z += directionToTarget.y * 2.0f * dt;

    // get the height! we need the chunk manager here though...
    mob->position.y = tgl::GetBarycentricHeightAtWorldPosition(mob->cm, mob->position);

    float distanceToTarget = glm::distance2(target, float2(mob->position.x, mob->position.z));
    if (distanceToTarget <= (acceptedDistance * acceptedDistance)) {
        finished = true;
    }
}

bool HasPendingTasks(ai_behaviour* behaviour)
{
    return behaviour->tasks.size() > 0u;
}

bool HasActiveTask(ai_behaviour* behaviour)
{
    return behaviour->currentTask != nullptr;
}

void UpdateBehaviour(ai_behaviour* behaviour, float dt)
{
    if (behaviour->currentTask == nullptr && behaviour->tasks.size() > 0u) {
        behaviour->currentTask = behaviour->tasks.front();
        behaviour->tasks.pop();
    }

    if (behaviour->currentTask) {
        behaviour->currentTask->update(dt);

        // remove task
        if (behaviour->currentTask->finished) {
            delete behaviour->currentTask;
            behaviour->currentTask = nullptr;
        }
    }
}

namespace tg
{
    mob_t* CreateMob(chunk_manager* cm, gl::texture* texture, const float3& position)
    {
        mob_t* mob = new mob_t;
        mob->cm = cm;
        mob->sprite = texture;
        mob->position = position;

        mob->behaviour = new ai_behaviour;

        return mob;
    }

    void DestroyMob(mob_t* mob)
    {
        if (mob)
        {
            delete mob->behaviour;
            delete mob;
        }
    }

    void UpdateMob(mob_t* mob, float dt)
    {
        UpdateBehaviour(mob->behaviour, dt);

        if (!HasActiveTask(mob->behaviour) && !HasPendingTasks(mob->behaviour)) {

            // first wait for two seconds
            mob->behaviour->tasks.push(new ai_wait(2.0f));

            // then move somewhere new (within some box around the current location)
            float2 target = float2(eng::random::next_float(-10.0f, 10.0f), eng::random::next_float(-10.0f, 10.0f)) + float2(mob->position.x, mob->position.z);
            mob->behaviour->tasks.push(new ai_moveto(mob, target, 1.5f));
        }
    }

    void SubmitDrawBatch(mob_t* mob, camera_t* camera, gl::program* shader)
    {
        if (mob->mesh) {
            gl::BindProgram(shader);

            glm::mat4 transform = glm::mat4(1.0f);
            transform *= glm::translate(glm::mat4(1.0f), mob->position);

            float2 directionToCamera = glm::normalize(float2(mob->position.x, mob->position.z) - float2(camera->eye.x, camera->eye.z));
            transform *= glm::rotate(glm::mat4(1.0f), glm::atan(directionToCamera.x, directionToCamera.y), float3(0.0f, 1.0f, 0.0f)); // rotate to player _somehow_

            gl::UploadUniform(shader, "u_w", transform);

            gl::BindVertexArray(mob->mesh);
            gl::DrawVertexArray(mob->mesh, GL_TRIANGLES, 0);
        }
    }
}