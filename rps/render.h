/*! @file render.h
 *  @brief Rendering
 *  @author Kyle Weicht
 *  @date 7/11/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#ifndef _render_h__
#define _render_h__

#include <stdint.h>
#include "rps_math.h"
//#include <OpenGLES/ES2/gl.h>
#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else  // For iOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

enum {
    ATTRIB_POSITION,
    ATTRIB_TEXCOORD,
    NUM_ATTRIBUTES
};

enum {
    UNIFORM_WORLD_MATRIX,
    UNIFORM_VIEWPROJECTION_MATRIX,
    UNIFORM_TEXTURE,
    UNIFORM_COLOR,
    NUM_UNIFORMS
};

enum {
    MESH_FULLSCREEN,
    MESH_QUAD,
    
    NUM_MESHES
};

enum {
    VERTEX_BUFFER,
    INDEX_BUFFER,
    
    NUM_BUFFERS
};

typedef enum {
    kPerspective,
    kOrthographic,

    kNumProjectionTypes
} projection_type_t;

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

void render_set_colorf(float r, float g, float b, float a);
void render_set_colorfv(float* c);
void render_set_projection_matrix(projection_type_t type);
void render_draw_quad(GLuint texture, float x, float y, float width, float height);
void render_draw_quad_transform(GLuint texture, const float4x4* transform);
void render_draw_custom_quad(GLuint texture, GLuint vao, float x, float y, float width, float height);
void render_draw_custom_quad_vbo(GLuint texture, GLuint vertex_buffer, GLuint index_buffer, float x, float y, float width, float height);
void render_draw_fullscreen_quad(void);
void render_resize(float width, float height);

void render_prepare(void);


#endif /* include guard */
