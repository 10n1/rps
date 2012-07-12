/*! @file render.h
 *  @brief Rendering
 *  @author Kyle Weicht
 *  @date 7/11/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#ifndef _render_h__
#define _render_h__

#include <OpenGLES/ES2/gl.h>

#ifdef __cplusplus
extern "C" { // Use C linkage
#endif

GLuint compile_shader(GLenum type, const char* source);

#ifdef __cplusplus
}
#endif

#endif /* include guard */
