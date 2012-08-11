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
void render_draw_quad(float x, float y, float width, float height);
void render_draw_fullscreen_quad(void);
void render_draw_letter(char letter, float x, float y, float scale);
void render_draw_string(const char* str, float x, float y, float scale);
void render_resize(float width, float height);

void render_load_font(const char* filename);

void render_prepare(void);

#pragma pack(push,1)
typedef struct {
    uint8_t     type;
    uint32_t    size;
} bmfont_block_type_t;

typedef struct {
    int16_t     fontSize;
    uint8_t     bitField; /* bit 0: smooth, bit 1: unicode, bit 2: italic, bit 3: bold, bit 4: fixedHeigth, bits 5-7: reserved */
    uint8_t     charSet;
    uint16_t    stretchH;
    uint8_t     aa;
    uint8_t     paddingUp;
    uint8_t     paddingRight;
    uint8_t     paddingDown;
    uint8_t     paddingLeft;
    uint8_t     spacingHoriz;
    uint8_t     spacingVert;
    uint8_t     outline;
    char        name[128];
} bmfont_info_t;

typedef struct {
    uint16_t    lineHeight;
    uint16_t    base;
    uint16_t    scaleW;
    uint16_t    scaleH;
    uint16_t    pages;
    uint8_t     bitField;
    uint8_t     alphaChnl;
    uint8_t     redChnl;
    uint8_t     greenChnl;
    uint8_t     blueChnl;    
} bmfont_common_t;

typedef struct {
    char    pageNames[128][8];
} bmfont_pages_t;

typedef struct {
    uint32_t    id;
    uint16_t    x;
    uint16_t    y;
    uint16_t    width;
    uint16_t    height;
    uint16_t    xoffset;
    uint16_t    yoffset;
    uint16_t    xadvance;
    uint8_t     page;
    uint8_t     chnl;
} bmfont_char_t;
#pragma pack(pop)

#endif /* include guard */
