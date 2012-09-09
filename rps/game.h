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
#include "ui.h"

typedef struct game_t game_t;
typedef struct player_t player_t;
typedef float float2[2];

typedef enum {
    kPause,
    kGame
} game_state_t;

typedef enum {
    kRock = 0,
    kPaper = 1,
    kScissors = 2,

    kInvalid = -1
} weapon_t;

struct player_t {
    weapon_t selection;
    int score;
};

typedef struct {
    weapon_t    weapon;
    float       timer;
} note_t;

struct game_t {
    Timer           timer;
    float           delta_time;
    int             initialized;
    note_t          current_weapon;
    float2          current_position;
    float           speed;
    player_t        player;
    game_state_t    state;
    float           pause_timer;
};

void game_initialize(game_t* game, float width, float height);
void game_update(game_t* game);
void game_render(game_t* game);
void game_shutdown(game_t* game);
void game_pause(game_t* game);
void game_resume(game_t* game);
void game_toggle_pause(ui_param_t* params);

void game_handle_tap(game_t* game, float x, float y);

#endif /* include guard */
