/*! @file render.c
 *  @brief Rendering
 *  @author Kyle Weicht
 *  @date 7/11/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "render.h"

#include <stdlib.h>
#include <stddef.h>
#include "stb_image.h"
#include "system.h"

extern void CNSLog(const char* format,...);
/*----------------------------------------------------------------------------*\
Internal
\*----------------------------------------------------------------------------*/
static int _link_program(GLuint program)
{
    GLint status;
    GLint logLength;
    glLinkProgram(program);
    
#if defined(DEBUG)
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc((size_t)logLength);
        glGetProgramInfoLog(program, logLength, &logLength, log);
        CNSLog("Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == 0) {
        return 1;
    }
    
    return 0;
}
static int _validate_program(GLuint program)
{
    GLint logLength, status;
    
    glValidateProgram(program);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc((size_t)logLength);
        glGetProgramInfoLog(program, logLength, &logLength, log);
        CNSLog("Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return 1;
    }
    
    return 0;
}

/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
GLuint render_create_shader(GLenum type, const char* source)
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
GLuint render_create_program(GLuint vertex_shader, GLuint fragment_shader, 
                             const bind_location_t* binds, int num_binds)
{
    int ii;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    
    /* Binding must be done pre-link. Not sure why */
    for(ii=0;ii<num_binds;++ii) {
        glBindAttribLocation(program, binds[ii].index, binds[ii].name);
    }
    
    if(_link_program(program) || _validate_program(program)) {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        if(program)
            glDeleteProgram(program);
        return 0;
    }
    
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return program;
}
GLuint render_create_texture(const char* filename)
{
    GLuint texture;
    int width;
    int height;
    int comp;
    void* data;
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    data = stbi_load(system_get_path(filename), &width, &height, &comp, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texture;
}
