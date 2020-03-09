
#include "engine/timer.h"

#include <chrono>

namespace chrono = std::chrono;

using high_res_clock = chrono::high_resolution_clock;
using milliseconds = chrono::milliseconds;
using seconds = chrono::seconds;

namespace eng
{
    timer* CreateTimer()
    {
        timer* result = new timer;
        result->time = time_now();

        return result;
    }

    void DestroyTimer(timer* t)
    {
        if (t != nullptr)
        {
            delete t;
        }
    }

    marked_time time_now()
    {
        return high_res_clock::now().time_since_epoch().count();
    }

    marked_time time_mark(timer* t)
    {
        marked_time last = t->time;
        t->time = time_now();

        return t->time - last;
    }

    marked_time time_elapsed(timer* t)
    {
        return time_now() - t->time;
    }

    double to_seconds(marked_time time)
    {
        // convert nano seconds to seconds
        return (double)time * 1e-09;
    }
}