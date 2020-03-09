
#pragma once

#include <random>

namespace eng
{
    namespace random
    {
        void seed(unsigned long long s);

        int next_int();

        float next_float();
        float next_float(float max);
        float next_float(float min, float max);
    }
}
