
#include "utility/gradient.h"

#include <algorithm>

gradient* CreateGradient()
{
    return new gradient;
}

void DestroyGradient(gradient* g)
{
    if (g)
    {
        delete g;
    }
}

float4 GetInterpolatedValue(gradient* g, float percentage)
{
    if (g->nodes.size() == 0)
    {
        return float4(0.0f);
    }

    float vp = glm::clamp(percentage, 0.0f, 1.0f);
    auto it = std::find_if(g->nodes.begin(), g->nodes.end(), [vp](const gradient_node& gn) -> bool
    {
        return gn.percentage > vp;
    });

    if (it == g->nodes.end())
    {
        return g->nodes.back().value;
    }

    size_t index = std::distance(g->nodes.begin(), it);
    if (index == 0)
    {
        return g->nodes.at(index).value;
    }

    const gradient_node& a = g->nodes.at(index - 1);
    const gradient_node& b = g->nodes.at(index - 0);

    return glm::mix(a.value, b.value, (vp - a.percentage) / (b.percentage - a.percentage));
}

void PushValue(gradient* g, const float4& value, float percentage)
{
    g->nodes.push_back({ value, percentage });
    std::sort(g->nodes.begin(), g->nodes.end(), [](const gradient_node& a, const gradient_node& b) -> bool
    {
        return a.percentage < b.percentage;
    });
}
