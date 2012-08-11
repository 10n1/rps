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
static float _width = 0;
static float _height = 0;
static int button_width = 0;
static int bottom = 0;
static int left = 0;
static struct {
    char    c;
    float   x;
    float   y;
    float   scale;
    int     selected;
} buttons[3] = { { 'R' }, { 'P' }, { 'S' } };

/* Paper and Scissors: http://www.Clker.com */
/* Rock: http://opengameart.org/content/rocks */
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
    _width = width;
    _height = height;
    //float aspect = fabsf(width/height);
    //width = 1.0f;
    //height = width/aspect;
    button_width = (int)width/3;
    bottom = -(int)height/2;
    left = (int)-width/2;
    render_resize(width, height);
    buttons[0].c = (char)render_create_texture("assets/rock.png");
    buttons[0].x = left + button_width*0.5f;
    buttons[0].y = bottom;
    buttons[0].scale = button_width;
    buttons[1].c = (char)render_create_texture("assets/paper.png");
    buttons[1].x = left + button_width*1.5f;
    buttons[1].y = bottom;
    buttons[1].scale = button_width;
    buttons[2].c = (char)render_create_texture("assets/scissors.png");
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
    for(ii=0;ii<3;++ii) {
        if(buttons[ii].selected)
            render_set_color(1.0f, 0.0f, 0.0f);
        else
            render_set_color(1.0f, 1.0f, 1.0f);
        render_draw_quad((GLuint)buttons[ii].c, buttons[ii].x, buttons[ii].y, buttons[ii].scale, buttons[ii].scale );
    }
}
void game_shutdown(game_t* game)
{
    game->initialized = 0;
}
void game_handle_tap(game_t* game, float x, float y)
{
    int ii;
    x -= _width/2;
    y = -y + _height/2;

    if(y >= bottom && y <= bottom+button_width)
    {
        for(ii=2;ii>=0;--ii) {
            if(x > buttons[ii].x - button_width/2 && x <= buttons[ii].x + button_width/2)
            {
                buttons[ii].selected = 1;
                continue;
            }
            buttons[ii].selected = 0;
        }
    }
}
