/*! @file render.c
 *  @brief Rendering
 *  @author Kyle Weicht
 *  @date 7/11/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "render.h"

#include <stdlib.h>
#include <stddef.h>

/*----------------------------------------------------------------------------*\
Internal
\*----------------------------------------------------------------------------*/
extern void CNSLog(const char* format,...);

/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
GLuint compile_shader(GLenum type, const char* source)
{   
    GLint status;
    GLint logLength;
    GLuint shader;
    
    if (!source) {
        CNSLog("Failed to load vertex shader");
        return 0;
    }
    
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
#if defined(DEBUG)
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc((size_t)logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        CNSLog("Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}
