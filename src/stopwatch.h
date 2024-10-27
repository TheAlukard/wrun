#pragma once

#include <time.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t elapsed;
    pthread_t thread;
    bool running;
} StopWatch;

static inline void sw_start(StopWatch *sw);
static inline void sw_stop(StopWatch *sw);
static inline void sw_reset(StopWatch *sw);
static inline void sw_restart(StopWatch *sw);
static inline double sw_elapsedsec(StopWatch *sw);
static inline uint64_t sw_elapsedms(StopWatch *sw);
 
#ifdef STOPWATCH_IMPLEMENTATION

void* start_running(void *_sw)
{
    long last_clock = 0;
    StopWatch *sw = (StopWatch*)_sw;

    while (true) {
        clock_t c = clock();
        sw->elapsed += c - last_clock;
        last_clock = c;
    }

    return NULL;
}


static inline void sw_start(StopWatch *sw)
{
    pthread_create(&sw->thread, NULL, start_running, sw);
    pthread_detach(sw->thread);
    sw->running = true;
}

static inline void sw_stop(StopWatch *sw)
{
    pthread_cancel(sw->thread);
    sw->running = false;
}

static inline void sw_reset(StopWatch *sw)
{
    sw->elapsed = 0;
}

static inline void sw_restart(StopWatch *sw)
{
    if (sw->running) {
        sw_stop(sw);
    }
    sw_reset(sw);
    sw_start(sw);
}

static inline double sw_elapsedsec(StopWatch *sw)
{
    return (double)sw->elapsed / (double)CLOCKS_PER_SEC;
}

static inline uint64_t sw_elapsedms(StopWatch *sw)
{
    return sw_elapsedsec(sw) * 1000;
}

#endif // STOPWATCH_IMPLEMENTATION
