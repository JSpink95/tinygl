#pragma once

#include "maths/maths.h"

using chunk_id = glm::ivec2;

namespace std
{
    template<>
    struct hash<chunk_id>
    {
        std::size_t operator()(const chunk_id& id) const
        {
            // he he he, I feel naughty
            std::size_t h1 = std::hash<unsigned int>{}((unsigned int)(id.x + INT_MAX));
            std::size_t h2 = std::hash<unsigned int>{}((unsigned int)(id.y + INT_MAX));
            return h1 ^ (h2 << 1);
        }
    };
}

struct chunk_vertex {
    float3 position;
    float4 color;
};

struct chunk_config {
    static const chunk_config defaultConfig;

    unsigned int size = 32u;
    float cellSize = 1.0f;
};

static inline int GetIndexFromCoord(const chunk_config& config, int x, int z)
{
    return x + z * (config.size + 1);
}
