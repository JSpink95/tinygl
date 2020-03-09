
#pragma once

namespace eng
{
    using marked_time = long long;

    struct timer
    {
        marked_time time;
    };

    timer* CreateTimer();
    void DestroyTimer(timer* t);

    marked_time time_now();
    marked_time time_mark(timer* t);
    marked_time time_elapsed(timer* t);

    double to_seconds(marked_time time);
}