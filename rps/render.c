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
static bmfont_char_t    _characters[256] = {0};
static GLuint           _character_meshes[256] = {0};
static GLKMatrix4       _projectionMatrix;

typedef struct {
    float   pos[3];
    float   tex[2];
} vertex_t;
static GLuint           _quad_mesh;

/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
void render_init(void)
{
    const vertex_t vertices[] =
    {
        -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, // TL
         0.5f,  0.5f, 0.0f,     1.0f, 0.0f, // TR
         0.5f, -0.5f, 0.0f,     1.0f, 1.0f, // BR
        -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, // BL
    };
    const uint16_t indices[] = 
    {
        0, 1, 2,
        3, 2, 0,
    };
    GLuint buffers[2] = {0};
    glGenVertexArraysOES(1, &_quad_mesh);
    glBindVertexArrayOES(_quad_mesh);
    
    glGenBuffers(2, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(0);
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
void render_load_font(const char* filename)
{
    float tex_width;
    float tex_height;
    float char_height;
    int ii, jj;
    uint8_t header[4];
    const char* full_path = system_get_path(filename);
    FILE* file = fopen(full_path, "rb");
    fread(header, sizeof(header), 1, file);
    if(header[0] != 'B' || header[1] != 'M' || header[2] != 'F' || header[3] != 3) {
        fclose(file);
        return;
    }
    
    do {
        bmfont_block_type_t type;
        fread(&type, sizeof(type), 1, file);
        switch(type.type) {
        case 1: {
                bmfont_info_t block;
                fread(&block, type.size, 1, file);
                break;
            }
        case 2: {
                bmfont_common_t block;
                fread(&block, type.size, 1, file);
                tex_width = (float)block.scaleW;
                tex_height = (float)block.scaleH;
                char_height = (float)block.base/tex_height;
                break;
            }
        case 3: {
                bmfont_info_t pages;
                fread(&pages, type.size, 1, file);
                break;
            }
        case 4: {
                int ii;
                int num_chars = type.size/sizeof(bmfont_char_t);
                for(ii=0; ii<num_chars; ++ii) {
                    bmfont_char_t character;
                    fread(&character, sizeof(character), 1, file);
                    _characters[character.id] = character;
                }
                break;
            }
        case 5:
            break;
        }
    } while(!feof(file) && !ferror(file));
    
    for(ii=0;ii<256;++ii) {
        float yoffset = _characters[ii].yoffset/tex_height;
        float width = _characters[ii].width/tex_width;
        vertex_t vertices[] = 
        {
            -width/2,  char_height+yoffset, 0.0f,      _characters[ii].x,                       _characters[ii].y, // TL
             width/2,  char_height+yoffset, 0.0f,      _characters[ii].x+_characters[ii].width, _characters[ii].y, // TR
             width/2,  0.0f+yoffset, 0.0f,             _characters[ii].x+_characters[ii].width, _characters[ii].y+_characters[ii].height, // BR
            -width/2,  0.0f+yoffset, 0.0f,             _characters[ii].x,                       _characters[ii].y+_characters[ii].height, // BL
        };
        const uint16_t indices[] = 
        {
            0, 1, 2,
            3, 2, 0,
        };
        GLuint buffers[2];
        if(_characters[ii].id == 0)
            continue;
            
        for(jj=0;jj<4; ++jj) {
            vertices[jj].pos[0] /= char_height;
            vertices[jj].pos[1] /= char_height;
            vertices[jj].tex[0] /= tex_width;
            vertices[jj].tex[1] /= tex_height;
        }
        glGenVertexArraysOES(1, &_character_meshes[ii]);
        glBindVertexArrayOES(_character_meshes[ii]);
        
        glGenBuffers(2, buffers);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), BUFFER_OFFSET(12));
        
        glBindVertexArrayOES(0);
        
    }
}
void render_draw_letter(char letter, float x, float y)
{
    extern int uniform_loc;
    GLKMatrix4 model = GLKMatrix4Identity;
    model.m[12] = x;
    model.m[13] = y;
    
    model = GLKMatrix4Multiply(model, _projectionMatrix);
    //model = GLKMatrix4Multiply(_projectionMatrix, model);
    
    glUniformMatrix4fv(uniform_loc, 1, 0, model.m);
    glBindVertexArrayOES(_character_meshes[letter]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}
void render_draw_string(const char* str, float x, float y, float scale)
{
    extern int uniform_loc;
    while(*str != '\0') {
        GLKMatrix4 mat = GLKMatrix4MakeTranslation(x, y, 0.0f);
        mat.m00 = mat.m11 = mat.m22 = scale;
        mat = GLKMatrix4Multiply(mat, _projectionMatrix);
        
        glUniformMatrix4fv(uniform_loc, 1, 0, mat.m);
        glBindVertexArrayOES(_character_meshes[*str]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
        x += 1;
        ++str;
    }
}
void render_draw_fullscreen_quad(void)
{
    extern int uniform_loc;
    glUniformMatrix4fv(uniform_loc, 1, 0, GLKMatrix4Identity.m);
    glBindVertexArrayOES(_quad_mesh);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}
void render_resize(float width, float height)
{
    _projectionMatrix = GLKMatrix4MakeOrtho(-width/2, width/2, -height/2, height/2, -1.0f, 1.0f);
}
