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

#ifdef __cplusplus
extern "C" { // Use C linkage
#endif 

typedef struct {
    int initialized;
} game_t;

void game_initialize(game_t* game);
void game_update(game_t* game, float width, float height);
void game_render(game_t* game);
void game_shutdown(game_t* game);

#ifdef __cplusplus
}
#endif

#endif /* include guard */
