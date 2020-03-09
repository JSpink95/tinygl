
#include "random.h"

namespace eng
{
    static std::uniform_int_distribution<int> random_distribution = std::uniform_int_distribution<int>(0, INT_MAX);
    static std::default_random_engine random_engine;

    namespace random
    {
        void seed(unsigned long long s)
        {
            random_engine.seed(s);
        }

        int next_int()
        {
            return random_distribution(random_engine);
        }

        float next_float()
        {
            return (float)next_int() / (float)INT_MAX;
        }

        float next_float(float max)
        {
            return next_float() * max;
        }

        float next_float(float min, float max)
        {
            return min + next_float() * (max - min);
        }
    }
}