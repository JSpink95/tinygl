
#include "game/chunk.h"
#include "game/chunk_manager.h"
#include "game/camera.h"
#include "game/grass.h"

#include "engine/random.h"

#include "gl/shader.h"
#include "gl/buffer.h"
#include "gl/vertex_array.h"
#include "gl/texture.h"

#include "glfw/glfw.h"

#include "glm/gtx/norm.hpp"

#include "utility/gradient.h"

#include <array>
#include <vector>

gradient* g = CreateGradient();

namespace tgl
{
    chunk_t* CreateChunk(chunk_manager* cm, const float2& offset)
    {
        // add extra padding for surrounding vertex info
        chunk_t* result = new chunk_t {
            cm,
            float3(offset.x, 0.0f, offset.y) * cm->chunkConfig.cellSize * (float)cm->chunkConfig.size,
            new float3[(cm->chunkConfig.size + 1) * (cm->chunkConfig.size + 1)]
        };

        //result->grass = tg::CreateGrass(result, 1000000); // one million :D
        result->grass = tg::CreateGrass(result, 10000); // ten thousand is fine though :(

        return result;
    }

    void DestroyChunk(chunk_t* chunk)
    {
        if (chunk)
        {
            delete[] chunk->positions;
            gl::DestroyVertexArray(chunk->heightmap);
            tg::DestroyGrass(chunk->grass);

            delete chunk;
        }
    }

    void GetChunkCornerPoints(chunk_t* chunk, float3 points[4])
    {
        const chunk_config& config = chunk->owner->chunkConfig;
        const float chunkWorldSize = config.size * config.cellSize;

        points[0] = float3(chunk->worldOffset.x + 0.0f          , 0.0f, chunk->worldOffset.z + 0.0f             );
        points[1] = float3(chunk->worldOffset.x + chunkWorldSize, 0.0f, chunk->worldOffset.z + 0.0f             );
        points[2] = float3(chunk->worldOffset.x + chunkWorldSize, 0.0f, chunk->worldOffset.z + chunkWorldSize   );
        points[3] = float3(chunk->worldOffset.x + 0.0f          , 0.0f, chunk->worldOffset.z + chunkWorldSize   );
    }

    float GetHeightAt(chunk_t* chunk, int x, int z)
    {
        const int index = GetIndexFromCoord(chunk->owner->chunkConfig, x, z);
        return chunk->positions[index].y;
    }

    float barycentric(const float3& p1, const float3& p2, const float3& p3, const float3& pos)
    {
        float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
        float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.z - p3.z)) / det;
        float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.z - p3.z)) / det;
        float l3 = 1.0f - l1 - l2;
        return l1 * p1.y + l2 * p2.y + l3 * p3.y;
    }

    float GetBarycentricHeightAtWorldPosition(chunk_t* chunk, const float3& wp, float3& oa, float3& ob, float3& oc)
    {
        // quad layout
        // 1,1--0,1
        //  |  / |
        //  | /  |
        // 1,0--0,0
        //
        // c--d
        // | /|
        // |/ |
        // b--a

        float3 lp = wp - chunk->worldOffset;

        // find quad
        int lx = (int)glm::floor(lp.x);
        int lz = (int)glm::floor(lp.z);

        float3 a = chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, lx + 0, lz + 0)];
        float3 b = chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, lx + 1, lz + 0)];
        float3 c = chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, lx + 1, lz + 1)];
        float3 d = chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, lx + 0, lz + 1)];

        // determine triangle
        float ltx = lp.x - (float)lx;
        float ltz = lp.z - (float)lz;

        // triangle line; x = 1 - z
        // x < (1 - z); use a,b,d
        // else; use b,c,d

        if (ltx <= (1.0f - ltz)) {
            oa = chunk->worldOffset + a;
            ob = chunk->worldOffset + b;
            oc = chunk->worldOffset + d;
            return barycentric(a, b, d, lp);
        }
        else {
            oa = chunk->worldOffset + b;
            ob = chunk->worldOffset + c;
            oc = chunk->worldOffset + d;
            return barycentric(b, c, d, lp);
        }
    }

    void GenerateHeights(chunk_t* chunk, const perlin_config& perlinConfig)
    {
        chunk->maxHeight = -FLT_MAX;
        chunk->minHeight = FLT_MAX;

        for (unsigned int x = 0; x < (chunk->owner->chunkConfig.size + 1); ++x) {
            for (unsigned int z = 0; z < (chunk->owner->chunkConfig.size + 1); ++z) {
                const int index = GetIndexFromCoord(chunk->owner->chunkConfig, x, z);

                float2 localOffset = float2((float)x, (float)z);
                chunk->positions[index] = float3(localOffset.x, pnNoise(perlinConfig, float2(chunk->worldOffset.x, chunk->worldOffset.z) + localOffset), localOffset.y);

                chunk->maxHeight = glm::max(chunk->positions[index].y, chunk->maxHeight);
                chunk->minHeight = glm::min(chunk->positions[index].y, chunk->minHeight);
            }
        }
    }

    void GenerateMesh(chunk_t* chunk)
    {
        float4 lowColor = float4(0.4f, 0.99f, 0.5f, 1.0f);
        float4 highColor = float4(1.0f);

        // 3 vertices per triangle, 2 triangles per quad
        chunk->vertexCount = chunk->owner->chunkConfig.size * chunk->owner->chunkConfig.size * 3 * 2;
        chunk->vertices = new chunk_vertex[chunk->vertexCount];
        int vertexIndex = 0;
        for (unsigned int x = 0; x < chunk->owner->chunkConfig.size; ++x) {
            for (unsigned int z = 0; z < chunk->owner->chunkConfig.size; ++z) {

                // quad layout
                // 1,1--0,1
                //  |  / |
                //  | /  |
                // 1,0--0,0

                float3 positions[6] = {
                    chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, x + 0, z + 0)],
                    chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, x + 0, z + 1)],
                    chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, x + 1, z + 0)],

                    chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, x + 1, z + 0)],
                    chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, x + 0, z + 1)],
                    chunk->positions[GetIndexFromCoord(chunk->owner->chunkConfig, x + 1, z + 1)],
                };

                for (int vindex = 0; vindex < 6; ++vindex) {
                    chunk->vertices[vertexIndex + vindex].position = positions[vindex];
                    chunk->vertices[vertexIndex + vindex].color = glm::mix(lowColor, highColor, positions[vindex].y / 16.0f);// GetInterpolatedValue(g, positions[vindex].y / 16.0f);
                }

                vertexIndex += 6;
            }
        }

        tg::GenerateMesh(chunk->grass);
        chunk->reloadMesh = true;
    }

    void UpdateTerrainChunk(chunk_t* chunk)
    {
        tg::UpdateGrass(chunk->grass, 0.0f);
        if (chunk->reloadMesh)
        {
            chunk->reloadMesh = false;

            gl::buffer* vertexBuffer = nullptr;
            gl::vertex_array* vertexArray = chunk->heightmap ? chunk->heightmap : gl::CreateVertexArray();
            vertexArray->vertexCount = chunk->vertexCount;

            if (chunk->heightmap && chunk->heightmap->buffers[0])
            {
                vertexBuffer = chunk->heightmap->buffers[0];
            }
            else
            {
                vertexBuffer = gl::CreateVertexBuffer(chunk->vertexCount * sizeof(chunk_vertex), chunk->vertices);
            }

            gl::SetIndexBuffer(vertexArray, nullptr, 0);
            gl::SetVertexBuffer(vertexArray, vertexBuffer, 0);

            // enable our positional & color data
            gl::EnableVertexAttribute(vertexArray, 0, 0, 3, sizeof(chunk_vertex), offsetof(chunk_vertex, position));
            gl::EnableVertexAttribute(vertexArray, 0, 1, 4, sizeof(chunk_vertex), offsetof(chunk_vertex, color));

            chunk->heightmap = vertexArray;
        }
    }

}