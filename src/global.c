/*******************************************************************************
** global.c (global variables)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "screen.h"

int           G_game_screen;
int           G_last_screen;

int           G_screen_alternate;
int           G_screen_page;
int           G_screen_choice;

int           G_flag_world_complete;
int           G_flag_window_minimized;
int           G_flag_window_fullscreen;
int           G_flag_window_vsync;
int           G_flag_quit_game;

unsigned int  G_timer_count;
unsigned int  G_saved_timer_count;

int           G_current_room;
int           G_current_world;

int           G_carrots_remaining;

int           G_bunny_action;
int           G_bunny_facing;
int           G_bunny_gravity;

int           G_marble_gravity;

/*******************************************************************************
** globals_init_variables()
*******************************************************************************/
short int globals_init_variables()
{
  G_game_screen = GAME_SCREEN_TITLE;
  G_last_screen = GAME_SCREEN_TITLE;

  G_screen_alternate = 0;
  G_screen_page = 0;
  G_screen_choice = 0;

  G_flag_world_complete = 0;
  G_flag_window_minimized = 0;
  G_flag_window_fullscreen = 0;
  G_flag_window_vsync = 0;
  G_flag_quit_game = 0;

  G_timer_count = 0;
  G_saved_timer_count = 0;

  G_current_room = 0;
  G_current_world = 0;

  G_carrots_remaining = 0;

  G_bunny_action = BUNNY_ACTION_STANDING;
  G_bunny_facing = BUNNY_FACING_RIGHT;
  G_bunny_gravity = BUNNY_GRAVITY_DOWN;

  G_marble_gravity = MARBLE_GRAVITY_DOWN;

  return 0;
}

