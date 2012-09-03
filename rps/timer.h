/*! @file timer.h
 *  @brief TODO: Add the purpose of this module
 *  @author Kyle Weicht
 *  @date 8/9/12 11:37 AM
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 *	@addtogroup timer timer
 *	@{
 */
#ifndef __timer_h__
#define __timer_h__

#include <stdint.h>

struct lua_State;

/*! Timer */
typedef struct Timer
{
    uint64_t    start_time;
    uint64_t    prev_time;
    double      frequency;
} Timer;

/*! @brief Initializes and starts the timer */
void timer_init(Timer* timer);
/*! @brief Resets the timer */
void timer_reset(Timer* timer);
/*! @brief The elapsed time since the last call to Timer::get_delta_time
 *  @return The time in seconds
 */
double timer_delta_time(Timer* timer);
/*! @brief The time since the last call to Timer::reset or Timer::init
 *  @return The time in seconds
 */
double timer_running_time(Timer* timer);
/*! @brief Export the timer functions and object to a Lua state */
int luaopen_timer(struct lua_State* L);

/* @} */
#endif /* include guard */
