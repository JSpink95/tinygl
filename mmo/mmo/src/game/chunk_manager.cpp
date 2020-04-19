
#include "game/chunk_manager.h"
#include "game/chunk.h"
#include "game/camera.h"
#include "game/grass.h"

#include "glfw/glfw.h"

#include "gl/buffer.h"
#include "gl/vertex_array.h"
#include "gl/shader.h"

#include "glm/gtx/norm.hpp"

namespace tgl
{
    // thread function
    // used to load/unload chunks
    void Thread_ChunkLoader(chunk_manager* cm)
    {
        while (cm->runnning) {

            if (cm->chunksToLoad.size() > 0) {
                chunk_t* chunk = nullptr;

                {
                    std::lock_guard<std::mutex> lock(cm->loadThreadMutex);
                    chunk = cm->chunksToLoad.back();
                    cm->chunksToLoad.pop_back();
                }

                if (chunk)
                {
                    GenerateHeights(chunk, cm->perlinConfig);
                    GenerateMesh(chunk);
                    chunk->hasBeenLoaded = true;
                }
            }
        }
    }

    chunk_manager* CreateChunkManager(const chunk_config& config, const perlin_config& perlinConfig)
    {
        chunk_manager* result = new chunk_manager{
            config, perlinConfig
        };

        result->runnning = true;
        result->loadThread = std::async(Thread_ChunkLoader, result);

        //PushValue(g, float4(0.00f, 0.00f, 0.00f, 1.00f), 0.00f);
        //PushValue(g, float4(1.00f, 0.00f, 0.00f, 1.00f), 0.25f);
        //PushValue(g, float4(0.00f, 1.00f, 0.00f, 1.00f), 0.50f);
        //PushValue(g, float4(0.00f, 0.00f, 1.00f, 1.00f), 0.75f);
        //PushValue(g, float4(1.00f, 1.00f, 1.00f, 1.00f), 1.00f);
        //PushValue(g, float4(0.23f, 0.80f, 0.10f, 1.00f), 0.0f);
        //PushValue(g, float4(1.00f, 1.00f, 1.00f, 1.00f), 1.0f);


        return result;
    }

    void DestroyChunkManager(chunk_manager* cm)
    {
        if (cm)
        {
            for (auto kv : cm->chunks) {
                if (kv.second) {
                    DestroyChunk(kv.second);
                }
            }

            cm->chunks.clear();
            delete cm;
        }
    }

    void AppendChunksToLoad(chunk_manager* cm, const float3& wp, const std::vector<chunk_t*>& chunks)
    {
        if (chunks.size() > 0) {
            std::lock_guard<std::mutex> lock(cm->loadThreadMutex);
            for (chunk_t* chunk : chunks) {
                chunk->hasBeenLoaded = false;
                cm->chunksToLoad.push_back(chunk);
            }

            std::sort(cm->chunksToLoad.begin(), cm->chunksToLoad.end(), [wp](chunk_t* a, chunk_t* b) -> bool
            {
                float da = glm::distance2(wp, a->worldOffset + float3(16.0f, 0.0f, 16.0f));
                float db = glm::distance2(wp, b->worldOffset + float3(16.0f, 0.0f, 16.0f));

                return db < da;
            });
        }
    }

    float3 GetWorldOffsetFromChunkId(const chunk_id& id)
    {
        return float3((float)id.x * 32.0f, 0.0f, (float)id.y * 32.0f);
    }

    chunk_id GetChunkIdFromWorldPosition(const float3& wp)
    {
        chunk_id result;
        result.x = (int)glm::floor(wp.x / 32.0f);
        result.y = (int)glm::floor(wp.z / 32.0f);

        return result;
    }

    float GetBarycentricHeightAtWorldPosition(chunk_manager* cm, const float3& wp)
    {
        chunk_id id = GetChunkIdFromWorldPosition(wp);
        auto it = cm->chunks.find(id);
        if (it != cm->chunks.end() && it->second->hasBeenLoaded)
        {
            float3 a, b, c;
            return GetBarycentricHeightAtWorldPosition(it->second, wp, a, b, c);
        }

        return 0.0f;
    }

    float GetBarycentricHeightAtWorldPosition(chunk_manager* cm, const float3& wp, float3& a, float3& b, float3& c)
    {
        chunk_id id = GetChunkIdFromWorldPosition(wp);
        auto it = cm->chunks.find(id);
        if (it != cm->chunks.end() && it->second->hasBeenLoaded)
        {
            return GetBarycentricHeightAtWorldPosition(it->second, wp, a, b, c);
        }

        return 0.0f;
    }

    void UpdateChunks(chunk_manager* cm, const float3& wp, const perlin_config& config, float dt)
    {
        const int chunkViewDistance = 12;
        chunk_id centre = GetChunkIdFromWorldPosition(wp);

        // remove old chunks

        std::vector<chunk_id> unloadedChunks;
        for (auto kv : cm->chunks) {
            if (chunk_t* chunk = kv.second) {
                chunk_id id = GetChunkIdFromWorldPosition(chunk->worldOffset);
                if (id.x < centre.x - chunkViewDistance || id.x > centre.x + chunkViewDistance || id.y < centre.y - chunkViewDistance || id.y > centre.y + chunkViewDistance) {
                    unloadedChunks.push_back(id);
                }
            }
        }

        for (chunk_id& id : unloadedChunks) {
            chunk_t* chunk = cm->chunks.at(id);
            if (chunk->hasBeenLoaded) {
                cm->chunks.erase(id);
                DestroyChunk(chunk);
            }
        }

        unloadedChunks.clear();

        // load new chunks

        std::vector<chunk_t*> loadedChunks;
        for (int x = centre.x - chunkViewDistance; x <= centre.x + chunkViewDistance; ++x) {
            for (int z = centre.y - chunkViewDistance; z <= centre.y + chunkViewDistance; ++z) {

                chunk_id id = chunk_id(x, z);
                auto it = cm->chunks.find(id);
                if (it == cm->chunks.end()) {
                    chunk_t* chunk = CreateChunk(cm, (float2)id);
                    cm->chunks.emplace(id, chunk);

                    loadedChunks.push_back(chunk);
                }

            }
        }

        // we load on a separate thread
        AppendChunksToLoad(cm, wp, loadedChunks);
        loadedChunks.clear();

        // update active chunks
        for (auto kv : cm->chunks) {
            chunk_t* chunk = kv.second;
            if (chunk != nullptr) {
                UpdateTerrainChunk(chunk);
            }
        }

    }

    bool IsChunkVisible(chunk_t* chunk, camera_t* camera)
    {
        float3 chunkCentre = float3(chunk->worldOffset.x + 16.0f, 0.0f, chunk->worldOffset.z + 16.0f);

        chunk_id camChunkId = GetChunkIdFromWorldPosition(camera->eye);
        chunk_id terrainChunkId = GetChunkIdFromWorldPosition(chunkCentre);

        if (camChunkId == terrainChunkId)
            return true;

        float3 chunkCornerPoints[4];
        GetChunkCornerPoints(chunk, chunkCornerPoints);

        float3 camXZ = float3(camera->eye.x, 0.0f, camera->eye.z);
        float3 cameraForward = glm::normalize(cGetFlatForward(camera));

        for (unsigned int idx = 0; idx < 4; ++idx) {
            float3 directionToChunk = glm::normalize(chunkCornerPoints[idx] - camXZ);

            if (glm::dot(directionToChunk, cameraForward) > 0.6f) {
                return true;
            }
        }

        return false;
    }

    void SubmitDrawbatch(chunk_manager* cm, camera_t* camera, gl::program* shader, gl::program* grassShader)
    {
        chunk_id camChunkId = GetChunkIdFromWorldPosition(camera->eye);

        unsigned int renderedChunks = 0u;

        for (auto kv : cm->chunks) {
            chunk_t* chunk = kv.second;
            if (chunk != nullptr && chunk->heightmap != nullptr) {

                if (IsChunkVisible(chunk, camera)) {
                    gl::BindProgram(shader);

                    glm::mat4 w = glm::translate(glm::mat4(1.0f), chunk->worldOffset);
                    gl::UploadUniform(shader, "u_w", w);

                    gl::BindVertexArray(chunk->heightmap);
                    gl::DrawVertexArray(chunk->heightmap, GL_TRIANGLES, chunk->heightmap->vertexCount);

                    chunk_id thisChunkId = GetChunkIdFromWorldPosition(chunk->worldOffset + float3(1.0f));

                    gl::BindProgram(grassShader);
                    tg::SubmitGrassDrawBatch(chunk->grass, grassShader);

                    renderedChunks += 1;
                }
            }
        }

        //printf("Rendered %i chunks this frame! that's %i blocks of grass!!\n", renderedChunks, renderedChunks * 10000u);
    }

    void SubmitTransparentDrawbatch(chunk_manager* cm, camera_t* camera, gl::program* shader)
    {
    }
}