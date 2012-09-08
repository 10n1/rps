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
#include "game_states.h"

typedef struct game_t game_t;
typedef struct player_t player_t;

typedef enum {
    kRoundStart,
    kRoundPicking,
    kRoundResults,
    kPause
} round_state_t;

typedef enum {
    kRock = 0,
    kPaper = 1,
    kScissors = 2,

    kInvalid = -1
} weapon_t;

struct player_t {
    char name[128];
    weapon_t selection;
    int score;
};

struct game_t {
    Timer           timer;
    float           delta_time;
    int             initialized;
    float           round_timer;
    round_state_t   round_state;
    round_state_t   prev_state;
    float           results_timer;
    player_t        players[2];
    game_state_t*   current_state;
};

void game_initialize(game_t* game, float width, float height);
void game_update(game_t* game);
void game_render(game_t* game);
void game_shutdown(game_t* game);
void game_pause(game_t* game);
void game_resume(game_t* game);

void game_handle_tap(game_t* game, float x, float y);

#endif /* include guard */
