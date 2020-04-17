
#include "game/terrain.h"
#include "game/camera.h"

#include "gl/shader.h"
#include "gl/buffer.h"
#include "gl/vertex_array.h"

#include "glfw/glfw.h"

#include "glm/gtx/norm.hpp"

#include <vector>

const terrain_chunk_config terrain_chunk_config::defaultConfig = {
    32, 1.0f
};

terrain_chunk* CreateTerrainChunk(const terrain_chunk_config& config, const float2& offset)
{
    // add extra padding for surrounding vertex info
    terrain_chunk* result = new terrain_chunk{
        config,
        float3(offset.x, 0.0f, offset.y) * config.cellSize * (float)config.gridSize,
        new float3[(config.gridSize + 1) * (config.gridSize + 1)]
    };

    return result;
}

void DestroyTerrainChunk(terrain_chunk* chunk)
{
    if (chunk)
    {
        delete[] chunk->positions;
        gl::DestroyVertexArray(chunk->mesh.mesh);

        if (chunk->mesh.vertices)
        {
            delete[] chunk->mesh.vertices;
        }

        delete chunk;
    }
}

int tcGetIndexFromCoord(terrain_chunk* chunk, int x, int z)
{
    return x + z * (chunk->config.gridSize + 1);
}

float tcGetHeightAt(terrain_chunk* chunk, int x, int z)
{
    const int index = tcGetIndexFromCoord(chunk, x, z);
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

float tcGetBarycentricHeightAtWorldPosition(terrain_chunk* chunk, const float3& wp, float3& oa, float3& ob, float3& oc)
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

    float3 a = chunk->positions[tcGetIndexFromCoord(chunk, lx + 0, lz + 0)];
    float3 b = chunk->positions[tcGetIndexFromCoord(chunk, lx + 1, lz + 0)];
    float3 c = chunk->positions[tcGetIndexFromCoord(chunk, lx + 1, lz + 1)];
    float3 d = chunk->positions[tcGetIndexFromCoord(chunk, lx + 0, lz + 1)];

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
    } else {
        oa = chunk->worldOffset + b;
        ob = chunk->worldOffset + c;
        oc = chunk->worldOffset + d;
        return barycentric(b, c, d, lp);
    }
}

void tcGenerateHeights(terrain_chunk* chunk, const perlin_config& perlinConfig)
{
    chunk->maxHeight = -FLT_MAX;
    for (int x = 0; x < (chunk->config.gridSize + 1); ++x) {
        for (int z = 0; z < (chunk->config.gridSize + 1); ++z) {
            const int index = tcGetIndexFromCoord(chunk, x, z);

            float2 localOffset = float2((float)x, (float)z);
            chunk->positions[index] = float3(localOffset.x, pnNoise(perlinConfig, float2(chunk->worldOffset.x, chunk->worldOffset.z) + localOffset), localOffset.y);
            chunk->maxHeight = glm::max(chunk->positions[index].y, chunk->maxHeight);
        }
    }
}

void tcGenerateMesh(terrain_chunk* chunk)
{
    float4 lowColor = float4(0.4f, 0.99f, 0.5f, 1.0f);
    float4 highColor = float4(1.0f);

    // 3 vertices per triangle, 2 triangles per quad
    chunk->mesh.vertexCount = chunk->config.gridSize * chunk->config.gridSize * 3 * 2;
    chunk->mesh.vertices = new terrain_vertex[chunk->mesh.vertexCount];
    int vertexIndex = 0;
    for (int x = 0; x < chunk->config.gridSize; ++x) {
        for (int z = 0; z < chunk->config.gridSize; ++z) {

            // quad layout
            // 1,1--0,1
            //  |  / |
            //  | /  |
            // 1,0--0,0
            
            float3 positions[6] = {
                chunk->positions[tcGetIndexFromCoord(chunk, x + 0, z + 0)],
                chunk->positions[tcGetIndexFromCoord(chunk, x + 0, z + 1)],
                chunk->positions[tcGetIndexFromCoord(chunk, x + 1, z + 0)],

                chunk->positions[tcGetIndexFromCoord(chunk, x + 1, z + 0)],
                chunk->positions[tcGetIndexFromCoord(chunk, x + 0, z + 1)],
                chunk->positions[tcGetIndexFromCoord(chunk, x + 1, z + 1)],
            };

            float4 color = float4(0.2f, 0.99f, 0.2f, 1.0f);
            for (int vindex = 0; vindex < 6; ++vindex) {

                if (vindex > 0 && vindex % 3 == 0) {
                    color = float4(0.2f, 0.2f, 0.99f, 1.0f);
                }

                chunk->mesh.vertices[vertexIndex + vindex].position = positions[vindex];
                chunk->mesh.vertices[vertexIndex + vindex].color = color;

            }

            vertexIndex += 6;
        }
    }

    chunk->reloadMesh = true;
}

void tcUpdateTerrainChunk(terrain_chunk* chunk)
{
    if (chunk->reloadMesh)
    {
        chunk->reloadMesh = false;

        gl::buffer* vertexBuffer = chunk->mesh.mesh ? chunk->mesh.mesh->buffers[0] : gl::CreateVertexBuffer(chunk->mesh.vertexCount * sizeof(terrain_vertex), chunk->mesh.vertices);
        gl::vertex_array* vertexArray = chunk->mesh.mesh ? chunk->mesh.mesh : gl::CreateVertexArray();

        vertexArray->vertexCount = chunk->mesh.vertexCount;
        gl::SetIndexBuffer(vertexArray, nullptr, 0);
        gl::SetVertexBuffer(vertexArray, vertexBuffer, 0);

        // enable our positional & color data
        gl::EnableVertexAttribute(vertexArray, 0, 0, 3, sizeof(terrain_vertex), offsetof(terrain_vertex, position));
        gl::EnableVertexAttribute(vertexArray, 0, 1, 4, sizeof(terrain_vertex), offsetof(terrain_vertex, color));

        chunk->mesh.mesh = vertexArray;
    }
}

void Thread_ChunkLoader(terrain_chunk_manager* tcm)
{
    while (tcm->runnning) {

        if (tcm->chunksToLoad.size() > 0) {
            terrain_chunk* chunk = nullptr;

            {
                std::lock_guard<std::mutex> lock(tcm->loadThreadMutex);
                chunk = tcm->chunksToLoad.back();
                tcm->chunksToLoad.pop_back();
            }

            if (chunk)
            {
                tcGenerateHeights(chunk, tcm->perlinConfig);
                tcGenerateMesh(chunk);
                chunk->hasBeenLoaded = true;
            }
        }
    }
}

terrain_chunk_manager* CreateTerrainChunkManager(const terrain_chunk_config& config, const perlin_config& perlinConfig)
{
    terrain_chunk_manager* result = new terrain_chunk_manager{
        config, perlinConfig
    };

    result->runnning = true;
    result->loadThread = std::async(Thread_ChunkLoader, result);

    return result;
}

void DestroyTerrainChunkManager(terrain_chunk_manager* tcm)
{
    if (tcm)
    {
        for (auto kv : tcm->chunks) {
            if (kv.second) {
                DestroyTerrainChunk(kv.second);
            }
        }

        tcm->chunks.clear();
        delete tcm;
    }
}

void tcmAppendChunksToLoad(terrain_chunk_manager* tcm, const float3& wp, const std::vector<terrain_chunk*>& chunks)
{
    if (chunks.size() > 0) {
        std::lock_guard<std::mutex> lock(tcm->loadThreadMutex);
        for (terrain_chunk* chunk: chunks) {
            chunk->hasBeenLoaded = false;
            tcm->chunksToLoad.push_back(chunk);
        }

        std::sort(tcm->chunksToLoad.begin(), tcm->chunksToLoad.end(), [wp](terrain_chunk* a, terrain_chunk* b) -> bool
        {
            float da = glm::distance2(wp, a->worldOffset + float3(16.0f, 0.0f, 16.0f));
            float db = glm::distance2(wp, b->worldOffset + float3(16.0f, 0.0f, 16.0f));

            return db < da;
        });
    }
}

float2 GetWorldOffsetFromChunkId(const chunk_id& id)
{
    return (float2)id * 32.0f;
}

chunk_id GetChunkIdFromWorldPosition(const float3& wp)
{
    chunk_id result;
    result.x = (int)glm::floor(wp.x / 32.0f);
    result.y = (int)glm::floor(wp.z / 32.0f);

    return result;
}

float tcmGetBarycentricHeightAtWorldPosition(terrain_chunk_manager* tcm, const float3& wp)
{
    chunk_id id = GetChunkIdFromWorldPosition(wp);
    auto it = tcm->chunks.find(id);
    if (it != tcm->chunks.end() && it->second->hasBeenLoaded)
    {
        float3 a, b, c;
        return tcGetBarycentricHeightAtWorldPosition(it->second, wp, a, b, c);
    }

    return 0.0f;
}

float tcmGetBarycentricHeightAtWorldPosition(terrain_chunk_manager* tcm, const float3& wp, float3& a, float3& b, float3& c)
{
    chunk_id id = GetChunkIdFromWorldPosition(wp);
    auto it = tcm->chunks.find(id);
    if (it != tcm->chunks.end() && it->second->hasBeenLoaded)
    {
        return tcGetBarycentricHeightAtWorldPosition(it->second, wp, a, b, c);
    }

    return 0.0f;
}

void tcmUpdateChunks(terrain_chunk_manager* tcm, const float3& wp, const perlin_config& config, float dt)
{
    const int chunkViewDistance = 12;
    chunk_id centre = GetChunkIdFromWorldPosition(wp);

    // remove old chunks

    std::vector<chunk_id> unloadedChunks;
    for (auto kv : tcm->chunks) {
        if (terrain_chunk* chunk = kv.second) {
            chunk_id id = GetChunkIdFromWorldPosition(chunk->worldOffset);
            if (id.x < centre.x - chunkViewDistance || id.x > centre.x + chunkViewDistance || id.y < centre.y - chunkViewDistance || id.y > centre.y + chunkViewDistance) {
                unloadedChunks.push_back(id);
            }
        }
    }

    for (chunk_id& id : unloadedChunks) {
        terrain_chunk* chunk = tcm->chunks.at(id);
        if (chunk->hasBeenLoaded) {
            tcm->chunks.erase(id);
            DestroyTerrainChunk(chunk);
        }
    }

    unloadedChunks.clear();

    // load new chunks

    std::vector<terrain_chunk*> loadedChunks;
    for (int x = centre.x - chunkViewDistance; x <= centre.x + chunkViewDistance; ++x) {
        for (int z = centre.y - chunkViewDistance; z <= centre.y + chunkViewDistance; ++z) {
            
            chunk_id id = chunk_id(x, z);
            auto it = tcm->chunks.find(id);
            if (it == tcm->chunks.end()) {
                terrain_chunk* chunk = CreateTerrainChunk(tcm->config, (float2)id);
                tcm->chunks.emplace(id, chunk);

                loadedChunks.push_back(chunk);
            }

        }
    }

    // we load on a separate thread
    tcmAppendChunksToLoad(tcm, wp, loadedChunks);
    loadedChunks.clear();

    for (auto kv : tcm->chunks) {
        terrain_chunk* chunk = kv.second;
        if (chunk != nullptr) {
            tcUpdateTerrainChunk(chunk);
        }
    }

}

bool tcIsChunkVisible(terrain_chunk* chunk, camera* cam)
{
    float3 chunkCentre = float3(chunk->worldOffset.x + 16.0f, 0.0f, chunk->worldOffset.z + 16.0f);

    chunk_id camChunkId = GetChunkIdFromWorldPosition(cam->eye);
    chunk_id terrainChunkId = GetChunkIdFromWorldPosition(chunkCentre);

    if (camChunkId == terrainChunkId)
        return true;

    float3 directionToChunk = glm::normalize(chunkCentre - float3(cam->eye.x, 0.0f, cam->eye.z));
    float3 cameraForward = glm::normalize(cGetForward(cam) * float3(1.0f, 0.0f, 1.0f));

    return glm::dot(directionToChunk, cameraForward) > 0.1f;
}

void tcmSubmitDrawbatch(terrain_chunk_manager* tcm, camera* cam, gl::program* shader)
{
    chunk_id camChunkId = GetChunkIdFromWorldPosition(cam->eye);

    for (auto kv : tcm->chunks) {
        terrain_chunk* chunk = kv.second;
        if (chunk != nullptr && chunk->mesh.mesh != nullptr) {

            if (tcIsChunkVisible(chunk, cam)) {
                glm::mat4 w = glm::translate(glm::mat4(1.0f), chunk->worldOffset);
                gl::UploadUniform(shader, "u_w", w);

                gl::BindVertexArray(chunk->mesh.mesh);
                gl::DrawVertexArray(chunk->mesh.mesh, GL_TRIANGLES, chunk->mesh.mesh->vertexCount);
            }
        }
    }
}
