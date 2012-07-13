/*  @file game.c
 *  @brief Main game class
 *  @author Kyle Weicht
 *  @date 7/12/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "game.h"

#include <stddef.h>

#include "system.h"
#include "render.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

extern void CNSLog(const char* format,...);
/*----------------------------------------------------------------------------*\
Internal
\*----------------------------------------------------------------------------*/
enum {
    ATTRIB_POSITION,
    ATTRIB_TEXCOORD,
    NUM_ATTRIBUTES
};

typedef struct {
    float   pos[3];
    float   tex[2];
} vertex_t;
static const vertex_t gQuadVertexData[] =
{
    -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, // TL
     0.5f,  0.5f, 0.0f,     1.0f, 0.0f, // TR
     0.5f, -0.5f, 0.0f,     1.0f, 1.0f, // BR
     
    -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, // BL
     0.5f, -0.5f, 0.0f,     1.0f, 1.0f, // BR
    -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, // TL
};

/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
void game_initialize(game_t* game) 
{
    char buffer[2048];
    GLuint vertex_shader;
    GLuint fragment_shader;
    bind_location_t binds[] = 
    {
        { ATTRIB_POSITION, "position" },
        { ATTRIB_TEXCOORD, "tex" }
    };

    /* Render data */
    glGenVertexArraysOES(1, &game->vao);
    glBindVertexArrayOES(game->vao);
    
    glGenBuffers(1, &game->quad_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, game->quad_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gQuadVertexData), gQuadVertexData, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(ATTRIB_POSITION);
    glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(ATTRIB_TEXCOORD);
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(0);
    
    /* Program */
    if(system_load_file("Shader.vsh", buffer, sizeof(buffer))) {
        CNSLog("Vertex shader load failed!\n");
        return;
    }
    vertex_shader = render_create_shader(GL_VERTEX_SHADER, buffer);
    
    if(system_load_file("Shader.fsh", buffer, sizeof(buffer))) {
        CNSLog("Fragment shader load failed!\n");
        return;
    }    
    fragment_shader = render_create_shader(GL_FRAGMENT_SHADER, buffer);
    game->program = render_create_program(vertex_shader, fragment_shader, binds, 2);
    
    /* Uniforms */
    game->uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX] = glGetUniformLocation(game->program, "modelViewProjectionMatrix");
    game->uniforms[UNIFORM_TEXTURE] = glGetUniformLocation(game->program, "diffuseTexture");
    game->uniforms[UNIFORM_COLOR] = glGetUniformLocation(game->program, "color");
    
    /* GL setup */
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    game->texture = render_create_texture("r.png");
    
    /* Font */
    render_load_font("font.fnt");
}
void game_update(game_t* game)
{
}
void game_render(game_t* game)
{
}
void game_shutdown(game_t* game)
{
    glDeleteBuffers(1, &game->quad_vertex_buffer);
    glDeleteVertexArraysOES(1, &game->vao);
    glDeleteProgram(game->program);
    glDeleteTextures(1, &game->texture);
}
