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
static GLuint _white_texture = 0;

static void _print_scores(game_t* game) {
    char buffer[256];
    float height = get_device_height();
    float scale = 0.8f;
    if(game->player.score > 0.0f) {
        render_set_color(0.0f, 0.8f, 0.0f, 1.0f);
        sprintf(buffer, "%c%d",'+', game->player.score);
    } else if(game->player.score < 0.0f) {
        render_set_color(0.8f, 0.0f, 0.0f, 1.0f);
        sprintf(buffer, "%d", game->player.score);
    } else {
        render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
        sprintf(buffer, "%d", game->player.score);
    }
    text_draw_formatted(buffer, kJustifyCenter, height/2-(128*scale), scale);
}
static weapon_t _get_computer_move(void) {
    return rand() % 3;
}
static float lerp(float a, float b, float t) {
    return a + t*(b-a);
}
static int _get_winner(weapon_t player_one, weapon_t player_two) {
    switch(player_one) {
        case kRock:
            if(player_two == kPaper)
                return 2;
            else if(player_two == kScissors)
                return 1;
        case kPaper:
            if(player_two == kScissors)
                return 2;
            else if(player_two == kRock)
                return 1;
        case kScissors:
            if(player_two == kRock)
                return 2;
            else if(player_two == kPaper)
                return 1;
        default:
            return 2;
    }
    return 0;
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
    timer_init(&game->timer);
    game->player.selection = kInvalid;
    game->current_weapon.weapon = _get_computer_move();
    game->current_weapon.timer = 2.0f;
    game->state = kPause;

    _white_texture = render_create_texture("assets/white.png");
}
void game_update(game_t* game) {
    game->speed = 1.0f + (game->player.score/10)*0.2f;
    game->speed = max(game->speed, 0.1f);
    
    game->delta_time = (float)timer_delta_time(&game->timer);
    game->current_weapon.timer -= (game->delta_time*game->speed);
    if(game->current_weapon.timer <= 0.0f) {
        if(_get_winner(game->player.selection, game->current_weapon.weapon) == 1)
            game->player.score += 1;
        else
            game->player.score -= 1;
        game->current_weapon.weapon = _get_computer_move();
        game->current_weapon.timer = 2.0f;
        game->player.selection = kInvalid;
    }
}
void game_render(game_t* game) {
    int ii;
    render_prepare();

    _print_scores(game);
    render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    render_draw_quad(_buttons[game->current_weapon.weapon].tex,
                     lerp(-get_device_width()/2, get_device_width()/2, 1-(game->current_weapon.timer/2.0f)),
                     0.0f,
                     150.0f,
                     150.0f);

    render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    for(ii=0;ii<_num_buttons;++ii) {
        if(_buttons[ii].weapon == game->player.selection)
            render_set_color(1.0f, 0.9f, 0.9f, 1.0f);
        else
            render_set_color(1.0f, 1.0f, 1.0f, 0.65f);
        render_draw_quad(_buttons[ii].tex,
                         _buttons[ii].x,
                         _buttons[ii].y,
                         _buttons[ii].scale,
                         _buttons[ii].scale);
    }

    if(game->state == kPause) {
        render_set_color(0.0f, 0.0f, 0.0f, 0.5f);
        glBindTexture(GL_TEXTURE_2D, _white_texture);
        render_draw_fullscreen_quad();
        render_set_color(1.0f, 1.0f, 1.0f, 1.0f);
        text_draw_formatted("Paused", kJustifyCenter, 100.0f, 1.5f);
    }
}
void game_shutdown(game_t* game) {
    game->initialized = 0;
}
void game_handle_tap(game_t* game, float x, float y) {
    int ii;
    x -= get_device_width()/2;
    y = -y + get_device_height()/2;

    for (ii=0; ii<_num_buttons; ++ii) {
        float l = _buttons[ii].x - _buttons[ii].scale/2;
        float r = _buttons[ii].x + _buttons[ii].scale/2;
        float b = _buttons[ii].y - _buttons[ii].scale/2;
        float t = _buttons[ii].y + _buttons[ii].scale/2;
        if(x > l && x <= r && y > b && y <= t)
        {
            game->player.selection = _buttons[ii].weapon;
            break;
        }
    }
}
void game_pause(game_t* game) {
    //game->state = kPause;
}
void game_resume(game_t* game) {
    //game->state = kGame;
}
