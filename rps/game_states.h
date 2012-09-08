/*! @file game_states.h
 *  @brief TODO: Add the purpose of this module
 *  @author Kyle Weicht
 *  @date 9/5/12 9:21 PM
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 *	@addtogroup game_states game_states
 *	@{
 */
#ifndef __game_states_h__
#define __game_states_h__

struct game_t;
typedef struct game_state_t game_state_t;

struct game_state_t {
    void (*on_enter)(struct game_t* game);
    void (*on_update)(struct game_t* game);
    void (*on_render)(struct game_t* game);
    void (*on_exit)(struct game_t* game);
};

/* @} */
#endif /* include guard */
