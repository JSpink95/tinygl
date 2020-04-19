#pragma once

#include "maths/maths.h"

#include <vector>

struct gradient_node
{
    float4 value;
    float percentage;
};

struct gradient
{
    std::vector<gradient_node> nodes;
};

gradient* CreateGradient();
void DestroyGradient(gradient* g);

float4 GetInterpolatedValue(gradient* g, float percentage);
void PushValue(gradient* g, const float4& value, float percentage);
