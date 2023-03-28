/*******************************************************************************
** controls.c (keyboard / joystick input)
*******************************************************************************/

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

#include "bunny.h"
#include "controls.h"
#include "doremi.h"
#include "global.h"
#include "menu.h"
#include "screen.h"

enum
{
  CONTROLS_INDEX_DPAD_UP = 0,
  CONTROLS_INDEX_DPAD_DOWN,
  CONTROLS_INDEX_DPAD_LEFT,
  CONTROLS_INDEX_DPAD_RIGHT,
  CONTROLS_INDEX_BUTTON_B,
  CONTROLS_INDEX_BUTTON_A,
  CONTROLS_INDEX_BUTTON_START,
  CONTROLS_INDEX_KEY_ESCAPE,
  CONTROLS_NUM_INDICES
};

enum
{
  CONTROLS_INPUT_STATE_OFF = 0,
  CONTROLS_INPUT_STATE_PRESSED,
  CONTROLS_INPUT_STATE_ON,
  CONTROLS_INPUT_STATE_RELEASED
};

#define CONTROLS_INPUT_1ST_REPEAT_TIME  30
#define CONTROLS_INPUT_2ND_REPEAT_TIME  (CONTROLS_INPUT_1ST_REPEAT_TIME + 12)

#define CONTROLS_INPUT_IS_PRESSED(index)                                       \
  (S_input_states[index] == CONTROLS_INPUT_STATE_PRESSED)

#define CONTROLS_INPUT_IS_RELEASED(index)                                      \
  (S_input_states[index] == CONTROLS_INPUT_STATE_RELEASED)

#define CONTROLS_INPUT_IS_ON_OR_PRESSED(index)                                 \
  ( (S_input_states[index] == CONTROLS_INPUT_STATE_ON) ||                      \
    (S_input_states[index] == CONTROLS_INPUT_STATE_PRESSED))

#define CONTROLS_INPUT_IS_OFF_OR_RELEASED(index)                               \
  ( (S_input_states[index] == CONTROLS_INPUT_STATE_OFF) ||                     \
    (S_input_states[index] == CONTROLS_INPUT_STATE_RELEASED))

#define CONTROLS_CHECK_FOR_REPEAT(index)                                       \
  if (CONTROLS_INPUT_IS_ON_OR_PRESSED(index))                                  \
  {                                                                            \
    S_input_time_elapsed[index] += 1;                                          \
                                                                               \
    if ((S_input_time_elapsed[index] == CONTROLS_INPUT_1ST_REPEAT_TIME) ||     \
        (S_input_time_elapsed[index] == CONTROLS_INPUT_2ND_REPEAT_TIME))       \
    {                                                                          \
      S_input_states[index] = CONTROLS_INPUT_STATE_PRESSED;                    \
    }                                                                          \
                                                                               \
    if (S_input_time_elapsed[index] >= CONTROLS_INPUT_2ND_REPEAT_TIME)         \
      S_input_time_elapsed[index] = CONTROLS_INPUT_1ST_REPEAT_TIME;            \
  }                                                                            \
  else                                                                         \
    S_input_time_elapsed[index] = 0;

#define CONTROLS_ANALOG_STICK_DEADZONE  8192

#define CONTROLS_MAX_GAMEPADS 4

#define CONTROLS_VERIFY_GAMEPAD_INSTANCE_ID()                                  \
  if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_NONE)                        \
    return 0;                                                                  \
  else if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_READY)                  \
  {                                                                            \
    if ((instance_id == S_gamepad_ids[0]) && (S_num_gamepads >= 1))            \
      controls_set_active_gamepad(CONTROLS_ACTIVE_GAMEPAD_1);                  \
    else if ((instance_id == S_gamepad_ids[1]) && (S_num_gamepads >= 2))       \
      controls_set_active_gamepad(CONTROLS_ACTIVE_GAMEPAD_2);                  \
    else if ((instance_id == S_gamepad_ids[2]) && (S_num_gamepads >= 3))       \
      controls_set_active_gamepad(CONTROLS_ACTIVE_GAMEPAD_3);                  \
    else if ((instance_id == S_gamepad_ids[3]) && (S_num_gamepads == 4))       \
      controls_set_active_gamepad(CONTROLS_ACTIVE_GAMEPAD_4);                  \
    else                                                                       \
      return 0;                                                                \
  }                                                                            \
  else if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_1)                      \
  {                                                                            \
    if (instance_id != S_gamepad_ids[0])                                       \
      return 0;                                                                \
  }                                                                            \
  else if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_2)                      \
  {                                                                            \
    if (instance_id != S_gamepad_ids[1])                                       \
      return 0;                                                                \
  }                                                                            \
  else if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_3)                      \
  {                                                                            \
    if (instance_id != S_gamepad_ids[2])                                       \
      return 0;                                                                \
  }                                                                            \
  else if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_4)                      \
  {                                                                            \
    if (instance_id != S_gamepad_ids[3])                                       \
      return 0;                                                                \
  }                                                                            \
  else                                                                         \
    return 0;

static int S_input_states[CONTROLS_NUM_INDICES];
static int S_input_time_elapsed[CONTROLS_NUM_INDICES];

static SDL_JoystickID S_gamepad_ids[CONTROLS_MAX_GAMEPADS];

static int S_num_gamepads;

static Sint16 S_old_stick_x;
static Sint16 S_old_stick_y;

static SDL_GameControllerButton S_button_mapped_as_b;
static SDL_GameControllerButton S_button_mapped_as_a;

static SDL_Scancode S_key_mapped_as_b;
static SDL_Scancode S_key_mapped_as_a;

int G_active_gamepad;
int G_game_button_layout;
int G_menu_button_layout;
int G_menu_key_layout;

/*******************************************************************************
** controls_setup()
*******************************************************************************/
short int controls_setup()
{
  int i;

  for (i = 0; i < CONTROLS_NUM_INDICES; i++)
  {
    S_input_states[i] = CONTROLS_INPUT_STATE_OFF;
    S_input_time_elapsed[i] = 0;
  }

  for (i = 0; i < CONTROLS_MAX_GAMEPADS; i++)
    S_gamepad_ids[i] = -1;

  S_num_gamepads = 0;

  S_old_stick_x = 0;
  S_old_stick_y = 0;

  S_button_mapped_as_b = SDL_CONTROLLER_BUTTON_A;
  S_button_mapped_as_a = SDL_CONTROLLER_BUTTON_B;

  S_key_mapped_as_b = SDL_SCANCODE_Z;
  S_key_mapped_as_a = SDL_SCANCODE_X;

  G_active_gamepad = CONTROLS_ACTIVE_GAMEPAD_NONE;
  G_game_button_layout = CONTROLS_GAME_BUTTON_LAYOUT_BA;
  G_menu_button_layout = CONTROLS_MENU_BUTTON_LAYOUT_BA;
  G_menu_key_layout = CONTROLS_MENU_KEY_LAYOUT_ZX;

  return 0;
}

/*******************************************************************************
** controls_reset_button_states()
*******************************************************************************/
short int controls_reset_button_states()
{
  /* reset the button states when the button layout */
  /* changes. otherwise, we might miss a button     */
  /* being released after the layout changes.       */
  S_input_states[CONTROLS_INDEX_BUTTON_B] = CONTROLS_INPUT_STATE_OFF;
  S_input_time_elapsed[CONTROLS_INDEX_BUTTON_B] = 0;

  S_input_states[CONTROLS_INDEX_BUTTON_A] = CONTROLS_INPUT_STATE_OFF;
  S_input_time_elapsed[CONTROLS_INDEX_BUTTON_A] = 0;

  return 0;
}

/*******************************************************************************
** controls_reset_all_states()
*******************************************************************************/
short int controls_reset_all_states()
{
  int i;

  /* reset all states when a gamepad is */
  /* remapped, plugged in, or unplugged */
  for (i = 0; i < CONTROLS_NUM_INDICES; i++)
  {
    S_input_states[i] = CONTROLS_INPUT_STATE_OFF;
    S_input_time_elapsed[i] = 0;
  }

  return 0;
}

/*******************************************************************************
** controls_map_gamepad()
*******************************************************************************/
short int controls_map_gamepad()
{
  /* note: for the buttons, the internal representation is in the style */
  /* of the NES, SNES, etc (i.e., the buttons read BA and YX from left  */
  /* to right). However, SDL2 has the GameController buttons the other  */
  /* way around (i.e., they read AB and XY from left to right).         */

  /* in-game screens */
  if ((G_game_screen == GAME_SCREEN_PLAYING)          || 
      (G_game_screen == GAME_SCREEN_ROOM_INTRO_PANEL) || 
      (G_game_screen == GAME_SCREEN_ROOM_COMPLETE_PANEL))
  {
    if (G_game_button_layout == CONTROLS_GAME_BUTTON_LAYOUT_BA)
    {
      S_button_mapped_as_b = SDL_CONTROLLER_BUTTON_A;
      S_button_mapped_as_a = SDL_CONTROLLER_BUTTON_B;
    }
    else if (G_game_button_layout == CONTROLS_GAME_BUTTON_LAYOUT_YB)
    {
      S_button_mapped_as_b = SDL_CONTROLLER_BUTTON_X;
      S_button_mapped_as_a = SDL_CONTROLLER_BUTTON_A;
    }
    else
    {
      S_button_mapped_as_b = SDL_CONTROLLER_BUTTON_A;
      S_button_mapped_as_a = SDL_CONTROLLER_BUTTON_B;
    }
  }
  /* menu screens */
  else
  {
    if (G_menu_button_layout == CONTROLS_MENU_BUTTON_LAYOUT_BA)
    {
      S_button_mapped_as_b = SDL_CONTROLLER_BUTTON_A;
      S_button_mapped_as_a = SDL_CONTROLLER_BUTTON_B;
    }
    else if (G_menu_button_layout == CONTROLS_MENU_BUTTON_LAYOUT_AB)
    {
      S_button_mapped_as_b = SDL_CONTROLLER_BUTTON_B;
      S_button_mapped_as_a = SDL_CONTROLLER_BUTTON_A;
    }
    else
    {
      S_button_mapped_as_b = SDL_CONTROLLER_BUTTON_A;
      S_button_mapped_as_a = SDL_CONTROLLER_BUTTON_B;
    }
  }

  /* reset the button states after remapping */
  controls_reset_button_states();

  return 0;
}

/*******************************************************************************
** controls_map_keyboard()
*******************************************************************************/
short int controls_map_keyboard()
{
  /* in-game screens */
  if ((G_game_screen == GAME_SCREEN_PLAYING)          || 
      (G_game_screen == GAME_SCREEN_ROOM_INTRO_PANEL) || 
      (G_game_screen == GAME_SCREEN_ROOM_COMPLETE_PANEL))
  {
    S_key_mapped_as_b = SDL_SCANCODE_Z;
    S_key_mapped_as_a = SDL_SCANCODE_X;
  }
  /* menu screens */
  else
  {
    if (G_menu_key_layout == CONTROLS_MENU_KEY_LAYOUT_ZX)
    {
      S_key_mapped_as_b = SDL_SCANCODE_Z;
      S_key_mapped_as_a = SDL_SCANCODE_X;
    }
    else if (G_menu_key_layout == CONTROLS_MENU_KEY_LAYOUT_XZ)
    {
      S_key_mapped_as_b = SDL_SCANCODE_X;
      S_key_mapped_as_a = SDL_SCANCODE_Z;
    }
    else
    {
      S_key_mapped_as_b = SDL_SCANCODE_Z;
      S_key_mapped_as_a = SDL_SCANCODE_X;
    }
  }

  /* reset the button states after remapping */
  controls_reset_button_states();

  return 0;
}

/*******************************************************************************
** controls_plug_in_all_gamepads()
*******************************************************************************/
short int controls_plug_in_all_gamepads()
{
  int i;

  SDL_GameController* gamepad_pointer;

  /* obtain first 4 gamepads */
  S_num_gamepads = 0;

  for (i = 0; i < SDL_NumJoysticks(); i++)
  {
    if (S_num_gamepads == CONTROLS_MAX_GAMEPADS)
      break;

    if (SDL_IsGameController(i))
    {
      gamepad_pointer = SDL_GameControllerOpen(i);

      if (gamepad_pointer != NULL)
      {
        S_gamepad_ids[S_num_gamepads] = 
          SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gamepad_pointer));

        S_num_gamepads += 1;
      }
    }
  }

  /* if any gamepads are found, set to the ready state        */
  /* (the 1st gamepad pressed will become the active gamepad) */
  if (S_num_gamepads == 0)
    controls_set_active_gamepad(CONTROLS_ACTIVE_GAMEPAD_NONE);
  else
    controls_set_active_gamepad(CONTROLS_ACTIVE_GAMEPAD_READY);

  return 0;
}

/*******************************************************************************
** controls_unplug_all_gamepads()
*******************************************************************************/
short int controls_unplug_all_gamepads()
{
  int i;

  SDL_GameController* gamepad_pointer;

  /* close all gamepads */
  for (i = 0; i < S_num_gamepads; i++)
  {
    gamepad_pointer = SDL_GameControllerFromInstanceID(S_gamepad_ids[i]);

    if (gamepad_pointer != NULL)
      SDL_GameControllerClose(gamepad_pointer);

    S_gamepad_ids[i] = -1;
  }

  /* reset the active gamepad */
  S_num_gamepads = 0;
  controls_set_active_gamepad(CONTROLS_ACTIVE_GAMEPAD_NONE);

  return 0;
}

/*******************************************************************************
** controls_set_active_gamepad()
*******************************************************************************/
short int controls_set_active_gamepad(int gamepad)
{
  if (((gamepad == CONTROLS_ACTIVE_GAMEPAD_NONE)  && (S_num_gamepads == 0)) || 
      ((gamepad == CONTROLS_ACTIVE_GAMEPAD_READY) && (S_num_gamepads >= 1)) || 
      ((gamepad == CONTROLS_ACTIVE_GAMEPAD_1)     && (S_num_gamepads >= 1)) || 
      ((gamepad == CONTROLS_ACTIVE_GAMEPAD_2)     && (S_num_gamepads >= 2)) || 
      ((gamepad == CONTROLS_ACTIVE_GAMEPAD_3)     && (S_num_gamepads >= 3)) || 
      ((gamepad == CONTROLS_ACTIVE_GAMEPAD_4)     && (S_num_gamepads == 4)))
  {
    G_active_gamepad = gamepad;
  }
  else if (S_num_gamepads == 0)
    G_active_gamepad = CONTROLS_ACTIVE_GAMEPAD_NONE;
  else
    G_active_gamepad = CONTROLS_ACTIVE_GAMEPAD_READY;

  controls_reset_all_states();

  return 0;
}

/*******************************************************************************
** controls_next_gamepad()
*******************************************************************************/
short int controls_next_gamepad()
{
  if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_NONE)
    return 0;

  if (((G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_1) && (S_num_gamepads > 1)) || 
      ((G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_2) && (S_num_gamepads > 2)) || 
      ((G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_3) && (S_num_gamepads > 3)))
  {
    G_active_gamepad += 1;
    controls_reset_all_states();
  }
  else
    return 0;

  doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);

  return 0;
}

/*******************************************************************************
** controls_last_gamepad()
*******************************************************************************/
short int controls_last_gamepad()
{
  if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_NONE)
    return 0;

  if ((G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_2) || 
      (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_3) || 
      (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_4))
  {
    G_active_gamepad -= 1;
    controls_reset_all_states();
  }
  else
    return 0;

  doremi_play_sfx(SFX_INDEX_TOGGLE_UP);

  return 0;
}

/*******************************************************************************
** controls_set_game_button_layout()
*******************************************************************************/
short int controls_set_game_button_layout(int layout)
{
  if ((layout < 0) || (layout >= CONTROLS_NUM_GAME_BUTTON_LAYOUTS))
    return 0;

  G_game_button_layout = layout;
  controls_map_gamepad();

  return 0;
}

/*******************************************************************************
** controls_game_button_layout_right()
*******************************************************************************/
short int controls_game_button_layout_right()
{
  if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_NONE)
    return 0;

  if (G_game_button_layout + 1 < CONTROLS_NUM_GAME_BUTTON_LAYOUTS)
  {
    G_game_button_layout += 1;
    controls_map_gamepad();
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** controls_game_button_layout_left()
*******************************************************************************/
short int controls_game_button_layout_left()
{
  if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_NONE)
    return 0;

  if (G_game_button_layout - 1 >= 0)
  {
    G_game_button_layout -= 1;
    controls_map_gamepad();
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** controls_set_menu_button_layout()
*******************************************************************************/
short int controls_set_menu_button_layout(int layout)
{
  if ((layout < 0) || (layout >= CONTROLS_NUM_MENU_BUTTON_LAYOUTS))
    return 0;

  G_menu_button_layout = layout;
  controls_map_gamepad();

  return 0;
}

/*******************************************************************************
** controls_menu_button_layout_right()
*******************************************************************************/
short int controls_menu_button_layout_right()
{
  if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_NONE)
    return 0;

  if (G_menu_button_layout + 1 < CONTROLS_NUM_MENU_BUTTON_LAYOUTS)
  {
    G_menu_button_layout += 1;
    controls_map_gamepad();
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** controls_menu_button_layout_left()
*******************************************************************************/
short int controls_menu_button_layout_left()
{
  if (G_active_gamepad == CONTROLS_ACTIVE_GAMEPAD_NONE)
    return 0;

  if (G_menu_button_layout - 1 >= 0)
  {
    G_menu_button_layout -= 1;
    controls_map_gamepad();
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** controls_set_menu_key_layout()
*******************************************************************************/
short int controls_set_menu_key_layout(int layout)
{
  if ((layout < 0) || (layout >= CONTROLS_NUM_MENU_KEY_LAYOUTS))
    return 0;

  G_menu_key_layout = layout;
  controls_map_keyboard();

  return 0;
}

/*******************************************************************************
** controls_menu_key_layout_right()
*******************************************************************************/
short int controls_menu_key_layout_right()
{
  if (G_menu_key_layout + 1 < CONTROLS_NUM_MENU_KEY_LAYOUTS)
  {
    G_menu_key_layout += 1;
    controls_map_keyboard();
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** controls_menu_key_layout_left()
*******************************************************************************/
short int controls_menu_key_layout_left()
{
  if (G_menu_key_layout - 1 >= 0)
  {
    G_menu_key_layout -= 1;
    controls_map_keyboard();
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** controls_gamepad_axis_moved()
*******************************************************************************/
short int controls_gamepad_axis_moved(SDL_JoystickID instance_id, 
                                      SDL_GameControllerAxis axis, 
                                      Sint16 value)
{
  CONTROLS_VERIFY_GAMEPAD_INSTANCE_ID()

  /* check the left analog stick for movement, in case  */
  /* someone wants to use that instead of the d-pad.    */
  /* as a wise man once asked: who are these people?    */

  /* left analog stick up/down */
  if (axis == SDL_CONTROLLER_AXIS_LEFTY)
  {
    /* up pressed */
    if ((value <= -CONTROLS_ANALOG_STICK_DEADZONE) && 
        (S_old_stick_y > -CONTROLS_ANALOG_STICK_DEADZONE))
    {
      if (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_UP))
        S_input_states[CONTROLS_INDEX_DPAD_UP] = CONTROLS_INPUT_STATE_PRESSED;
    }
    /* up released */
    else if ( (value > -CONTROLS_ANALOG_STICK_DEADZONE) && 
              (S_old_stick_y <= -CONTROLS_ANALOG_STICK_DEADZONE))
    {
      if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_UP))
        S_input_states[CONTROLS_INDEX_DPAD_UP] = CONTROLS_INPUT_STATE_RELEASED;
    }
    /* down released */
    else if ( (value < CONTROLS_ANALOG_STICK_DEADZONE) && 
              (S_old_stick_y >= CONTROLS_ANALOG_STICK_DEADZONE))
    {
      if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_DOWN))
        S_input_states[CONTROLS_INDEX_DPAD_DOWN] = CONTROLS_INPUT_STATE_RELEASED;
    }
    /* down pressed */
    else if ( (value >= CONTROLS_ANALOG_STICK_DEADZONE) && 
              (S_old_stick_y < CONTROLS_ANALOG_STICK_DEADZONE))
    {
      if (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_DOWN))
        S_input_states[CONTROLS_INDEX_DPAD_DOWN] = CONTROLS_INPUT_STATE_PRESSED;
    }

    S_old_stick_y = value;
  }

  /* left analog stick left/right */
  if (axis == SDL_CONTROLLER_AXIS_LEFTX)
  {
    /* left pressed */
    if ((value <= -CONTROLS_ANALOG_STICK_DEADZONE) && 
        (S_old_stick_x > -CONTROLS_ANALOG_STICK_DEADZONE))
    {
      if (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_LEFT))
        S_input_states[CONTROLS_INDEX_DPAD_LEFT] = CONTROLS_INPUT_STATE_PRESSED;
    }
    /* left released */
    else if ( (value > -CONTROLS_ANALOG_STICK_DEADZONE) && 
              (S_old_stick_x <= -CONTROLS_ANALOG_STICK_DEADZONE))
    {
      if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_LEFT))
        S_input_states[CONTROLS_INDEX_DPAD_LEFT] = CONTROLS_INPUT_STATE_RELEASED;
    }
    /* right released */
    else if ( (value < CONTROLS_ANALOG_STICK_DEADZONE) && 
              (S_old_stick_x >= CONTROLS_ANALOG_STICK_DEADZONE))
    {
      if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_RIGHT))
        S_input_states[CONTROLS_INDEX_DPAD_RIGHT] = CONTROLS_INPUT_STATE_RELEASED;
    }
    /* right pressed */
    else if ( (value >= CONTROLS_ANALOG_STICK_DEADZONE) && 
              (S_old_stick_x < CONTROLS_ANALOG_STICK_DEADZONE))
    {
      if (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_RIGHT))
        S_input_states[CONTROLS_INDEX_DPAD_RIGHT] = CONTROLS_INPUT_STATE_PRESSED;
    }

    S_old_stick_x = value;
  }

  return 0;
}

/*******************************************************************************
** controls_gamepad_button_pressed()
*******************************************************************************/
short int controls_gamepad_button_pressed(SDL_JoystickID instance_id, 
                                          SDL_GameControllerButton button)
{
  CONTROLS_VERIFY_GAMEPAD_INSTANCE_ID()

  /* dpad up */
  if ((button == SDL_CONTROLLER_BUTTON_DPAD_UP) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_UP)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_UP] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* dpad down */
  if ((button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_DOWN)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_DOWN] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* dpad left */
  if ((button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_LEFT)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_LEFT] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* dpad right */
  if ((button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_RIGHT)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_RIGHT] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* button b */
  if ((button == S_button_mapped_as_b) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_BUTTON_B)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_B] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* button a */
  if ((button == S_button_mapped_as_a) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_BUTTON_A)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_A] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* button start */
  if ((button == SDL_CONTROLLER_BUTTON_START) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_BUTTON_START)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_START] = CONTROLS_INPUT_STATE_PRESSED;
  }

  return 0;
}

/*******************************************************************************
** controls_gamepad_button_released()
*******************************************************************************/
short int controls_gamepad_button_released( SDL_JoystickID instance_id, 
                                            SDL_GameControllerButton button)
{
  CONTROLS_VERIFY_GAMEPAD_INSTANCE_ID()

  /* dpad up */
  if ((button == SDL_CONTROLLER_BUTTON_DPAD_UP) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_UP)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_UP] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* dpad down */
  if ((button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_DOWN)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_DOWN] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* dpad left */
  if ((button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_LEFT)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_LEFT] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* dpad right */
  if ((button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_RIGHT)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_RIGHT] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* button b */
  if ((button == S_button_mapped_as_b) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_BUTTON_B)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_B] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* button a */
  if ((button == S_button_mapped_as_a) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_BUTTON_A)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_A] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* button start */
  if ((button == SDL_CONTROLLER_BUTTON_START) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_BUTTON_START)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_START] = CONTROLS_INPUT_STATE_RELEASED;
  }

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_pressed()
*******************************************************************************/
short int controls_keyboard_key_pressed(SDL_Scancode code)
{
  /* dpad up */
  if ((code == SDL_SCANCODE_UP) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_UP)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_UP] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* dpad down */
  if ((code == SDL_SCANCODE_DOWN) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_DOWN)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_DOWN] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* dpad left */
  if ((code == SDL_SCANCODE_LEFT) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_LEFT)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_LEFT] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* dpad right */
  if ((code == SDL_SCANCODE_RIGHT) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_DPAD_RIGHT)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_RIGHT] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* button b */
  if ((code == S_key_mapped_as_b) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_BUTTON_B)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_B] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* button a */
  if ((code == S_key_mapped_as_a) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_BUTTON_A)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_A] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* button start */
  if ((code == SDL_SCANCODE_RETURN) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_BUTTON_START)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_START] = CONTROLS_INPUT_STATE_PRESSED;
  }

  /* escape key */
  if ((code == SDL_SCANCODE_ESCAPE) && 
      (CONTROLS_INPUT_IS_OFF_OR_RELEASED(CONTROLS_INDEX_KEY_ESCAPE)))
  {
    S_input_states[CONTROLS_INDEX_KEY_ESCAPE] = CONTROLS_INPUT_STATE_PRESSED;
  }

  return 0;
}

/*******************************************************************************
** controls_keyboard_key_released()
*******************************************************************************/
short int controls_keyboard_key_released(SDL_Scancode code)
{
  /* dpad up */
  if ((code == SDL_SCANCODE_UP) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_UP)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_UP] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* dpad down */
  if ((code == SDL_SCANCODE_DOWN) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_DOWN)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_DOWN] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* dpad left */
  if ((code == SDL_SCANCODE_LEFT) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_LEFT)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_LEFT] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* dpad right */
  if ((code == SDL_SCANCODE_RIGHT) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_RIGHT)))
  {
    S_input_states[CONTROLS_INDEX_DPAD_RIGHT] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* button b */
  if ((code == S_key_mapped_as_b) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_BUTTON_B)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_B] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* button a */
  if ((code == S_key_mapped_as_a) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_BUTTON_A)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_A] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* button start */
  if ((code == SDL_SCANCODE_RETURN) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_BUTTON_START)))
  {
    S_input_states[CONTROLS_INDEX_BUTTON_START] = CONTROLS_INPUT_STATE_RELEASED;
  }

  /* escape key */
  if ((code == SDL_SCANCODE_ESCAPE) && 
      (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_KEY_ESCAPE)))
  {
    S_input_states[CONTROLS_INDEX_KEY_ESCAPE] = CONTROLS_INPUT_STATE_RELEASED;
  }

  return 0;
}

/*******************************************************************************
** controls_remove_edges()
*******************************************************************************/
short int controls_remove_edges()
{
  int i;

  for (i = 0; i < CONTROLS_NUM_INDICES; i++)
  {
    if (S_input_states[i] == CONTROLS_INPUT_STATE_PRESSED)
      S_input_states[i] = CONTROLS_INPUT_STATE_ON;

    if (S_input_states[i] == CONTROLS_INPUT_STATE_RELEASED)
      S_input_states[i] = CONTROLS_INPUT_STATE_OFF;
  }

  return 0;
}

/*******************************************************************************
** controls_process_player_input_gameplay()
*******************************************************************************/
short int controls_process_player_input_gameplay()
{
  /* d pad left */
  if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_LEFT))
    bunny_player_input_d_pad_left_on();
  else
    bunny_player_input_d_pad_left_off();

  /* d pad right */
  if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_RIGHT))
    bunny_player_input_d_pad_right_on();
  else
    bunny_player_input_d_pad_right_off();

  /* d pad up */
  if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_UP))
    bunny_player_input_d_pad_up_on();
  else
    bunny_player_input_d_pad_up_off();

  /* d pad down */
  if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_DOWN))
    bunny_player_input_d_pad_down_on();
  else
    bunny_player_input_d_pad_down_off();

  /* button b (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_BUTTON_B))
  {
    if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_UP))
      bunny_player_input_d_pad_up_on_and_button_b_pressed();
    else if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_DOWN))
      bunny_player_input_d_pad_down_on_and_button_b_pressed();
    else if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_LEFT))
      bunny_player_input_d_pad_left_on_and_button_b_pressed();
    else if (CONTROLS_INPUT_IS_ON_OR_PRESSED(CONTROLS_INDEX_DPAD_RIGHT))
      bunny_player_input_d_pad_right_on_and_button_b_pressed();
    else
      bunny_player_input_d_pad_neutral_and_button_b_pressed();

    bunny_player_input_always_when_button_b_pressed();
  }

  /* button a (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_BUTTON_A))
    bunny_player_input_button_a_pressed();

  /* button a (falling edge) */
  if (CONTROLS_INPUT_IS_RELEASED(CONTROLS_INDEX_BUTTON_A))
    bunny_player_input_button_a_released();

  /* start button (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_BUTTON_START))
    menu_player_input_button_start_pressed();

  /* escape key (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_KEY_ESCAPE))
    menu_player_input_button_start_pressed();

  /* remove edges */
  controls_remove_edges();

  return 0;
}

/*******************************************************************************
** controls_process_player_input_menus()
*******************************************************************************/
short int controls_process_player_input_menus()
{
  /* retrigger d pad press if repeat time has elapsed */
  CONTROLS_CHECK_FOR_REPEAT(CONTROLS_INDEX_DPAD_UP)
  CONTROLS_CHECK_FOR_REPEAT(CONTROLS_INDEX_DPAD_DOWN)
  CONTROLS_CHECK_FOR_REPEAT(CONTROLS_INDEX_DPAD_LEFT)
  CONTROLS_CHECK_FOR_REPEAT(CONTROLS_INDEX_DPAD_RIGHT)

  /* d pad up (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_DPAD_UP))
    menu_player_input_d_pad_up_pressed();

  /* d pad down (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_DPAD_DOWN))
    menu_player_input_d_pad_down_pressed();

  /* d pad left (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_DPAD_LEFT))
    menu_player_input_d_pad_left_pressed();

  /* d pad right (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_DPAD_RIGHT))
    menu_player_input_d_pad_right_pressed();

  /* button b (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_BUTTON_B))
    menu_player_input_button_b_pressed();

  /* button a (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_BUTTON_A))
    menu_player_input_button_a_pressed();

  /* start button (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_BUTTON_START))
    menu_player_input_button_b_pressed();

  /* escape key (rising edge) */
  if (CONTROLS_INPUT_IS_PRESSED(CONTROLS_INDEX_KEY_ESCAPE))
    menu_player_input_button_a_pressed();

  /* remove edges */
  controls_remove_edges();

  return 0;
}

