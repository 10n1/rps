/*! @file ui.h
 *  @brief UI functionality
 *  @author Kyle Weicht
 *  @date 9/8/12 10:21 PM
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 *	@addtogroup ui ui
 *	@{
 */
#ifndef __ui_h__
#define __ui_h__

#include "render.h"

typedef struct {
    float r;
    float g;
    float b;
    float a;
} color_t;

static const color_t kWhite = {1.0f, 1.0f, 1.0f, 1.0f};
static const color_t KRed =   {1.0f, 0.0f, 0.0f, 1.0f};
static const color_t kGreen = {0.0f, 1.0f, 0.0f, 1.0f};
static const color_t kBlue =  {0.0f, 0.0f, 1.0f, 1.0f};
static const color_t kBlack = {0.0f, 0.0f, 0.0f, 1.0f};

typedef enum {
    kJustifyLeft,
    kJustifyCenter,
    kJustifyRight
} ui_justify_t;

typedef union {
    void* ptr;
    intptr_t i;
    float f;
    const char* str;
} ui_param_t;
typedef void (ui_callback_t)(ui_param_t*);

typedef struct {
    const char* text;
    GLuint tex;
    float x;
    float y;
    float width;
    float height;
    float font_size;
    int active;
    color_t color;
    ui_callback_t*  callback;
    ui_param_t      params[4];
} button_t;

void ui_init(void);
void ui_draw_text(const char* text, float x, float y, float size);
void ui_draw_text_formatted(const char* text, ui_justify_t justify, float y, float size);
float ui_text_size(void);
float ui_text_width(const char* text);
button_t* ui_create_button_text(const char* text, float x, float y, float size);
button_t* ui_create_button_texture(GLuint tex, float x, float y, float width, float height);
void ui_render(void);
void ui_tap(float x, float y);

/* @} */
#endif /* include guard */
