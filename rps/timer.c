/*! @file timer.c
 *  @author Kyle Weicht
 *  @date 8/9/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "timer.h"

#if defined(__APPLE__)

#include <mach/mach_time.h>
void timer_init(Timer* timer) {
    mach_timebase_info_data_t frequency = {0,0};
    mach_timebase_info(&frequency);
    timer->frequency = (double)frequency.numer / (double)frequency.denom;
    timer->frequency *= 1e-9;
    timer_reset(timer);
}
void timer_reset(Timer* timer) {
    timer->start_time = timer->prev_time = mach_absolute_time();
}
double timer_delta_time(Timer* timer) {
    uint64_t current_time = mach_absolute_time();
    double delta_time = (double)(current_time - timer->prev_time) * timer->frequency;
    timer->prev_time = current_time;    
    return delta_time;
}
double timer_running_time(Timer* timer) {
    uint64_t current_time = mach_absolute_time();
    double running_time = (double)(current_time - timer->start_time) * timer->frequency;    
    return running_time;
}
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
void timer_init(Timer* timer) {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency( &freq );
    timer->frequency = 1.0/(double)freq.QuadPart;
    timer_reset(timer);
}
void timer_reset(Timer* timer) {
    QueryPerformanceCounter((LARGE_INTEGER*)&timer->start_time);
    timer->prev_time = timer->start_time;
}
double timer_delta_time(Timer* timer) {
    uint64_t current_time;
    double delta_time;
    QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
    delta_time = (current_time - timer->prev_time) * timer->frequency;
    timer->prev_time = current_time;
    return delta_time;
}
double timer_running_time(Timer* timer) {
    uint64_t current_time;
    double  running_time;
    QueryPerformanceCounter( (LARGE_INTEGER*)&current_time );
    running_time = (current_time - timer->start_time) * timer->frequency;
    return running_time;
}
#else
    #error Need a timer
#endif
