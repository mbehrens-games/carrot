/*******************************************************************************
** controls.h (keyboard / joystick input)
*******************************************************************************/

#ifndef CONTROLS_H
#define CONTROLS_H

#include <SDL2/SDL.h>

enum
{
  CONTROLS_ACTIVE_GAMEPAD_NONE = 0,
  CONTROLS_ACTIVE_GAMEPAD_READY,
  CONTROLS_ACTIVE_GAMEPAD_1,
  CONTROLS_ACTIVE_GAMEPAD_2,
  CONTROLS_ACTIVE_GAMEPAD_3,
  CONTROLS_ACTIVE_GAMEPAD_4
};

enum
{
  CONTROLS_GAME_BUTTON_LAYOUT_BA = 0,
  CONTROLS_GAME_BUTTON_LAYOUT_YB,
  CONTROLS_NUM_GAME_BUTTON_LAYOUTS
};

enum
{
  CONTROLS_MENU_BUTTON_LAYOUT_BA = 0,
  CONTROLS_MENU_BUTTON_LAYOUT_AB,
  CONTROLS_NUM_MENU_BUTTON_LAYOUTS
};

enum
{
  CONTROLS_MENU_KEY_LAYOUT_ZX = 0,
  CONTROLS_MENU_KEY_LAYOUT_XZ,
  CONTROLS_NUM_MENU_KEY_LAYOUTS
};

extern int G_active_gamepad;
extern int G_game_button_layout;
extern int G_menu_button_layout;
extern int G_menu_key_layout;

/* function declarations */
short int controls_setup();

short int controls_reset_button_states();
short int controls_reset_all_states();

short int controls_map_gamepad();
short int controls_map_keyboard();

short int controls_plug_in_all_gamepads();
short int controls_unplug_all_gamepads();

short int controls_set_active_gamepad(int gamepad);
short int controls_next_gamepad();
short int controls_last_gamepad();

short int controls_set_game_button_layout(int layout);
short int controls_game_button_layout_right();
short int controls_game_button_layout_left();

short int controls_set_menu_button_layout(int layout);
short int controls_menu_button_layout_right();
short int controls_menu_button_layout_left();

short int controls_set_menu_key_layout(int layout);
short int controls_menu_key_layout_right();
short int controls_menu_key_layout_left();

short int controls_gamepad_axis_moved(SDL_JoystickID instance_id, 
                                      SDL_GameControllerAxis axis, 
                                      Sint16 value);

short int controls_gamepad_button_pressed(SDL_JoystickID instance_id, 
                                          SDL_GameControllerButton button);
short int controls_gamepad_button_released( SDL_JoystickID instance_id, 
                                            SDL_GameControllerButton button);

short int controls_keyboard_key_pressed(SDL_Scancode code);
short int controls_keyboard_key_released(SDL_Scancode code);

short int controls_process_player_input_gameplay();
short int controls_process_player_input_menus();

#endif
