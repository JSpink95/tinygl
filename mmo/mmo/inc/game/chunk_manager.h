#pragma once

#include "game/chunk_types.h"
#include "game/perlin.h"

#include <future>
#include <unordered_map>

struct chunk_t;
struct camera_t;

namespace gl
{
    struct program;
}

struct chunk_manager
{
    const chunk_config chunkConfig;
    const perlin_config perlinConfig;

    // thread data
    bool runnning;
    std::future<void> loadThread;
    std::mutex loadThreadMutex;
    std::vector<chunk_t*> chunksToLoad;

    // game data
    std::unordered_map<chunk_id, chunk_t*> chunks;
};

namespace tgl
{
    // creators/destructors
    chunk_manager* CreateChunkManager(const chunk_config& config, const perlin_config& perlinConfig);
    void DestroyChunkManager(chunk_manager* tcm);

    // thread safe chunk loading
    void AppendChunksToLoad(chunk_manager* tcm, const float3& wp, const std::vector<chunk_t*>& chunks);

    // getters
    float3 GetWorldOffsetFromChunkId(const chunk_id& id);
    chunk_id GetChunkIdFromWorldPosition(const float3& wp);

    // height getters
    float GetBarycentricHeightAtWorldPosition(chunk_manager* tcm, const float3& wp);
    float GetBarycentricHeightAtWorldPosition(chunk_manager* tcm, const float3& wp, float3& a, float3& b, float3& c);

    // update
    void UpdateChunks(chunk_manager* tcm, const float3& wp, const perlin_config& config, float dt);

    // rendering
    void SubmitDrawbatch(chunk_manager* tcm, camera_t* camera, gl::program* shader, gl::program* grassShader);
    void SubmitTransparentDrawbatch(chunk_manager* tcm, camera_t* camera, gl::program* shader);
}
