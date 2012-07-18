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
int grid;

/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
void game_initialize(game_t* game) 
{
    /* Render data */
    
    /* GL setup */
    render_init();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    game->texture = render_create_texture("assets/font_0.png");
    grid = render_create_texture("assets/grid.png");
    
    /* Font */
    render_load_font("assets/font.fnt");
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
    glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    render_prepare();
    
    glBindTexture(GL_TEXTURE_2D, game->texture);
    
    if(game && 0) {
        render_draw_letter('@', 0.5f, 0);
        render_draw_string("Hello", -0.5f, 0.0f, 1.0f);
    } else {
        glBindTexture(GL_TEXTURE_2D, grid);
        render_draw_fullscreen_quad();
    }
}
void game_shutdown(game_t* game)
{
    glDeleteBuffers(1, &game->quad_vertex_buffer);
    glDeleteVertexArraysOES(1, &game->vao);
    glDeleteProgram(game->program);
    glDeleteTextures(1, &game->texture);
}
