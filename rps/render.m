/*! @file render.c
 *  @brief Rendering
 *  @author Kyle Weicht
 *  @date 7/11/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "render.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <GLKit/GLKMath.h>
#include "stb_image.h"
#include "system.h"

#include "texture2d.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
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
        char* message = log;
        glGetProgramInfoLog(program, logLength, &logLength, log);
        CNSLog("Program link log:%s", message);
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
        CNSLog("Program validate log:%s", log);
        free(log);
    }
    
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return 1;
    }
    
    return 0;
}

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

static GLuint           _character_meshes[256] = {0};
static GLKMatrix4       _projectionMatrix;
static GLuint           _program = 0;
static GLuint           _meshes[NUM_MESHES] = {0};
static GLint            _uniforms[NUM_UNIFORMS] = {-1};
static Texture2D*       _font = nil;

/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
void render_init(void)
{
    char buffer[2048] = {0};
    const vertex_t quad_vertices[] =
    {
        -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, // TL
         0.5f,  0.5f, 0.0f,     1.0f, 0.0f, // TR
         0.5f, -0.5f, 0.0f,     1.0f, 1.0f, // BR
        -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, // BL
    };
    const vertex_t fullscreen_quad_vertices[] =
    {
        -1.0f,  1.0f, 0.0f,     0.0f, 0.0f, // TL
         1.0f,  1.0f, 0.0f,     1.0f, 0.0f, // TR
         1.0f, -1.0f, 0.0f,     1.0f, 1.0f, // BR
        -1.0f, -1.0f, 0.0f,     0.0f, 1.0f, // BL
    };
    const uint16_t indices[] = 
    {
        0, 1, 2,
        3, 2, 0,
    };
    bind_location_t binds[] = 
    {
        { ATTRIB_POSITION, "position" },
        { ATTRIB_TEXCOORD, "tex" }
    };
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint buffers[2] = {0};
    glGenVertexArraysOES(NUM_MESHES, _meshes);
    glBindVertexArrayOES(_meshes[MESH_FULLSCREEN]);
    
    glGenBuffers(2, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreen_quad_vertices), fullscreen_quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(_meshes[MESH_QUAD]);
    
    glGenBuffers(2, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(0);
    
    /* Create program */
    if(system_load_file("assets/Shaders/Shader.vsh", buffer, sizeof(buffer))) {
        CNSLog("Vertex shader load failed!\n");
        return;
    }
    vertex_shader = render_create_shader(GL_VERTEX_SHADER, buffer);
    
    if(system_load_file("assets/Shaders/Shader.fsh", buffer, sizeof(buffer))) {
        CNSLog("Fragment shader load failed!\n");
        return;
    }    
    fragment_shader = render_create_shader(GL_FRAGMENT_SHADER, buffer);
    _program = render_create_program(vertex_shader, fragment_shader, binds, 2);
    
    _uniforms[UNIFORM_VIEWPROJECTION_MATRIX] = glGetUniformLocation(_program, "viewProjectionMatrix");
    _uniforms[UNIFORM_WORLD_MATRIX] = glGetUniformLocation(_program, "worldMatrix");
    _uniforms[UNIFORM_TEXTURE] = glGetUniformLocation(_program, "diffuseTexture");
    _uniforms[UNIFORM_COLOR] = glGetUniformLocation(_program, "color");

    _font = [[Texture2D alloc] initWithString:@"Test" dimensions:CGSizeMake(50, 50) alignment:UITextAlignmentLeft fontName:@"Arial" fontSize:50.0f];
}
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
    if(data == NULL) {
        CNSLog("Texture %s load failed", filename);
        return 0;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texture;
}
void render_draw_fullscreen_quad(void)
{
    glUniformMatrix4fv(_uniforms[UNIFORM_WORLD_MATRIX], 1, 0, GLKMatrix4Identity.m);
    glUniformMatrix4fv(_uniforms[UNIFORM_VIEWPROJECTION_MATRIX], 1, 0, GLKMatrix4Identity.m);
    glBindVertexArrayOES(_meshes[MESH_FULLSCREEN]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
    glUniformMatrix4fv(_uniforms[UNIFORM_VIEWPROJECTION_MATRIX], 1, 0, _projectionMatrix.m);
}
void render_draw_quad(GLuint texture, float x, float y, float width, float height)
{
    GLKMatrix4 world = GLKMatrix4MakeTranslation(x, y, 0.0f);
    world = GLKMatrix4Multiply(world, GLKMatrix4MakeScale(width, height, 1.0f)); 
    
    glUniformMatrix4fv(_uniforms[UNIFORM_WORLD_MATRIX], 1, 0, world.m);
    glUniformMatrix4fv(_uniforms[UNIFORM_VIEWPROJECTION_MATRIX], 1, 0, _projectionMatrix.m);
    glBindVertexArrayOES(_meshes[MESH_QUAD]);
    glBindTexture(GL_TEXTURE_2D, [_font name]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
    glUniformMatrix4fv(_uniforms[UNIFORM_WORLD_MATRIX], 1, 0, GLKMatrix4Identity.m);
    glUniformMatrix4fv(_uniforms[UNIFORM_VIEWPROJECTION_MATRIX], 1, 0, _projectionMatrix.m);
}
void render_resize(float width, float height)
{
    _projectionMatrix = GLKMatrix4MakeOrtho(-width/2, width/2, -height/2, height/2, 0.0f, 1.0f);
}
void render_set_color(float r, float g, float b)
{
    glUniform4f(_uniforms[UNIFORM_COLOR], r, g, b, 1.0f);
}
void render_prepare(void)
{
    glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(_program);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_TEXTURE_2D);
    [_font drawAtPoint:CGPointMake(100, 100)];
    NSLog(@"%@\n", [_font description]);
}
