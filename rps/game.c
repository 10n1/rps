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
static float _width     = 320;
static float _height    = 480;
static struct {
    GLuint tex;
    float x;
    float y;
    float scale;
} _buttons[16] = {0};
static int _num_buttons = 0;
static int _selected = -1;

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
    _buttons[0].tex = render_create_texture("assets/rock.png");
    _buttons[0].x = -0.25f;
    _buttons[0].scale = 0.25f;
    _buttons[1].tex = render_create_texture("assets/paper.png");
    _buttons[1].x = 0.0f;
    _buttons[1].scale = 0.25f;
    _buttons[2].tex = render_create_texture("assets/scissors.png");
    _buttons[2].x = 0.25f;
    _buttons[2].scale = 0.25f;
    _num_buttons = 3;
}
void game_update(game_t* game, float width, float height)
{
    _width = width;
    _height = height;
    render_resize(width, height);
}
void game_render(game_t* game)
{
    int ii;
    render_prepare();

    for(ii=0;ii<_num_buttons;++ii) {
        if(ii == _selected)
            render_set_color(1.0f, 0.0f, 0.0f);
        else            
            render_set_color(1.0f, 1.0f, 1.0f);
        render_draw_quad(_buttons[ii].tex,
                         _buttons[ii].x,
                         _buttons[ii].y,
                         _buttons[ii].scale,
                         _buttons[ii].scale);
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
    printf("X: %f\tY: %f\n", x, y);

    x /= _width;
    y /= _height;
    printf("X: %f\tY: %f\n", x, y);

    _selected = -1;
    for (ii=0; ii<_num_buttons; ++ii) {
        float l = _buttons[ii].x - _buttons[ii].scale/2;
        float r = _buttons[ii].x + _buttons[ii].scale/2;
        float b = _buttons[ii].y - _buttons[ii].scale/2;
        float t = _buttons[ii].y + _buttons[ii].scale/2;
        if(x > l && x <= r && y > b && y <= t)
        {
            _selected = ii;
            break;
        }
    }
}
