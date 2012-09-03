/*! @file font.c
 *  @author Kyle Weicht
 *  @date 9/2/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "font.h"

#include <stdio.h>

#include "system.h"
#include "render.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

extern float get_device_scale(void);

/*
 * Internal 
 */
enum {
    kBMFontInfoBlock = 1,
    kBMFontCommonBlock = 2,
    kBMFontPagesBlock = 3,
    kBMFontCharsBlock = 4,
    kBMFontKerningBlock = 5
};
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
    int16_t     xoffset;
    int16_t     yoffset;
    int16_t     xadvance;
    uint8_t     page;
    uint8_t     chnl;
} bmfont_char_t;

typedef struct {
    uint32_t    first;
    uint32_t    second;
    int16_t     amount;
} bmfont_kerning_pairs_t;
#pragma pack(pop)

static GLuint   _font_texture = 0;
static bmfont_info_t    _font_info = {0};
static bmfont_common_t  _font_common = {0};
static bmfont_char_t    _font_chars[256] = {0};
static GLuint           _char_meshes[256] = {0};

/*
 * External
 */
void load_font(void) {
    int ii, jj;
    uint8_t header[4];
    FILE* file = fopen(system_get_path("assets/verdanas.fnt"), "rb");
    fread(header, sizeof(header), 1, file);
    if(header[0] != 'B' || header[1] != 'M' || header[2] != 'F' || header[3] != 3) {
        fclose(file);
        return;
    }
    
    do {
        bmfont_block_type_t type;
        fread(&type, sizeof(type), 1, file);
        switch(type.type) {
        case kBMFontInfoBlock: {
                fread(&_font_info, type.size, 1, file);
                break;
            }
        case kBMFontCommonBlock: {
                fread(&_font_common, type.size, 1, file);
                break;
            }
        case kBMFontPagesBlock: {
                char font_texture[256];
                bmfont_pages_t pages;
                fread(&pages, type.size, 1, file);
                sprintf(font_texture, "assets/%s", pages.pageNames[0]);
                _font_texture = render_create_texture(font_texture);
                break;
            }
        case kBMFontCharsBlock: {
                int ii;
                int num_chars = type.size/sizeof(bmfont_char_t);
                for(ii=0; ii<num_chars; ++ii) {
                    bmfont_char_t character;
                    fread(&character, sizeof(character), 1, file);
                    if(character.id == 'A')
                        _font_chars[character.id] = character;
                    _font_chars[character.id] = character;
                }
                break;
            }
        case kBMFontKerningBlock:
            break;
        }
    } while(!feof(file) && !ferror(file));
    fclose(file);

    for(ii=0;ii<256;++ii) {
        bmfont_char_t c = _font_chars[ii];
        vertex_t quad_vertices[] =
        {
            0.0f,    c.height, 0.0f,     c.x,         c.y,          // TL
            c.width, c.height, 0.0f,     c.x+c.width, c.y,          // TR
            c.width, 0.0f,     0.0f,     c.x+c.width, c.y+c.height, // BR
            0.0f,    0.0f,     0.0f,     c.x,         c.y+c.height, // BL
        };
        const uint16_t indices[] =
        {
            0, 1, 2,
            3, 2, 0,
        };
        GLuint buffers[2] = {0};
        if(_font_chars[ii].id == 0)
            continue;
        glGenVertexArraysOES(1, &_char_meshes[ii]);
        glBindVertexArrayOES(_char_meshes[ii]);

        for (jj=0; jj<4; ++jj) {
            quad_vertices[jj].tex[0] /= (float)_font_common.scaleW;
            quad_vertices[jj].tex[1] /= (float)_font_common.scaleH;
        }

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
    }
}
void draw_text(const char* text, float x, float y, float size) {
    float draw_x = x;
    float draw_y = y;
    size *= get_device_scale()/2;
    while(text && *text) {
        char c = *text;
        bmfont_char_t glyph = _font_chars[c];
        if(c == '\n') {
            draw_y -= _font_common.lineHeight*size;
            draw_x = x;
            ++text;
            continue;
        } else if(c != ' ') {
            render_draw_custom_quad(_font_texture, _char_meshes[c],
                                    draw_x+glyph.xoffset*size,
                                    draw_y-(glyph.height+glyph.yoffset-_font_common.lineHeight)*size,
                                    size, size);
        }
        draw_x += glyph.xadvance * size;
        ++text;
    }
}
