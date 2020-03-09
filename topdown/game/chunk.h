
#pragma once

#include <functional>

namespace gf
{
    using chunkCellInitialiser = std::function<void(unsigned int x, unsigned int y, unsigned int z, bool& active)>;

    struct chunk_config
    {
        static const chunk_config defaultConfig;

        unsigned int width;
        unsigned int height;
        unsigned int depth;
    };

    unsigned int getChunkSizeFromConfig(const chunk_config& config);

    struct chunk
    {
        unsigned int const width, const height, const depth;
        bool* cells;
    };

    chunk* CreateChunk(const chunk_config& config);
    chunk* CreateChunk(const chunk_config& config, chunkCellInitialiser initialiser);
    void DestroyChunk(chunk* c);

    unsigned int getChunkCellCount(chunk* c);
    void convertChunkIndexToXYZ(chunk* c, unsigned int idx, unsigned int &x, unsigned int &y, unsigned int &z);
    void convertChunkXYZToIndex(chunk* c, unsigned int x, unsigned int y, unsigned int z, unsigned int &idx);

    template<typename TPredicate>
    void forEachCellInChunk(chunk* c, TPredicate predicate)
    {
        unsigned int cellCount = getChunkCellCount(c);
        for (unsigned int index = 0; index < cellCount; ++index)
        {
            unsigned int x, y, z;
            convertChunkIndexToXYZ(c, index, x, y, z);

            predicate(x, y, z, c->cells[index]);
        }
    }
}
