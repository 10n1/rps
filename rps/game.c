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
static int grid = 0;
static int button_width = 0;
static int bottom = 0;
static int left = 0;
static struct {
    char    c;
    float   x;
    float   y;
    float   scale;
} buttons[3] = { { 'R' }, { 'P' }, { 'S' } };

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
    //float aspect = fabsf(width/height);
    //width = 1.0f;
    //height = width/aspect;
    button_width = (int)width/3;
    bottom = -(int)height/2;
    left = (int)-width/2;
    render_resize(width, height);
    buttons[0].c = 'R';
    buttons[0].x = left + button_width*0.5f;
    buttons[0].y = bottom;
    buttons[0].scale = button_width;
    buttons[1].c = 'P';
    buttons[1].x = left + button_width*1.5f;
    buttons[1].y = bottom;
    buttons[1].scale = button_width;
    buttons[2].c = 'S';
    buttons[2].x = left + button_width*2.5f;
    buttons[2].y = bottom;
    buttons[2].scale = button_width;
}
void game_render(game_t* game)
{
    int ii;
    render_prepare();
    
    glBindTexture(GL_TEXTURE_2D, grid);
    //render_draw_quad(160, 0.0f, 320, 1);
    //render_draw_fullscreen_quad();
    for(ii=0;ii<3;++ii)
        render_draw_letter(buttons[ii].c, buttons[ii].x, buttons[ii].y, buttons[ii].scale );
}
void game_shutdown(game_t* game)
{
    game->initialized = 0;
}
