
#pragma once

#include "maths/maths.h"
#include "game/perlin.h"

#include <future>
#include <unordered_map>

using chunk_id = glm::ivec2;

namespace std
{
    template<>
    struct hash<chunk_id>
    {
        std::size_t operator()(const chunk_id& id) const
        {
            std::size_t h1 = std::hash<unsigned int>{}((unsigned int)(id.x + INT_MAX));
            std::size_t h2 = std::hash<unsigned int>{}((unsigned int)(id.y + INT_MAX));
            return h1 ^ (h2 << 1);
        }
    };
}

namespace gl
{
    struct program;
    struct vertex_array;
}

struct camera;

struct terrain_chunk_config
{
    static const terrain_chunk_config defaultConfig;

    int gridSize;
    float cellSize;
};

struct terrain_vertex
{
    float3 position;
    float4 color;
};

struct terrain_chunk_mesh
{
    unsigned int vertexCount = 0;
    terrain_vertex* vertices = nullptr;
    gl::vertex_array* mesh = nullptr;
};

struct terrain_chunk
{
    const terrain_chunk_config config;

    float3 worldOffset = float3(0.0f);
    float3* positions = nullptr;
    float maxHeight = 1.0f;

    // thread data
    bool hasBeenLoaded = false;
    bool reloadMesh = false;

    // render data
    terrain_chunk_mesh mesh;
};

terrain_chunk* CreateTerrainChunk(const terrain_chunk_config& config, const float2& offset);
void DestroyTerrainChunk(terrain_chunk* chunk);

int tcGetIndexFromCoord(terrain_chunk* chunk, int x, int z);
float tcGetHeightAt(terrain_chunk* chunk, int x, int z);
float tcGetBarycentricHeightAtWorldPosition(terrain_chunk* chunk, const float3& wp, float3& oa, float3& ob, float3& oc);
void tcGenerateHeights(terrain_chunk* chunk, const perlin_config& perlinConfig);
void tcGenerateMesh(terrain_chunk* chunk);
void tcUpdateTerrainChunk(terrain_chunk* chunk);

struct terrain_chunk_manager
{
    const terrain_chunk_config config;
    const perlin_config perlinConfig;

    // thread data
    bool runnning;
    std::future<void> loadThread;
    std::mutex loadThreadMutex;
    std::vector<terrain_chunk*> chunksToLoad;

    // game data
    std::unordered_map<chunk_id, terrain_chunk*> chunks;
};

terrain_chunk_manager* CreateTerrainChunkManager(const terrain_chunk_config& config, const perlin_config& perlinConfig);
void DestroyTerrainChunkManager(terrain_chunk_manager* tcm);

void tcmAppendChunksToLoad(terrain_chunk_manager* tcm, const float3& wp, const std::vector<terrain_chunk*>& chunks);

float2 GetWorldOffsetFromChunkId(const chunk_id& id);
chunk_id GetChunkIdFromWorldPosition(const float3& wp);
float tcmGetBarycentricHeightAtWorldPosition(terrain_chunk_manager* tcm, const float3& wp);
float tcmGetBarycentricHeightAtWorldPosition(terrain_chunk_manager* tcm, const float3& wp, float3& a, float3& b, float3& c);
void tcmUpdateChunks(terrain_chunk_manager* tcm, const float3& wp, const perlin_config& config, float dt);
void tcmSubmitDrawbatch(terrain_chunk_manager* tcm, camera* cam, gl::program* shader);

