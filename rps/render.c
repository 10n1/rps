/*! @file render.c
 *  @brief Rendering
 *  @author Kyle Weicht
 *  @date 7/11/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "rps_math.h"
#include "stb_image.h"
#include "system.h"
#include "render.h"


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

    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE && 0) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            char *log = (char*)malloc((size_t)logLength);
            glGetProgramInfoLog(program, logLength, &logLength, (GLchar*)log);
            CNSLog("Program validate log:%s", log);
            free(log);
            return 1;
        }
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

static GLuint               _character_meshes[256] = {0};
static float4x4             _projectionMatrix[kNumProjectionTypes];
static GLuint               _program = 0;
static GLuint               _meshes[NUM_MESHES] = {0};
static GLint                _uniforms[NUM_UNIFORMS] = {-1};
static projection_type_t    _current_projection = kOrthographic;

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
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texture;
}
void render_set_projection_matrix(projection_type_t type) {
    _current_projection = type;
}
void render_draw_fullscreen_quad(void)
{
    float4x4 mIdentity = float4x4identity();
    glUniformMatrix4fv(_uniforms[UNIFORM_WORLD_MATRIX], 1, 0, (const GLfloat*)&mIdentity);
    glUniformMatrix4fv(_uniforms[UNIFORM_VIEWPROJECTION_MATRIX], 1, 0, (const GLfloat*)&mIdentity);
    
    glBindVertexArrayOES(_meshes[MESH_FULLSCREEN]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
    
    glUniformMatrix4fv(_uniforms[UNIFORM_VIEWPROJECTION_MATRIX], 1, 0, (const GLfloat*)&_projectionMatrix[_current_projection]);
}
void render_draw_quad_transform(GLuint texture, const float4x4* transform) {
    glUniformMatrix4fv(_uniforms[UNIFORM_WORLD_MATRIX], 1, 0, (const GLfloat*)transform);
    glUniformMatrix4fv(_uniforms[UNIFORM_VIEWPROJECTION_MATRIX], 1, 0, (const GLfloat*)&_projectionMatrix[_current_projection]);
    glBindVertexArrayOES(_meshes[MESH_QUAD]);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}
void render_draw_quad(GLuint texture, float x, float y, float width, float height)
{
    render_draw_custom_quad(texture, _meshes[MESH_QUAD], x, y, width, height);
}
void render_draw_custom_quad(GLuint texture, GLuint vao, float x, float y, float width, float height) {
    float4x4 mWorld = float4x4translation(x, y, 0.0f);
    float4x4 mScale = float4x4Scale(width, height, 1.0f);
    mWorld = float4x4multiply(&mScale, &mWorld);
    
    glUniformMatrix4fv(_uniforms[UNIFORM_WORLD_MATRIX], 1, 0, (const GLfloat*)&mWorld);
    glUniformMatrix4fv(_uniforms[UNIFORM_VIEWPROJECTION_MATRIX], 1, 0, (const GLfloat*)&_projectionMatrix[_current_projection]);
    
    glBindVertexArrayOES(vao);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}
void render_resize(float width, float height)
{
    _projectionMatrix[kOrthographic] = float4x4OrthographicLH(width, height, 0.0f, 1.0f);
    _projectionMatrix[kPerspective] = float4x4PerspectiveFovLH(DegToRad(50.0f), width/height, 0.1f, 100.0f);
}
void render_set_colorf(float r, float g, float b, float a)
{
    glUniform4f(_uniforms[UNIFORM_COLOR], r, g, b, a);
}
void render_set_colorfv(float* c) {
    glUniform4fv(_uniforms[UNIFORM_COLOR], 1, c);
}
void render_prepare(void)
{
    glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(_program);
}
