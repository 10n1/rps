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

extern void CNSLog(const char* format,...);
/*----------------------------------------------------------------------------*\
Internal
\*----------------------------------------------------------------------------*/
static float _width     = 0;
static float _height    = 0;
static struct {
    GLuint tex;
    float x;
    float y;
    float scale;
    weapon_t weapon;
} _buttons[16] = {0};
static int _num_buttons = 0;
static struct {
    char name[128];
    weapon_t selection;
    int score;
} _players[2] = {0};

static void _print_scores(void) {
    char buffer[256];
    float scale = 0.35f;
    if(_players[0].score > _players[1].score)
        render_set_color(0.0f, 1.0f, 0.0f, 1.0f);
    else if(_players[0].score == _players[1].score)
        render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    else
        render_set_color(1.0f, 0.0f, 0.0f, 1.0f);        
    sprintf(buffer, "%s: %d", _players[0].name, _players[0].score);
    draw_text_formatted(buffer, kJustifyLeft, _height/2-(128*scale), scale);

    
    if(_players[1].score > _players[0].score)
        render_set_color(0.0f, 1.0f, 0.0f, 1.0f);
    else if(_players[0].score == _players[1].score)
        render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    else
        render_set_color(1.0f, 0.0f, 0.0f, 1.0f);
    sprintf(buffer, "%s: %d", _players[1].name, _players[1].score);
    draw_text_formatted(buffer, kJustifyRight, _height/2-(128*scale), scale);
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
    draw_text_formatted(buffer, kJustifyCenter, 0.0f, opacity*2.0f);
}
static weapon_t _get_computer_move(void) {
    return rand() % 3;
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
    load_font();
    /* GL setup */
    render_init();
    _buttons[0].tex = render_create_texture("assets/rock.png");
    _buttons[0].x = -0.25f;
    _buttons[0].scale = 0.25f;
    _buttons[0].weapon = kRock;
    _buttons[1].tex = render_create_texture("assets/paper.png");
    _buttons[1].x = 0.0f;
    _buttons[1].scale = 0.25f;
    _buttons[0].weapon = kPaper;
    _buttons[2].tex = render_create_texture("assets/scissors.png");
    _buttons[2].x = 0.25f;
    _buttons[2].scale = 25.0f;
    _buttons[2].weapon = kScissors;
    _num_buttons = 3;
    _width = width;
    _height = height;
    render_resize(width, height);
    
    for(ii=0;ii<_num_buttons;++ii) {
        float scale = width/3;
        _buttons[ii].scale = scale;
        _buttons[ii].x = ii*scale - width/2 + scale/2;
        _buttons[ii].y = -height/2 + scale/2;
    }

    timer_init(&game->timer);
    sprintf(_players[0].name, "Player");
    sprintf(_players[1].name, "Computer");
    _players[0].selection = _players[1].selection = kInvalid;
    game->round_timer = 4.0f;
    game->round_state = kRoundStart;
}
void game_update(game_t* game) {
    game->delta_time = (float)timer_delta_time(&game->timer);
    game->round_timer -= game->delta_time;
    game->results_timer -= game->delta_time;
    switch(game->round_state) {
        case kRoundStart:
            game->round_timer = 4.0f;
            game->round_state = kRoundPicking;
            break;
        case kRoundPicking:
            _players[1].selection = _get_computer_move();
            if(game->round_timer < 0.0f) {
                game->round_state = kRoundResults;
                game->results_timer = 3.0f;
            }
            break;
        case kRoundResults:
            switch(_players[0].selection) {
                case kRock:
                    if(_players[1].selection == kPaper) {
                        ++_players[1].score;
                    } else if(_players[1].selection == kScissors) {
                        ++_players[0].score;
                    }
                    break;
                case kPaper:
                    if(_players[1].selection == kScissors) {
                        ++_players[1].score;
                    } else if(_players[1].selection == kRock) {
                        ++_players[0].score;
                    }
                    break;
                case kScissors:
                    if(_players[1].selection == kRock) {
                        ++_players[1].score;
                    } else if(_players[1].selection == kPaper) {
                        ++_players[0].score;
                    }
                    break;
                default:
                    ++_players[1].score;
                    break;
            }
            if(game->results_timer < 0.0f)
                game->round_state = kRoundStart;
            break;
    }
}
void game_render(game_t* game) {
    int ii;
    render_prepare();

    _print_scores();
    _print_timer(game->round_timer);
    render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    for(ii=0;ii<_num_buttons;++ii) {
        if(_buttons[ii].weapon == _players[0].selection)
            render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
        else            
            render_set_color(1.0f, 1.0f, 1.0f, 0.75f);
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
    x -= _width/2;
    y = -y + _height/2;
    printf("X: %f\tY: %f\n", x, y);

    for (ii=0; ii<_num_buttons; ++ii) {
        float l = _buttons[ii].x - _buttons[ii].scale/2;
        float r = _buttons[ii].x + _buttons[ii].scale/2;
        float b = _buttons[ii].y - _buttons[ii].scale/2;
        float t = _buttons[ii].y + _buttons[ii].scale/2;
        if(x > l && x <= r && y > b && y <= t)
        {
            _players[0].selection = _buttons[ii].weapon;
            break;
        }
    }
}
