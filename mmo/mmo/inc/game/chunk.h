
#pragma once

#include "game/chunk_types.h"
#include "game/perlin.h"

#include <future>
#include <unordered_map>

namespace gl
{
    struct program;
    struct vertex_array;
    struct texture;

    struct textured_mesh;
}

struct camera_t;
struct chunk_t;
struct grass_t;

struct terrain_chunk_mesh
{
    unsigned int vertexCount = 0;
    chunk_vertex* vertices = nullptr;
    gl::vertex_array* mesh = nullptr;
};

struct chunk_manager;

struct chunk_t
{
    chunk_manager* owner = nullptr;

    // heightmap data
    float3 worldOffset = float3(0.0f);
    float3* positions = nullptr;
    float maxHeight = -FLT_MAX;
    float minHeight = FLT_MAX;

    // thread data
    bool hasBeenLoaded = false;
    bool reloadMesh = false;

    // render data
    grass_t* grass = nullptr;
    gl::vertex_array* heightmap = nullptr;
    chunk_vertex* vertices = nullptr;
    unsigned int vertexCount = 0;
};

namespace tgl
{
    chunk_t* CreateChunk(chunk_manager* cm, const float2& offset);
    void DestroyChunk(chunk_t* chunk);

    // get corner points
    void GetChunkCornerPoints(chunk_t* chunk, float3 points[4]);

    // get heights
    float GetHeightAt(chunk_t* chunk, int x, int z);
    float GetBarycentricHeightAtWorldPosition(chunk_t* chunk, const float3& wp, float3& oa, float3& ob, float3& oc);

    // chunk generation
    void GenerateHeights(chunk_t* chunk, const perlin_config& perlinConfig);
    void GenerateMesh(chunk_t* chunk);
    void UpdateTerrainChunk(chunk_t* chunk);
}
