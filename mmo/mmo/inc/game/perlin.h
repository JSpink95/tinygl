#pragma once

#include "maths/maths.h"

#include <vector>

struct perlin_config
{
    int seed = 404;
    float scale = 0.1f;
    float amplitude = 1.0f;
};

struct perlin
{
    std::vector<int> perms;
};

perlin* CreatePerlin(int seed);
void DestroyPerlin(perlin* p);

float pnNoise(const perlin_config& config, const float2& p);