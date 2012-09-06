/*  @file game.c
 *  @brief Main game class
 *  @author Kyle Weicht
 *  @date 7/12/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include "game.h"

#include <stddef.h>
#include <GLKit/GLKMath.h>

#include "system.h"
#include "render.h"
#include "font.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a < b) ? a : b)

extern void CNSLog(const char* format,...);
/*----------------------------------------------------------------------------*\
Internal
\*----------------------------------------------------------------------------*/
static struct {
    GLuint tex;
    float x;
    float y;
    float scale;
    weapon_t weapon;
} _buttons[16] = {0};
static int _num_buttons = 0;
static GLuint _sheet_texture = 0;

static void _print_scores(game_t* game) {
    char buffer[256];
    float height = get_device_height();
    float scale = 0.35f;
    if(game->players[0].score > game->players[1].score)
        render_set_color(0.0f, 1.0f, 0.0f, 1.0f);
    else if(game->players[0].score == game->players[1].score)
        render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    else
        render_set_color(1.0f, 0.0f, 0.0f, 1.0f);        
    sprintf(buffer, "%s: %d", game->players[0].name, game->players[0].score);
    text_draw_formatted(buffer, kJustifyLeft, height/2-(128*scale), scale);

    
    if(game->players[1].score > game->players[0].score)
        render_set_color(0.0f, 1.0f, 0.0f, 1.0f);
    else if(game->players[0].score == game->players[1].score)
        render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    else
        render_set_color(1.0f, 0.0f, 0.0f, 1.0f);
    sprintf(buffer, "%s: %d", game->players[1].name, game->players[1].score);
    text_draw_formatted(buffer, kJustifyRight, height/2-(128*scale), scale);
}
static void _print_timer(float f) {
    char buffer[128];
    float floored = floorf(f);
    float opacity = f-floored;
    int i = (int)floored;
    render_set_color(1.0f, 1.0f, 1.0f, opacity);
    if(i > 3)
    sprintf(buffer, "%d", (int)i);
    else if(i == 3)
        sprintf(buffer, "Rock!");
    else if(i == 2)
        sprintf(buffer, "Paper!");
    else if(i == 1)
        sprintf(buffer, "Scissors!");
    else
        sprintf(buffer, "Shoot!");
    text_draw_formatted(buffer, kJustifyCenter, 0.0f, opacity + 1.0f);
}
static weapon_t _get_computer_move(void) {
    return rand() % 3;
}
static float lerp(float a, float b, float t) {
    return a + t*(b-a);
}

/* Paper and Scissors: http://www.Clker.com */
/* Rock: http://opengameart.org/content/rocks */
/* Font: http://www.fontsquirrel.com/fonts/TitilliumText */
/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
void game_initialize(game_t* game, float width, float height) {
    int ii;
    game->initialized = 1;
    font_load();
    /* GL setup */
    render_init();
    _buttons[0].tex = render_create_texture("assets/rock.png");
    _buttons[0].x = -0.25f;
    _buttons[0].scale = 0.25f;
    _buttons[0].weapon = kRock;
    _buttons[1].tex = render_create_texture("assets/paper.png");
    _buttons[1].x = 0.0f;
    _buttons[1].scale = 0.25f;
    _buttons[1].weapon = kPaper;
    _buttons[2].tex = render_create_texture("assets/scissors.png");
    _buttons[2].x = 0.25f;
    _buttons[2].scale = 25.0f;
    _buttons[2].weapon = kScissors;
    _num_buttons = 3;
    render_resize(width, height);
    
    for(ii=0;ii<_num_buttons;++ii) {
        float scale = width/3;
        _buttons[ii].scale = scale;
        _buttons[ii].x = ii*scale - width/2 + scale/2;
        _buttons[ii].y = -height/2 + scale/2;
    }
    _sheet_texture = render_create_texture("assets/sheet.png");
    timer_init(&game->timer);
    sprintf(game->players[0].name, "Player");
    sprintf(game->players[1].name, "Computer");
    game->players[0].selection = game->players[1].selection = kInvalid;
    game->round_timer = 4.0f;
    game->round_state = kRoundStart;
}
void game_update(game_t* game) {
    game->delta_time = (float)timer_delta_time(&game->timer);
    game->round_timer -= game->delta_time;
    game->results_timer -= game->delta_time;
    switch(game->round_state) {
        case kPause:
            return;
        case kRoundStart:
            game->round_timer = 4.0f;
            game->round_state = kRoundPicking;
            break;
        case kRoundPicking:
            game->players[1].selection = _get_computer_move();
            if(game->round_timer < 0.0f) {
                game->round_state = kRoundResults;
                game->results_timer = 3.0f;
            }
            break;
        case kRoundResults: {
            static int begin = 1;
            if(begin) {
                switch(game->players[0].selection) {
                    case kRock:
                        if(game->players[1].selection == kPaper) {
                            ++game->players[1].score;
                        } else if(game->players[1].selection == kScissors) {
                            ++game->players[0].score;
                        }
                        break;
                    case kPaper:
                        if(game->players[1].selection == kScissors) {
                            ++game->players[1].score;
                        } else if(game->players[1].selection == kRock) {
                            ++game->players[0].score;
                        }
                        break;
                    case kScissors:
                        if(game->players[1].selection == kRock) {
                            ++game->players[1].score;
                        } else if(game->players[1].selection == kPaper) {
                            ++game->players[0].score;
                        }
                        break;
                    default:
                        ++game->players[1].score;
                        break;
                }
                begin = 0;
            } else {
                if(game->results_timer < 0.0f) {
                    game->round_state = kRoundStart;
                    begin = 1;
                }
            }
            break;
        }
    }
}
void game_render(game_t* game) {
    int ii;
    render_prepare();

    _print_scores(game);
    switch(game->round_state) {
        case kPause:
            return;
        case kRoundStart:
            break;
        case kRoundPicking:
            _print_timer(game->round_timer);
            break;
        case kRoundResults: {
            GLKMatrix4 transform = GLKMatrix4Identity;
            float x, y, percent;
            percent = (3.0f-game->results_timer)/3.0f;
            x = lerp(get_device_width()/2, 0.0f, percent);
            y = 0.0f;
            render_set_color(1.0f, 1.0f, 1.0f, 1.0f);

            switch(game->players[0].selection) {
                case kRock:
                    transform = GLKMatrix4Multiply(GLKMatrix4MakeScale(100.0f, 100.0f, 1.0f), GLKMatrix4MakeZRotation(-percent*3.5f));
                    transform = GLKMatrix4Multiply(GLKMatrix4MakeTranslation(-x, y, 0.0f), transform);
                    render_draw_quad_transform(_buttons[game->players[0].selection].tex, transform);
                    break;
                case kPaper:
                    percent = (3.0f-game->results_timer)/3.0f;
                    x = lerp(get_device_width()/2, 0.0f, min(1.0f,percent*2));
                    render_draw_quad(_sheet_texture, -x, y, 100.0f, 100.0f);
                    break;
                case kScissors:
                    render_draw_quad(_buttons[game->players[0].selection].tex, -x, y, 100.0f, 100.0f);
                    break;
                default:
                    break;
            }

            percent = (3.0f-game->results_timer)/3.0f;
            x = lerp(get_device_width()/2, 0.0f, percent);
            switch(game->players[1].selection) {
                case kRock:
                    transform = GLKMatrix4Multiply(GLKMatrix4MakeScale(100.0f, 100.0f, 1.0f), GLKMatrix4MakeZRotation(percent*3.5f));
                    transform = GLKMatrix4Multiply(GLKMatrix4MakeTranslation(x, y, 0.0f), transform);
                    render_draw_quad_transform(_buttons[game->players[1].selection].tex, transform);
                    break;
                case kPaper:
                case kScissors:
                    render_draw_quad(_buttons[game->players[1].selection].tex, x, y, 100.0f, 100.0f);
                    break;
                default:
                    break;
            }
            break;
        }
    }
    render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    for(ii=0;ii<_num_buttons;++ii) {
        if(_buttons[ii].weapon == game->players[0].selection)
            render_set_color(1.0f, 0.9f, 0.9f, 1.0f);
        else
            render_set_color(1.0f, 1.0f, 1.0f, 0.65f);
        render_draw_quad(_buttons[ii].tex,
                         _buttons[ii].x,
                         _buttons[ii].y,
                         _buttons[ii].scale,
                         _buttons[ii].scale);
    }
}
void game_shutdown(game_t* game) {
    game->initialized = 0;
}
void game_handle_tap(game_t* game, float x, float y) {
    int ii;
    if(game->round_state != kRoundPicking)
        return;
    x -= get_device_width()/2;
    y = -y + get_device_height()/2;

    for (ii=0; ii<_num_buttons; ++ii) {
        float l = _buttons[ii].x - _buttons[ii].scale/2;
        float r = _buttons[ii].x + _buttons[ii].scale/2;
        float b = _buttons[ii].y - _buttons[ii].scale/2;
        float t = _buttons[ii].y + _buttons[ii].scale/2;
        if(x > l && x <= r && y > b && y <= t)
        {
            game->players[0].selection = _buttons[ii].weapon;
            break;
        }
    }
}
void game_pause(game_t* game) {
    game->prev_state = game->round_state;
    game->round_state = kPause;
}
void game_resume(game_t* game) {
    timer_reset(&game->timer);
    game->round_state = game->prev_state;
}
