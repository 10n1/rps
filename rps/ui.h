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

typedef enum {
    kJustifyLeft,
    kJustifyCenter,
    kJustifyRight
} ui_justify_t;

void ui_init(void);
void ui_draw_text(const char* text, float x, float y, float size);
void ui_draw_text_formatted(const char* text, ui_justify_t justify, float y, float size);
int ui_text_size(void);

/* @} */
#endif /* include guard */
