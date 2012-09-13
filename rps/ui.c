/*! @file ui.c
 *  @author Kyle Weicht
 *  @date 9/8/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "ui.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "render.h"
#include "system.h"

/*
 * Internal
 */

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

static GLuint           _font_textures[16] = {0};
static bmfont_info_t    _font_info = {0};
static bmfont_common_t  _font_common = {0};
static bmfont_char_t    _font_chars[256] = {0};
static GLuint           _char_meshes[256] = {0};
static int              _char_textures[256] = {0};
static button_t         _buttons[256] = {0};
static int              _num_buttons = 0;
static float            _scale = 0.0f;

static void _load_font(const char* filename) {
    int ii, jj;
    uint8_t header[4];
    FILE* file = fopen(system_get_path(filename), "rb");
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
                char font_texture[256] = {0};
                char pagenames[1024] = {0};
                const char* curr_pagename = pagenames;
                int ii=0;
                fread(&pagenames, type.size, 1, file);
                while(strlen(curr_pagename))
                {
                    sprintf(font_texture, "assets/%s", curr_pagename);
                    _font_textures[ii++] = render_create_texture(font_texture);
                    while(*curr_pagename != '\0')
                        ++curr_pagename;
                    ++curr_pagename;
                }
                break;
            }
        case kBMFontCharsBlock: {
                int ii;
                int num_chars = type.size/sizeof(bmfont_char_t);
                for(ii=0; ii<num_chars; ++ii) {
                    bmfont_char_t character;
                    fread(&character, sizeof(character), 1, file);
                    _font_chars[character.id] = character;
                }
                break;
            }
        case kBMFontKerningBlock:
            break;
        }
    } while(!feof(file) && !ferror(file));
    fclose(file);

    _scale = 64.0f/_font_common.lineHeight;
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

        _char_textures[ii] = _font_textures[c.page];
    }
}
static const char* _draw_line(const char* text, float x, float y, float size) {
    while(text && *text) {
        char c = *text;
        bmfont_char_t glyph = _font_chars[c];
        if(c == '\n') {
            return text+1;
        }
        if(c != ' ') {
            render_draw_custom_quad(_char_textures[c], _char_meshes[c],
                                    x+glyph.xoffset*size,
                                    y-(glyph.height+glyph.yoffset-_font_common.lineHeight)*size,
                                    size, size);
        }
        x += glyph.xadvance * size;
        ++text;
    }
    return NULL;
}

/*
 * External
 */
void ui_init(void) {
    _load_font("assets/andika.fnt");
}
void ui_draw_text(const char* text, float x, float y, float size) {
    size *= _scale * get_device_scale()/2;
    while(text && *text) {
        text = _draw_line(text, x, y, size);
        if(text)
            y -= _font_common.lineHeight*size;
    }
}
void ui_draw_text_formatted(const char* text, ui_justify_t justify, float y, float size) {
    const char* orig_text = text;
    float draw_x = 0.0f;
    float draw_y = y;
    float total_width = 0;
    size *= _scale * get_device_scale()/2;
    while(text && *text) {
        char c = *text;
        bmfont_char_t glyph = _font_chars[c];
        total_width += glyph.xadvance * size;
        ++text;
    }
    text = orig_text;
    switch(justify) {
        case kJustifyCenter: {
            draw_x -= total_width/2;
            break;
        }
        case kJustifyLeft: {
            draw_x = -get_device_width()/2 + 2*get_device_scale();
            break;
        }
        default:
            draw_x = get_device_width()/2-total_width - 2*get_device_scale();
            break;
    }
    while(text && *text) {
        text = _draw_line(text, draw_x, draw_y, size);
        if(text)
            y -= _font_common.lineHeight*size;
    }
}
int ui_text_size(void) {
    return _font_common.lineHeight * (int)(get_device_scale()/2 * _scale);
}
button_t* ui_create_button_text(const char* text, float x, float y, float size) {
    button_t* button = &_buttons[_num_buttons++];
    button->text = text;
    button->tex = 0;
    button->x = x;
    button->y = y;
    button->width = size;
    button->height = size;
    button->active = 1;
    return button;
}
button_t* ui_create_button_texture(GLuint tex, float x, float y, float width, float height) {
    button_t* button = &_buttons[_num_buttons++];
    button->tex = tex;
    button->text = NULL;
    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    button->active = 1;
    button->color.r = button->color.g = button->color.b = button->color.a = 1.0f;
    return button;
}
void ui_render(void) {
    int ii;
    for(ii=0;ii<_num_buttons;++ii) {
        button_t* button = _buttons + ii;
        if(!button->active)
            continue;

        render_set_colorfv((float*)&button->color);
        if(button->text) {
            ui_draw_text(button->text, button->x, button->y, button->height);
        } else {
            render_draw_quad(button->tex,
                             button->x,
                             button->y,
                             button->width,
                             button->height);
        }
    }
}
void ui_tap(float x, float y) {
    int ii;
    x -= get_device_width()/2;
    y = -y + get_device_height()/2;

    for (ii=0; ii<_num_buttons; ++ii) {
        float l, r, b, t;
        if(!_buttons[ii].active || !_buttons[ii].callback)
            continue;
        l = _buttons[ii].x - _buttons[ii].width/2;
        r = _buttons[ii].x + _buttons[ii].width/2;
        b = _buttons[ii].y - _buttons[ii].height/2;
        t = _buttons[ii].y + _buttons[ii].height/2;
        if(x > l && x <= r && y > b && y <= t)
        {
            _buttons[ii].callback(_buttons[ii].params);
            break;
        }
    }
}
