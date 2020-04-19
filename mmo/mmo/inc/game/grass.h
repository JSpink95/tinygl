#pragma once

#include "maths/maths.h"

struct chunk_t;

namespace gl
{
    struct vertex_array;
    struct program;
}

struct grass_vertex {
    float3 position;
    float2 texcoord;
};

struct grass_instance {
    glm::mat4 transform;
};

struct grass_t {
    chunk_t* owner = nullptr;
    gl::vertex_array* mesh = nullptr;
    unsigned int instanceCount = 0u;

    bool reloadMesh = false;
    grass_instance* instances = nullptr;
};

namespace tg
{
    grass_t* CreateGrass(chunk_t* owner, unsigned int instanceCount);
    void DestroyGrass(grass_t* grass);

    void GenerateMesh(grass_t* grass);

    void UpdateGrass(grass_t* grass, float dt);
    void SubmitGrassDrawBatch(grass_t* grass, gl::program* shader);
}
