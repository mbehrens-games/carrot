/*******************************************************************************
** global.h (global variables)
*******************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H

enum
{
  BUNNY_ACTION_STANDING = 0,
  BUNNY_ACTION_ACCELERATING,
  BUNNY_ACTION_WALKING,
  BUNNY_ACTION_DECELERATING,
  BUNNY_ACTION_GRABBING,
  BUNNY_ACTION_THROWING,
  BUNNY_ACTION_KICKING,
  BUNNY_ACTION_CLIMBING,
  BUNNY_ACTION_HOPPING,
  BUNNY_ACTION_JUMPING,
  BUNNY_ACTION_FALLING,
  BUNNY_ACTION_JUMPING_DOUBLE,
  BUNNY_ACTION_FALLING_DOUBLE
};

enum
{
  BUNNY_FACING_RIGHT = 0,
  BUNNY_FACING_LEFT
};

enum
{
  BUNNY_GRAVITY_DOWN = 0,
  BUNNY_GRAVITY_UP
};

enum
{
  MARBLE_GRAVITY_DOWN = 0,
  MARBLE_GRAVITY_UP,
  MARBLE_GRAVITY_RIGHT,
  MARBLE_GRAVITY_LEFT
};

extern int            G_game_screen;
extern int            G_last_screen;

extern int            G_screen_alternate;
extern int            G_screen_page;
extern int            G_screen_choice;

extern int            G_flag_world_complete;
extern int            G_flag_redraw_grid_objects;
extern int            G_flag_window_minimized;
extern int            G_flag_window_fullscreen;
extern int            G_flag_window_vsync;
extern int            G_flag_quit_game;

extern unsigned int   G_timer_count;
extern unsigned int   G_saved_timer_count;

extern int            G_current_room;
extern int            G_current_world;

extern int            G_carrots_remaining;

extern int            G_bunny_action;
extern int            G_bunny_facing;
extern int            G_bunny_gravity;

extern int            G_marble_gravity;

/* function declarations */
short int globals_init_variables();

#endif
