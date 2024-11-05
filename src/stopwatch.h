#pragma once

#include <time.h>
#include <stdint.h>

typedef struct {
    clock_t start;
    uint64_t elapsed;
} StopWatch;

#define SW_API static inline

SW_API void sw_start(StopWatch *sw)
{
    sw->start = clock();
    sw->elapsed = 0;
}

SW_API void sw_stop(StopWatch *sw)
{
    sw->elapsed = clock() - sw->start;
}

SW_API double sw_elapsedsec(StopWatch *sw)
{
    sw_stop(sw);
    return (double)sw->elapsed / (double)CLOCKS_PER_SEC;
}

SW_API uint64_t sw_elapsedms(StopWatch *sw)
{
    return sw_elapsedsec(sw) * 1000;
}

#undef SW_API
