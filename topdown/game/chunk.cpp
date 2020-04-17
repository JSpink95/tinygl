
#include "game/chunk.h"

#include "maths/maths.h"
#include "gl/buffer.h"
#include "gl/vertex_array.h"

#include "glm/gtc/noise.hpp"

#include <vector>

struct chunk_vertex
{
    float3 position;
    float2 texcoord;
    float textureId;
};

/**


chunk layout X-Z
   _ _ _ _
. |_|_|_|_|
^ |_|_|_|_|
| |_|_|_|_|
z |_|_|_|_|
 x ->...



*/

namespace gf
{
    const chunk_config chunk_config::defaultConfig = { 32, 8, 32 };

    unsigned int getChunkSizeFromConfig(const chunk_config& config)
    {
        return config.width * config.height * config.depth;
    }

    chunk* CreateChunk(const chunk_config& config)
    {
        // make cells inactive by default
        return CreateChunk(config, [](unsigned int x, unsigned int y, unsigned int z, bool& active) -> void { active = false; });
    }

    chunk* CreateChunk(const chunk_config& config, chunkCellInitialiser initialiser)
    {
        chunk* result = new chunk{ config.width, config.height, config.depth, nullptr };
        result->cells = new bool[getChunkSizeFromConfig(config)];

        forEachCellInChunk(result, initialiser);

        return result;
    }
    
    chunk* CreateChunkFromNoise(const chunk_config& config)
    {
        auto initialiser = [](unsigned int x, unsigned int y, unsigned int z, bool& active) -> void
        {
            static float scale = 0.01f;
            float sx = scale * (float)x;
            float sz = scale * (float)z;
            float sy = scale * (float)y;
            float height = math::perlin(float3(sx, sz, 0.0f)) * 0.5f + 0.5f;

            active = sy < height;
        };

        return CreateChunk(config, initialiser);
    }

    void DestroyChunk(chunk* c)
    {
        if (c)
        {
            delete c;
        }
    }

    chunk_mesh* CreateChunkMesh(chunk* c)
    {
        std::vector<chunk_vertex> vertices;
        auto buildMesh = [&vertices](unsigned int x, unsigned int y, unsigned int z, bool active) -> void
        {
            if (active)
            {
                vertices.push_back({ float3((float)x + 0.0f, (float)y + 0.5f, (float)z + 0.0f), float2(0.0f, 0.0f), 0.0f });
                vertices.push_back({ float3((float)x + 1.0f, (float)y + 0.5f, (float)z + 0.0f), float2(1.0f, 0.0f), 0.0f });
                vertices.push_back({ float3((float)x + 1.0f, (float)y + 0.5f, (float)z + 1.0f), float2(1.0f, 1.0f), 0.0f });

                vertices.push_back({ float3((float)x + 0.0f, (float)y + 0.5f, (float)z + 0.0f), float2(0.0f, 0.0f), 1.0f });
                vertices.push_back({ float3((float)x + 1.0f, (float)y + 0.5f, (float)z + 1.0f), float2(1.0f, 1.0f), 1.0f });
                vertices.push_back({ float3((float)x + 0.0f, (float)y + 0.5f, (float)z + 1.0f), float2(0.0f, 1.0f), 1.0f });
            }
        };

        forEachCellInChunk(c, buildMesh);

        if (vertices.size() > 0)
        {
            chunk_mesh* mesh = gl::CreateVertexArray();
            mesh->vertexCount = vertices.size();

            gl::buffer* vertexBuffer = gl::CreateVertexBuffer(sizeof(chunk_vertex) * vertices.size(), &vertices.at(0));
            gl::SetIndexBuffer(mesh, nullptr, 0);
            gl::SetVertexBuffer(mesh, vertexBuffer, 0);
            gl::EnableVertexAttribute(mesh, 0, 0, 3, sizeof(chunk_vertex), offsetof(chunk_vertex, position));
            gl::EnableVertexAttribute(mesh, 0, 1, 2, sizeof(chunk_vertex), offsetof(chunk_vertex, texcoord));
            gl::EnableVertexAttribute(mesh, 0, 2, 1, sizeof(chunk_vertex), offsetof(chunk_vertex, textureId));

            return mesh;
        }

        return nullptr;
    }

    void DestroyChunkMesh(chunk_mesh* mesh)
    {
        if (mesh != nullptr)
        {
            gl::DestroyVertexArray(mesh);
        }
    }

    unsigned int getChunkCellCount(chunk* c)
    {
        return c->width * c->height * c->depth;
    }

    void convertChunkIndexToXYZ(chunk* c, unsigned int idx, unsigned int &x, unsigned int &y, unsigned int &z)
    {
        x = idx % c->width;
        z = idx / c->width;
        y = idx / (c->width * c->depth);
    }

    void convertChunkXYZToIndex(chunk* c, unsigned int x, unsigned int y, unsigned int z, unsigned int &idx)
    {
    }
}