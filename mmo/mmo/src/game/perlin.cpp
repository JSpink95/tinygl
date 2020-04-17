
#include "game/perlin.h"

#include "glm/gtc/noise.hpp"

#include <algorithm>
#include <numeric>

perlin* CreatePerlin(int seed)
{
    perlin* result = new perlin;

    std::vector<int> perms;
    perms.resize(256);

    std::iota(perms.begin(), perms.end(), 0);
    std::random_shuffle(perms.begin(), perms.end());

    result->perms = perms;
    result->perms.insert(result->perms.end(), perms.begin(), perms.end());

    return result;
}

void DestroyPerlin(perlin* p)
{
    if (p)
    {
        delete p;
    }
}

float pnNoise(const perlin_config& config, const float2& p)
{
    float persistence = 0.6f;
    float lacunarity = 2.1f;

    float amplitude = config.amplitude;
    float frequency = 0.5f;

    float result = 0.0f;
    for (int octave = 0; octave < 5; ++octave) {
        float2 xy = p * config.scale * frequency;

        float perlinValue = glm::perlin(xy) * 0.5f + 0.5f;
        result += perlinValue * amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return result;
}
