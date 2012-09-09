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
//#include "font.h"
#include "ui.h"

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
    float scale = 1.0f;
    if(game->player.score > 0.0f) {
        render_set_colorf(0.0f, 0.8f, 0.0f, 1.0f);
        sprintf(buffer, "%c%d",'+', game->player.score);
    } else if(game->player.score < 0.0f) {
        render_set_colorf(0.8f, 0.0f, 0.0f, 1.0f);
        sprintf(buffer, "%d", game->player.score);
    } else {
        render_set_colorf(1.0f, 1.0f, 1.0f, 1.0f);
        sprintf(buffer, "%d", game->player.score);
    }
    ui_draw_text_formatted(buffer, kJustifyCenter, height/2-(ui_text_size()*scale), scale);
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
static void _player_selection(ui_param_t* p) {
    game_t* game = p[0].ptr;
    weapon_t weapon = p[1].i;
    game->player.selection = weapon;
}

/* Paper and Scissors: http://www.Clker.com */
/* Rock: http://opengameart.org/content/rocks */
/* Font: http://www.fontsquirrel.com/fonts/TitilliumText */
/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
void game_initialize(game_t* game, float width, float height) {
    int ii;
    button_t* button;
    /* GL setup */
    render_init();
    ui_init();
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
    _num_buttons = 2;
    render_resize(width, height);

    for(ii=0;ii<3;++ii) {
        float scale = width/3;
        _buttons[ii].scale = scale;
        _buttons[ii].x = ii*scale - width/2 + scale/2;
        _buttons[ii].y = -height/2 + scale/2;
    }
    game->player.selection = kInvalid;
    game->current_weapon.weapon = _get_computer_move();
    game->current_weapon.timer = 2.0f;
    game->state = kPause;

    _white_texture = render_create_texture("assets/white.png");

    button = ui_create_button_texture(render_create_texture("assets/pause.png"),
                                      -width/2 + 50.0f,
                                      height/2 - 50.0f,
                                      50.0f,
                                      50.0f);
    button->callback = (ui_callback_t*)game_toggle_pause;
    button->params[0].ptr = game;

    button = ui_create_button_texture(render_create_texture("assets/scissors.png"),
                                      (2*width/3)-width/2 + width/6,
                                      -height/2 + width/6,
                                      width/3,
                                      width/3);
    button->callback = _player_selection;
    button->params[0].ptr = game;
    button->params[1].i = kScissors;

    timer_init(&game->timer);
    srand((int32_t)game->timer.start_time);
    game->initialized = 1;
}
void game_update(game_t* game) {
    game->delta_time = (float)timer_delta_time(&game->timer);
    if(game->state == kPause)
        return;

    if(game->pause_timer > 0.0f) {
        game->pause_timer -= game->delta_time;
        return;
    }
    
    game->speed = 1.0f + (game->player.score/10)*0.2f;
    game->speed = max(game->speed, 0.1f);
    game->current_weapon.timer -= (game->delta_time*game->speed);
    if(game->current_weapon.timer <= 0.0f) {
        if(_get_winner(game->player.selection, game->current_weapon.weapon) == 1)
            game->player.score += 1;
        else
            game->player.score -= 1;
        game->current_weapon.weapon = _get_computer_move();
        game->current_weapon.timer = 2.0f;
    }
}
void game_render(game_t* game) {
    int ii;
    render_prepare();

    _print_scores(game);
    render_set_colorf(1.0f, 1.0f, 1.0f, 1.0f);
    render_draw_quad(_buttons[game->current_weapon.weapon].tex,
                     lerp(-get_device_width()/2, get_device_width()/2, 1-(game->current_weapon.timer/2.0f)),
                     0.0f,
                     75.0f*get_device_scale(),
                     75.0f*get_device_scale());

    render_set_colorf(1.0f, 1.0f, 1.0f, 1.0f);
    for(ii=0;ii<_num_buttons;++ii) {
        if(_buttons[ii].weapon == game->player.selection)
            render_set_colorf(1.0f, 0.9f, 0.9f, 1.0f);
        else
            render_set_colorf(1.0f, 1.0f, 1.0f, 0.65f);
        render_draw_quad(_buttons[ii].tex,
                         _buttons[ii].x,
                         _buttons[ii].y,
                         _buttons[ii].scale,
                         _buttons[ii].scale);
    }

    if(game->state == kPause) {
        render_set_colorf(0.0f, 0.0f, 0.0f, 0.5f);
        glBindTexture(GL_TEXTURE_2D, _white_texture);
        render_draw_fullscreen_quad();
        render_set_colorf(1.0f, 1.0f, 1.0f, 1.0f);
        ui_draw_text_formatted("Paused", kJustifyCenter, 100.0f, 1.5f);
    } else if(game->pause_timer > 0.0f) {
        char buffer[32];
        sprintf(buffer, "%.1f", game->pause_timer);
        render_set_colorf(0.0f, 0.0f, 0.0f, 0.5f);
        glBindTexture(GL_TEXTURE_2D, _white_texture);
        render_draw_fullscreen_quad();
        render_set_colorf(1.0f, 1.0f, 1.0f, 1.0f);
        ui_draw_text_formatted("Paused", kJustifyCenter, 100.0f, 1.5f);
        ui_draw_text_formatted(buffer, kJustifyCenter, 40.0f, 1.0f);
    }

    ui_render();
}
void game_shutdown(game_t* game) {
    game->initialized = 0;
}
void game_handle_tap(game_t* game, float x, float y) {
    int ii;
    ui_tap(x, y);
    x -= get_device_width()/2;
    y = -y + get_device_height()/2;

    for (ii=0; ii<_num_buttons; ++ii) {
        float l = _buttons[ii].x - _buttons[ii].scale/2;
        float r = _buttons[ii].x + _buttons[ii].scale/2;
        float b = _buttons[ii].y - _buttons[ii].scale/2;
        float t = _buttons[ii].y + _buttons[ii].scale/2;
        if(x > l && x <= r && y > b && y <= t)
        {
            if(ii < 3)
                game->player.selection = _buttons[ii].weapon;
            break;
        }
    }
}
void game_toggle_pause(ui_param_t* p) {
    game_t* game = p[0].ptr;
    if(game->state == kPause)
        game_resume(game);
    else
        game_pause(game);
}
void game_pause(game_t* game) {
    game->state = kPause;
    game->pause_timer = 3.0f;
}
void game_resume(game_t* game) {
    game->state = kGame;
    timer_init(&game->timer);
}
