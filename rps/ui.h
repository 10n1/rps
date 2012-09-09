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

typedef enum {
    kJustifyLeft,
    kJustifyCenter,
    kJustifyRight
} ui_justify_t;

typedef void (ui_callback_t)(void*);

typedef struct {
    const char* text;
    GLuint tex;
    float x;
    float y;
    float width;
    float height;
    int active;
    float color[4];
    ui_callback_t*  callback;
    void*           userdata;
} button_t;

void ui_init(void);
void ui_draw_text(const char* text, float x, float y, float size);
void ui_draw_text_formatted(const char* text, ui_justify_t justify, float y, float size);
int ui_text_size(void);
//button_t* ui_create_button_text(const char* text, float x, float y, float size);
button_t* ui_create_button_texture(GLuint tex, float x, float y, float width, float height);
void ui_render(void);
void ui_tap(float x, float y);

/* @} */
#endif /* include guard */
