/*  @file game.c
 *  @brief Main game class
 *  @author Kyle Weicht
 *  @date 7/12/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "game.h"

#include <stddef.h>
#include <GLKit/GLKMath.h>

#include "system.h"
#include "render.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

extern void CNSLog(const char* format,...);
/*----------------------------------------------------------------------------*\
Internal
\*----------------------------------------------------------------------------*/
static int grid;

/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
void game_initialize(game_t* game) 
{
    game->initialized = 1;
    /* GL setup */
    render_init();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    grid = render_create_texture("assets/grid.png");
}
void game_update(game_t* game, float width, float height)
{
    float aspect = fabsf(width/height);
    width = 1.0f;
    height = width/aspect;
    render_resize(width, height);
}
void game_render(game_t* game)
{
    render_prepare();
    
    glBindTexture(GL_TEXTURE_2D, grid);
    render_draw_fullscreen_quad();
}
void game_shutdown(game_t* game)
{
    game->initialized = 0;
}
