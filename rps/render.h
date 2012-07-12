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

#ifdef __cplusplus
extern "C" { // Use C linkage
#endif

typedef struct {
    uint32_t    index;
    const char* name;
} bind_location_t;

GLuint render_create_shader(GLenum type, const char* source);
GLuint render_create_program(GLuint vertex_shader, GLuint fragment_shader,
                             const bind_location_t* binds, int num_binds);
GLuint render_create_texture(const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* include guard */
