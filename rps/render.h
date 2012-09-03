/*! @file render.h
 *  @brief Rendering
 *  @author Kyle Weicht
 *  @date 7/11/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#ifndef _render_h__
#define _render_h__

#include <stdint.h>
#include <OpenGLES/ES2/gl.h>

enum {
    ATTRIB_POSITION,
    ATTRIB_TEXCOORD,
    NUM_ATTRIBUTES
};

typedef struct {
    float   pos[3];
    float   tex[2];
} vertex_t;
typedef struct {
    uint32_t    index;
    const char* name;
} bind_location_t;

void render_init(void);
GLuint render_create_shader(GLenum type, const char* source);
GLuint render_create_program(GLuint vertex_shader, GLuint fragment_shader,
                             const bind_location_t* binds, int num_binds);
GLuint render_create_texture(const char* filename);

void render_set_color(float r, float g, float b);
void render_draw_quad(GLuint texture, float x, float y, float width, float height);
void render_draw_custom_quad(GLuint texture, GLuint vao, float x, float y, float width, float height);
void render_draw_fullscreen_quad(void);
void render_resize(float width, float height);

void render_prepare(void);


#endif /* include guard */
