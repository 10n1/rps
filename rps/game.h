/*! @file game.h
 *  @brief The main game class
 *  @author Kyle Weicht
 *  @date 7/12/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#ifndef _game_h__
#define _game_h__

#include <stdint.h>
#include <OpenGLES/ES2/gl.h>
#include "timer.h"

#ifdef __cplusplus
extern "C" { // Use C linkage
#endif

typedef enum {
    kRoundStart,
    kRoundPicking,
    kRoundResults
} round_state_t;

typedef enum {
    kRock = 0,
    kPaper = 1,
    kScissors = 2,

    kInvalid = -1
} weapon_t;

typedef struct {
    Timer timer;
    float delta_time;
    int initialized;
    float round_timer;
    round_state_t round_state;
    float results_timer;
} game_t;

void game_initialize(game_t* game, float width, float height);
void game_update(game_t* game);
void game_render(game_t* game);
void game_shutdown(game_t* game);

void game_handle_tap(game_t* game, float x, float y);

#ifdef __cplusplus
}
#endif

#endif /* include guard */
