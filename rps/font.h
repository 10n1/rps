/*! @file font.h
 *  @brief TODO: Add the purpose of this module
 *  @author Kyle Weicht
 *  @date 9/2/12 12:07 PM
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 *	@addtogroup font font
 *	@{
 */
#ifndef __font_h__
#define __font_h__

typedef enum {
    kJustifyLeft,
    kJustifyCenter,
    kJustifyRight
} text_justification_t;

void font_load(void);
void text_draw(const char* text, float x, float y, float size);
void text_draw_formatted(const char* text, text_justification_t justify, float y, float size);

/* @} */
#endif /* include guard */
