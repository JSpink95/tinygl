
#include "game/grass.h"
#include "game/chunk.h"

#include "gl/buffer.h"
#include "gl/vertex_array.h"

#include "engine/random.h"

#include "glfw/glfw.h"

#include <vector>

namespace tg
{
    grass_t* CreateGrass(chunk_t* owner, unsigned int instanceCount)
    {
        grass_t* grass = new grass_t {
            owner, nullptr, instanceCount
        };

        return grass;
    }

    void DestroyGrass(grass_t* grass)
    {
        if (grass)
        {
            delete grass;
        }
    }

    void GenerateMesh(grass_t* grass)
    {
        grass->instances = new grass_instance[grass->instanceCount];
        for (unsigned int instance = 0; instance < grass->instanceCount; ++instance) {
            float x = eng::random::next_float(0.0f, 32.0f) + grass->owner->worldOffset.x;
            float z = eng::random::next_float(0.0f, 32.0f) + grass->owner->worldOffset.z;

            float3 a, b, c;
            float y = tgl::GetBarycentricHeightAtWorldPosition(grass->owner, float3(x, 0.0f, z), a, b, c);

            grass->instances[instance] = { 
                glm::translate(glm::mat4(1.0f), float3(x, y, z))
            };
        }

        grass->reloadMesh = true;

    }

    void UpdateGrass(grass_t* grass, float dt)
    {
        if (grass->reloadMesh)
        {
            grass->reloadMesh = false;

            constexpr const int vertexCount = 8;
            constexpr const int indexCount = 12;

            grass_vertex vertices[vertexCount] = {
                { float3(-0.1f, 0.0f, -0.1f), float2(0.0f, 0.0f) }, { float3(0.1f, 0.0f,  0.1f), float2(1.0f, 0.0f) },  { float3(0.1f, 0.2f,  0.1f), float2(1.0f, 1.0f) }, { float3(-0.1f, 0.2f, -0.1f), float2(0.0f, 1.0f) },
                { float3(-0.1f, 0.0f,  0.1f), float2(0.0f, 0.0f) }, { float3(0.1f, 0.0f, -0.1f), float2(1.0f, 0.0f) },  { float3(0.1f, 0.2f, -0.1f), float2(1.0f, 1.0f) }, { float3(-0.1f, 0.2f,  0.1f), float2(0.0f, 1.0f) },
            };

            unsigned int indices[indexCount] = {
                0, 1, 2,
                0, 2, 3,

                4, 5, 6,
                4, 6, 7,
            };

            // stitch it all together
            gl::buffer* indexBuffer = gl::CreateIndexBuffer(sizeof(unsigned int) * indexCount, indices);
            gl::buffer* vertexBuffer = gl::CreateVertexBuffer(sizeof(grass_vertex) * vertexCount, vertices);
            gl::buffer* instanceBuffer = gl::CreateInstanceBuffer(sizeof(grass_instance) * grass->instanceCount, grass->instances);

            gl::vertex_array* va = gl::CreateVertexArray();
            va->vertexCount = vertexCount;

            gl::SetIndexBuffer(va, indexBuffer, indexCount);
            gl::SetVertexBuffer(va, vertexBuffer, 0);
            gl::SetVertexBuffer(va, instanceBuffer, 1);

            gl::EnableVertexAttribute(va, 0, 0, 3, sizeof(grass_vertex), offsetof(grass_vertex, position));
            gl::EnableVertexAttribute(va, 0, 1, 2, sizeof(grass_vertex), offsetof(grass_vertex, texcoord));

            // instance data
            gl::EnableVertexAttribute(va, 1, 2, 4, sizeof(grass_instance), 0);
            gl::EnableVertexAttribute(va, 1, 3, 4, sizeof(grass_instance), 16);
            gl::EnableVertexAttribute(va, 1, 4, 4, sizeof(grass_instance), 32);
            gl::EnableVertexAttribute(va, 1, 5, 4, sizeof(grass_instance), 48);

            gl::BindVertexArray(va);
            glVertexAttribDivisor(2, 1);    // for every 1 instance, update the buffer at index 1.
            glVertexAttribDivisor(3, 1);    // for every 1 instance, update the buffer at index 1.
            glVertexAttribDivisor(4, 1);    // for every 1 instance, update the buffer at index 1.
            glVertexAttribDivisor(5, 1);    // for every 1 instance, update the buffer at index 1.
            gl::BindVertexArray(0);

            grass->mesh = va;
        }
    }

    void SubmitGrassDrawBatch(grass_t* grass, gl::program* shader)
    {
        glPointSize(100.0f);

        if (grass->mesh)
        {
            glDisable(GL_CULL_FACE);
            gl::BindVertexArray(grass->mesh);
            gl::DrawVertexArrayInstanced(grass->mesh, GL_TRIANGLES, 0, grass->instanceCount);
            glEnable(GL_CULL_FACE);
        }
    }
}