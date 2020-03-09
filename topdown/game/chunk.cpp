
#include "game/chunk.h"

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

    void DestroyChunk(chunk* c)
    {
        if (c)
        {
            delete c;
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