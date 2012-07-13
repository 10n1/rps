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

enum {
    UNIFORM_MODELVIEWPROJECTION_MATRIX,
    UNIFORM_TEXTURE,
    UNIFORM_COLOR,
    NUM_UNIFORMS
};

typedef struct {
    
    /* Render definitions */
    GLuint  program;
    GLint   uniforms[NUM_UNIFORMS];
    GLuint  vao;
    GLuint  quad_vertex_buffer;
    GLuint  quad_index_buffer;
    GLuint  texture;
} game_t;

void game_initialize(game_t* game);
void game_update(game_t* game, float width, float height);
void game_render(game_t* game);
void game_shutdown(game_t* game);

#ifdef __cplusplus
}
#endif

#endif /* include guard */
