/*  @file game.c
 *  @brief Main game class
 *  @author Kyle Weicht
 *  @date 7/12/12
 *  @copyright Copyright (c) 2012 Kyle Weicht. All rights reserved.
 */
#include <stddef.h>
#include <stdio.h>

#include "rps_math.h"
#include "system.h"
#include "render.h"
#include "ui.h"
#include "game.h"


#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a < b) ? a : b)

extern void CNSLog(const char* format,...);
/*----------------------------------------------------------------------------*\
Internal
\*----------------------------------------------------------------------------*/
static const float kBaseWeaponTimer = 1.5f;
static const int kWeaponTable[kNumWeapons][kNumWeapons] =
{     //   R   P   S  
/* R */ {  0, -1, +1 },
/* P */ { +1,  0, -1 },
/* S */ { -1, +1,  0 },
};

static GLuint _logo_texture = 0;
static GLuint _weapon_textures[kNumWeapons] = {0};
static GLuint _button_texture = 0;

static void _print_scores(game_t* game) {
    char buffer[256];
    float height = get_device_height();
    float scale = 2.0f;
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
    ui_draw_text_formatted(buffer, kJustifyRight, height/2-ui_text_size()*scale + 12*get_device_scale(), scale);
}
static weapon_t _get_computer_move(void) {
    return rand() % 3;
}
static float lerp(float a, float b, float t) {
    return a + t*(b-a);
}
static int _get_winner(weapon_t player_one, weapon_t player_two) {
    int result = kWeaponTable[player_one][player_two];
    if(player_one == kInvalid)
        return -1;
    return result;
}
static void _player_selection(ui_param_t* p) {
    game_t* game = p[0].ptr;
    weapon_t weapon = p[1].i;
    int ii;
    for(ii=0;ii<kNumWeapons;++ii) {
        if(ii == weapon)
            game->weapon_buttons[ii]->color = KRed;
        else
            game->weapon_buttons[ii]->color = kWhite;
    }
    game->player.selection = weapon;
}

static void _game_start(ui_param_t* p) {
    game_t* game = p[0].ptr;
    int ii;
    game->state = kGame;
    
    for(ii=0;ii<kNumWeapons;++ii) {
        game->weapon_buttons[ii]->active = 1;
    }
    game->pause_button->active = 1;
    game->play_button->active = 0;
    
    timer_init(&game->timer);
    srand((int32_t)game->timer.start_time);
    game->initialized = 1;
    game->speed = 1.0f;
    game->player.score = 0;
    game->player.selection = kInvalid;

    
    for(ii=0;ii<kMaxNoteQueue;++ii) {
        game->attacking_weapons[ii].weapon = _get_computer_move();
        game->attacking_weapons[ii].timer = kBaseWeaponTimer*(ii+1);
    }
}
static void _game_quit(ui_param_t* p) {
    game_t* game = p[0].ptr;
    int ii;
    for(ii=0;ii<kNumWeapons;++ii) {
        game->weapon_buttons[ii]->active = 0;
    }
    game->pause_button->active = 0;
    game->quit_button->active = 0;
    game->resume_button->active = 0;
    game->pause_background->active = 0;
    
    game->play_button->active = 1;

    game->state = kMainMenu;
}

/* Paper and Scissors: http://www.Clker.com */
/* Rock: http://opengameart.org/content/rocks */
/* Font: http://www.fontsquirrel.com/fonts/TitilliumText */
/*----------------------------------------------------------------------------*\
External
\*----------------------------------------------------------------------------*/
void game_initialize(game_t* game, float width, float height) {
    int ii;
    float scale;
    /* GL setup */
    render_init();
    ui_init();
    render_resize(width, height);
    
    game->player.selection = kInvalid;
    for(ii=0;ii<kMaxNoteQueue;++ii) {
        game->attacking_weapons[ii].weapon = _get_computer_move();
        game->attacking_weapons[ii].timer = kBaseWeaponTimer*(ii+1);
    }
    game->state = kMainMenu;

    _button_texture = render_create_texture("assets/button.png");
    _logo_texture = render_create_texture("assets/logo.png");
    _weapon_textures[kRock] = render_create_texture("assets/rock.png");
    _weapon_textures[kPaper] = render_create_texture("assets/paper.png");
    _weapon_textures[kScissors] = render_create_texture("assets/scissors.png");

    scale = 40.0f*get_device_scale();
    game->pause_button = ui_create_button_texture(render_create_texture("assets/pause.png"),
                                      -width/2 + scale,
                                      height/2 - scale,
                                      scale,
                                      scale);
    game->pause_button->callback = (ui_callback_t*)game_toggle_pause;
    game->pause_button->params[0].ptr = game;
    game->pause_button->active = 0;
    
    for(ii=0;ii<kNumWeapons;++ii) {
        float button_size = width/kNumWeapons;
        button_t* button;
        button = ui_create_button_texture(_weapon_textures[ii],
                                          (ii*button_size)-width/2 + button_size/2,
                                          -height/2 + button_size/2,
                                          button_size,
                                          button_size);
        button->callback = _player_selection;
        button->params[0].ptr = game;
        button->params[1].i = (weapon_t)ii;
        button->active = 0;
        game->weapon_buttons[ii] = button;
    }

    game->pause_background = ui_create_button_texture(render_create_texture("assets/white.png"),
                                                      0,
                                                      0,
                                                      width,
                                                      height);
    game->pause_background->active = 0;
    game->pause_background->color = kBlack;
    game->pause_background->color.a = 0.5f;

    game->resume_button = ui_create_button_texture(_button_texture, 0, 0, ui_text_width("Resume")*1.2f, ui_text_size()-8*get_device_scale());
    game->resume_button->callback = (ui_callback_t*)game_resume;
    game->resume_button->params[0].ptr = game;
    game->resume_button->active = 0;
    
    game->quit_button = ui_create_button_texture(_button_texture, 0, -50*get_device_scale(), ui_text_width("Quit")*1.2f, ui_text_size()-8*get_device_scale());
    game->quit_button->callback = (ui_callback_t*)_game_quit;
    game->quit_button->params[0].ptr = game;
    game->quit_button->active = 0;
    
    game->play_button = ui_create_button_texture(_button_texture, 0, 0, ui_text_width("Play")*1.2f*1.5f, 1.5f*ui_text_size()-8*get_device_scale());
    game->play_button->callback = _game_start;
    game->play_button->params[0].ptr = game;
    game->play_button->active = 1;

    timer_init(&game->timer);
    srand((int32_t)game->timer.start_time);
    game->initialized = 1;
    game->speed = 1.0f;

    render_set_projection_matrix(kOrthographic);
}
void game_update(game_t* game) {
    int ii;
    game->delta_time = (float)timer_delta_time(&game->timer);
    if(game->state == kPause)
        return;
    if(game->state == kMainMenu)
    {
        return;
    }
    
    for(ii=0;ii<kMaxNoteQueue;++ii)
        game->attacking_weapons[ii].timer -= (game->delta_time*game->speed);
        
    if(game->attacking_weapons[0].timer <= 0.0f) {
        int result = _get_winner(game->player.selection, game->attacking_weapons[0].weapon);
        game->player.score += result;
        if(result > 0)
            game->speed *= 1.01f;
        else if(result < 0)
            game->speed *= 0.99f;
        game->speed = max(game->speed, 0.1f);

        for(ii=0;ii<kMaxNoteQueue-1;++ii)
            game->attacking_weapons[ii] = game->attacking_weapons[ii+1];
            
        game->attacking_weapons[kMaxNoteQueue-1].weapon = _get_computer_move();
        game->attacking_weapons[kMaxNoteQueue-1].timer = kBaseWeaponTimer*kMaxNoteQueue;

        game->player.selection = kInvalid;
        for(ii=0;ii<kNumWeapons;++ii)
            game->weapon_buttons[ii]->color = kWhite;
    }
}
void game_render(game_t* game) {
    float4x4 transform = float4x4translation(0.0f, -0.2f, -10.0f);
    int ii;
    render_prepare();

    if(game->state == kMainMenu)
    {
        render_draw_quad(_logo_texture, 0, 120*get_device_scale(), 200*get_device_scale(), 100*get_device_scale());

        ui_render();
        render_set_colorf(0.0f, 0.0f, 0.0f, 1.0f);
        ui_draw_text_formatted("Play", kJustifyCenter, -ui_text_size()/2+2*get_device_scale(), 1.0f);
        return;
    }

    _print_scores(game);
    
    render_set_colorf(1.0f, 1.0f, 1.0f, 1.0f);
    render_set_projection_matrix(kPerspective);
    
    for(ii=kMaxNoteQueue-1;ii>=0;--ii) {
        transform.r3.z = lerp( -30.0f,
                               -3.0f,
                                1-(game->attacking_weapons[ii].timer/kBaseWeaponTimer));
        transform.r3.y = lerp(  7.0f,
                               -0.2f,
                                1-(game->attacking_weapons[ii].timer/kBaseWeaponTimer));
        render_set_colorf(1.0f, 1.0f, 1.0f, lerp(1.0f, 0.0f, 1-(game->attacking_weapons[ii].timer/0.25f)) );
        render_draw_quad_transform(_weapon_textures[game->attacking_weapons[ii].weapon], &transform);
    }
    render_set_projection_matrix(kOrthographic);

    render_set_colorf(1.0f, 1.0f, 1.0f, 1.0f);
    ui_render();
    
    if(game->state == kPause) {
        render_set_colorf(1.0f, 1.0f, 1.0f, 1.0f);
        ui_draw_text_formatted("Paused", kJustifyCenter, 100.0f, 1.5f);
        render_set_colorf(0.0f, 0.0f, 0.0f, 1.0f);
        ui_draw_text_formatted("Resume", kJustifyCenter, -ui_text_size()/2+2*get_device_scale(), 1.0f);
        ui_draw_text_formatted("Quit", kJustifyCenter, -50*get_device_scale()-ui_text_size()/2+2*get_device_scale(), 1.0f);
    }
}
void game_shutdown(game_t* game) {
    game->initialized = 0;
}
void game_handle_tap(game_t* game, float x, float y) {
    ui_tap(x, y);
}
void game_toggle_pause(ui_param_t* p) {
    game_t* game = p[0].ptr;
    if(game->state == kPause)
        game_resume(p);
    else
        game_pause(p);
}
void game_pause(ui_param_t* p) {
    game_t* game = p[0].ptr;
    if(game->state == kMainMenu)
        return;
    game->state = kPause;
    game->resume_button->active = 1;
    game->quit_button->active = 1;
    game->pause_background->active = 1;
}
void game_resume(ui_param_t* p) {
    game_t* game = p[0].ptr;
    if(game->state == kMainMenu)
        return;
    game->state = kGame;
    timer_init(&game->timer);
    game->pause_background->active = 0;
    game->resume_button->active = 0;
    game->quit_button->active = 0;
}
