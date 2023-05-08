/*******************************************************************************
** menu.c (menu functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "doremi.h"
#include "fade.h"
#include "gameloop.h"
#include "global.h"
#include "graphics.h"
#include "level.h"
#include "menu.h"
#include "options.h"
#include "savegame.h"
#include "screen.h"
#include "vbmenu.h"
#include "vbsprite.h"
#include "video.h"

/*******************************************************************************
** menu_player_input_d_pad_up_pressed()
*******************************************************************************/
short int menu_player_input_d_pad_up_pressed()
{
  if (G_game_screen == GAME_SCREEN_TITLE)
  {
    if (G_screen_choice > 0)
    {
      G_screen_choice -= 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }
  else if (G_game_screen == GAME_SCREEN_SAVE_GAME_SELECT)
  {
    if (G_screen_choice > 0)
    {
      if (G_save_game_percent != 0)
      {
        G_screen_choice -= 1;
        doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
      }
    }
  }
  else if (G_game_screen == GAME_SCREEN_CONFIRM_CLEAR_SAVE_GAME)
  {
    if (G_screen_choice > 0)
    {
      G_screen_choice -= 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }
  else if (G_game_screen == GAME_SCREEN_WORLD_SELECT)
  {
    if (G_screen_choice > 0)
    {
      G_screen_choice -= 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }
  else if (G_game_screen == GAME_SCREEN_ROOM_SELECT)
  {
    if (G_screen_choice > 0)
    {
      G_screen_choice -= 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }
  else if (G_game_screen == GAME_SCREEN_STORY_SELECT)
  {
    if (G_screen_choice > 0)
    {
      G_screen_choice -= 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }
  else if (G_game_screen == GAME_SCREEN_OPTIONS)
  {
    if (G_screen_choice > 0)
    {
      G_screen_choice -= 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }
  else if (G_game_screen == GAME_SCREEN_PAUSE_PANEL)
  {
    if (G_screen_choice > 0)
    {
      G_screen_choice -= 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }
  else if (G_game_screen == GAME_SCREEN_RETRY_PANEL)
  {
    if (G_screen_choice > 0)
    {
      G_screen_choice -= 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }

  return 0;
}

/*******************************************************************************
** menu_player_input_d_pad_down_pressed()
*******************************************************************************/
short int menu_player_input_d_pad_down_pressed()
{
  if (G_game_screen == GAME_SCREEN_TITLE)
  {
    if (G_screen_choice < SCREEN_TITLE_NUM_CHOICES - 1)
    {
      G_screen_choice += 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
  }
  else if (G_game_screen == GAME_SCREEN_SAVE_GAME_SELECT)
  {
    if (G_screen_choice < SCREEN_SAVE_GAME_SELECT_NUM_CHOICES - 1)
    {
      if (G_save_game_percent != 0)
      {
        G_screen_choice += 1;
        doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
      }
    }
  }
  else if (G_game_screen == GAME_SCREEN_CONFIRM_CLEAR_SAVE_GAME)
  {
    if (G_screen_choice < SCREEN_CONFIRM_CLEAR_SAVE_GAME_NUM_CHOICES - 1)
    {
      G_screen_choice += 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
  }
  else if (G_game_screen == GAME_SCREEN_WORLD_SELECT)
  {
    if (G_screen_choice < SCREEN_WORLD_SELECT_NUM_CHOICES - 1)
    {
      G_screen_choice += 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
  }
  else if (G_game_screen == GAME_SCREEN_ROOM_SELECT)
  {
    if (G_screen_choice < SCREEN_ROOM_SELECT_NUM_CHOICES - 1)
    {
      if (G_screen_choice < G_save_game_data[G_screen_alternate])
      {
        G_screen_choice += 1;
        doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
      }
    }
  }
  else if (G_game_screen == GAME_SCREEN_STORY_SELECT)
  {
    if (G_screen_choice < SCREEN_STORY_SELECT_NUM_CHOICES - 1)
    {
      if (((G_screen_choice == 0) && (G_save_game_percent >= 330)) || 
          ((G_screen_choice == 1) && (G_save_game_percent >= 660)) || 
          ((G_screen_choice == 2) && (G_save_game_percent == 1000)))
      {
        G_screen_choice += 1;
        doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
      }
    }
  }
  else if (G_game_screen == GAME_SCREEN_OPTIONS)
  {
    if ((G_screen_alternate == SCREEN_OPTIONS_ALTERNATE_STANDARD) && 
        (G_screen_choice < SCREEN_STANDARD_OPTIONS_NUM_CHOICES - 1))
    {
      G_screen_choice += 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
    else if ( (G_screen_alternate == SCREEN_OPTIONS_ALTERNATE_MORE) && 
              (G_screen_choice < SCREEN_MORE_OPTIONS_NUM_CHOICES - 1))
    {
      G_screen_choice += 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
  }
  else if (G_game_screen == GAME_SCREEN_PAUSE_PANEL)
  {
    if (G_screen_choice < SCREEN_PAUSE_PANEL_NUM_CHOICES - 1)
    {
      G_screen_choice += 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
  }
  else if (G_game_screen == GAME_SCREEN_RETRY_PANEL)
  {
    if (G_screen_choice < SCREEN_RETRY_PANEL_NUM_CHOICES - 1)
    {
      G_screen_choice += 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
  }

  return 0;
}

/*******************************************************************************
** menu_player_input_d_pad_left_pressed()
*******************************************************************************/
short int menu_player_input_d_pad_left_pressed()
{
  if (G_game_screen == GAME_SCREEN_SAVE_GAME_SELECT)
  {
    if (G_screen_page > 0)
    {
      game_loop_change_screen(GAME_SCREEN_SAVE_GAME_SELECT, 0, G_screen_page - 1, 0);
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }
  else if (G_game_screen == GAME_SCREEN_WORLD_SELECT)
  {
    if (G_screen_page > 0)
    {
      game_loop_change_screen(GAME_SCREEN_WORLD_SELECT, 0, G_screen_page - 1, 0);
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }
  else if (G_game_screen == GAME_SCREEN_OPTIONS)
  {
    /* note that the doremi_play_sfx calls occur within the functions called */
    /* standard options */
    if (G_screen_alternate == SCREEN_OPTIONS_ALTERNATE_STANDARD)
    {
      if (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_WINDOW_SIZE)
      {
        graphics_decrease_window_size();
      }
      else if ( (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_FULLSCREEN) && 
                (G_flag_window_fullscreen == 1))
      {
        graphics_toggle_fullscreen();
      }
      else if ( (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_VSYNC) && 
                (G_flag_window_vsync == 1))
      {
        graphics_toggle_vsync();
      }
      else if (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_BRIGHTNESS)
      {
        video_decrease_brightness();
      }
      else if (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_UPSCALING_MODE)
      {
        video_cycle_upscaling_mode_left();
      }
    }
    /* more options */
    else if (G_screen_alternate == SCREEN_OPTIONS_ALTERNATE_MORE)
    {
      if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_SOUND_VOLUME)
      {
        doremi_decrease_volume();
      }
      else if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_GAMEPAD_NUMBER)
      {
        controls_last_gamepad();
      }
      else if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_GAME_BUTTONS)
      {
        controls_game_button_layout_left();
      }
      else if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_MENU_BUTTONS)
      {
        controls_menu_button_layout_left();
      }
      else if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_MENU_KEYS)
      {
        controls_menu_key_layout_left();
      }
    }
  }
  else if (G_game_screen == GAME_SCREEN_HELP)
  {
    if (G_screen_page > 0)
    {
      game_loop_change_screen(GAME_SCREEN_HELP, 0, G_screen_page - 1, 0);
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }

  return 0;
}

/*******************************************************************************
** menu_player_input_d_pad_right_pressed()
*******************************************************************************/
short int menu_player_input_d_pad_right_pressed()
{
  if (G_game_screen == GAME_SCREEN_SAVE_GAME_SELECT)
  {
    if (G_screen_page < SCREEN_SAVE_GAME_SELECT_NUM_PAGES - 1)
    {
      game_loop_change_screen(GAME_SCREEN_SAVE_GAME_SELECT, 0, G_screen_page + 1, 0);
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
  }
  else if (G_game_screen == GAME_SCREEN_WORLD_SELECT)
  {
    if (G_screen_page < SCREEN_WORLD_SELECT_NUM_PAGES - 1)
    {
      if (((G_screen_page == 0) && (G_save_game_percent >= 330)) || 
          ((G_screen_page == 1) && (G_save_game_percent >= 660)))
      {
        game_loop_change_screen(GAME_SCREEN_WORLD_SELECT, 0, G_screen_page + 1, 0);
        doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
      }
    }
  }
  else if (G_game_screen == GAME_SCREEN_OPTIONS)
  {
    /* note that the doremi_play_sfx calls occur within the functions called */
    /* standard options */
    if (G_screen_alternate == SCREEN_OPTIONS_ALTERNATE_STANDARD)
    {
      if (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_WINDOW_SIZE)
      {
        graphics_increase_window_size();
      }
      else if ( (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_FULLSCREEN) && 
                (G_flag_window_fullscreen == 0))
      {
        graphics_toggle_fullscreen();
      }
      else if ( (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_VSYNC) && 
                (G_flag_window_vsync == 0))
      {
        graphics_toggle_vsync();
      }
      else if (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_BRIGHTNESS)
      {
        video_increase_brightness();
      }
      else if (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_UPSCALING_MODE)
      {
        video_cycle_upscaling_mode_right();
      }
    }
    /* more options */
    else if (G_screen_alternate == SCREEN_OPTIONS_ALTERNATE_MORE)
    {
      if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_SOUND_VOLUME)
      {
        doremi_increase_volume();
      }
      else if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_GAMEPAD_NUMBER)
      {
        controls_next_gamepad();
      }
      else if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_GAME_BUTTONS)
      {
        controls_game_button_layout_right();
      }
      else if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_MENU_BUTTONS)
      {
        controls_menu_button_layout_right();
      }
      else if (G_screen_choice == SCREEN_MORE_OPTIONS_CHOICE_MENU_KEYS)
      {
        controls_menu_key_layout_right();
      }
    }
  }
  else if (G_game_screen == GAME_SCREEN_HELP)
  {
    if (G_screen_page < SCREEN_HELP_NUM_PAGES - 1)
    {
      game_loop_change_screen(GAME_SCREEN_HELP, 0, G_screen_page + 1, 0);
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
  }

  return 0;
}

/*******************************************************************************
** menu_player_input_button_b_pressed()
*******************************************************************************/
short int menu_player_input_button_b_pressed()
{
  /* title screen choice selected */
  if (G_game_screen == GAME_SCREEN_TITLE)
  {
    if (G_screen_choice == SCREEN_TITLE_CHOICE_PLAY_GAME)
    {
      fade_start_transition(GAME_SCREEN_SAVE_GAME_SELECT, 0, 0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
    else if (G_screen_choice == SCREEN_TITLE_CHOICE_OPTIONS)
    {
      fade_start_transition(GAME_SCREEN_OPTIONS, SCREEN_OPTIONS_ALTERNATE_STANDARD, 0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
    else if (G_screen_choice == SCREEN_TITLE_CHOICE_HELP)
    {
      fade_start_transition(GAME_SCREEN_HELP, 0, 0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
    else if (G_screen_choice == SCREEN_TITLE_CHOICE_CREDITS)
    {
      fade_start_transition(GAME_SCREEN_CREDITS, 0, 0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
    else if (G_screen_choice == SCREEN_TITLE_CHOICE_QUIT)
    {
      fade_start_transition(GAME_SCREEN_QUIT, 0, 0, 0);
    }
  }
  /* save game select screen choice selected */
  else if (G_game_screen == GAME_SCREEN_SAVE_GAME_SELECT)
  {
    if (G_save_game_percent != 0)
    {
      if (G_screen_choice == SCREEN_SAVE_GAME_SELECT_CHOICE_WORLD_SELECT)
      {
        fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 0, 0);
        doremi_play_sfx(SFX_INDEX_MENU_SELECT);
      }
      else if (G_screen_choice == SCREEN_SAVE_GAME_SELECT_CHOICE_STORY_SELECT)
      {
        fade_start_transition(GAME_SCREEN_STORY_SELECT, 0, 0, 0);
        doremi_play_sfx(SFX_INDEX_MENU_SELECT);
      }
      else if (G_screen_choice == SCREEN_SAVE_GAME_SELECT_CHOICE_CLEAR_SAVE_GAME)
      {
        game_loop_change_screen(GAME_SCREEN_CONFIRM_CLEAR_SAVE_GAME, 0, 
                                0, 
                                SCREEN_CONFIRM_CLEAR_SAVE_GAME_CHOICE_NO);
        doremi_play_sfx(SFX_INDEX_MENU_SELECT);
      }
    }
    else
    {
      fade_start_transition(GAME_SCREEN_STORY_SCENE, 0, 0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
  }
  /* confirm clear save game choice selected */
  else if (G_game_screen == GAME_SCREEN_CONFIRM_CLEAR_SAVE_GAME)
  {
    if (G_screen_choice == SCREEN_CONFIRM_CLEAR_SAVE_GAME_CHOICE_YES)
    {
      save_game_clear();
      game_loop_change_screen(GAME_SCREEN_SAVE_GAME_SELECT, 0, 
                              G_save_game_current_slot - 1, 0);
      doremi_play_sfx(SFX_INDEX_KABOOM);
    }
    else if (G_screen_choice == SCREEN_CONFIRM_CLEAR_SAVE_GAME_CHOICE_NO)
    {
      game_loop_change_screen(GAME_SCREEN_SAVE_GAME_SELECT, 0, 
                              G_save_game_current_slot - 1, 
                              SCREEN_SAVE_GAME_SELECT_CHOICE_CLEAR_SAVE_GAME);
      doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
    }
  }
  /* world select screen choice selected */
  else if (G_game_screen == GAME_SCREEN_WORLD_SELECT)
  {
    if ((G_screen_page >= 0) && (G_screen_page < SCREEN_WORLD_SELECT_NUM_PAGES))
    {
      fade_start_transition(GAME_SCREEN_ROOM_SELECT, 
                            G_screen_page * SCREEN_WORLD_SELECT_NUM_CHOICES + G_screen_choice,
                            0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
  }
  /* room select screen choice selected */
  else if (G_game_screen == GAME_SCREEN_ROOM_SELECT)
  {
    if ((G_screen_alternate >= 0) && (G_screen_alternate < SCREEN_ROOM_SELECT_NUM_ALTERNATES))
    {
      if (G_screen_choice <= G_save_game_data[G_screen_alternate])
      {
        G_current_room = G_screen_alternate * SCREEN_ROOM_SELECT_NUM_CHOICES + G_screen_choice;
        fade_start_transition(GAME_SCREEN_PLAYING, SCREEN_PLAYING_ALTERNATE_LOAD_ROOM, 0, 0);
        doremi_play_sfx(SFX_INDEX_MENU_SELECT);
      }
    }
  }
  /* story select screen choice selected */
  else if (G_game_screen == GAME_SCREEN_STORY_SELECT)
  {
    if ((G_screen_choice >= 0) && (G_screen_choice < SCREEN_STORY_SELECT_NUM_CHOICES))
    {
      fade_start_transition(GAME_SCREEN_STORY_SCENE, G_screen_choice, 0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
  }
  /* story scene exited */
  else if (G_game_screen == GAME_SCREEN_STORY_SCENE)
  {
    if (G_last_screen == GAME_SCREEN_STORY_SELECT)
      fade_start_transition(GAME_SCREEN_STORY_SELECT, 0, 0, G_screen_alternate);
    else if (G_last_screen == GAME_SCREEN_ROOM_COMPLETE_PANEL)
    {
      if (G_screen_alternate == 0)
        fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 0, 0);
      else if (G_screen_alternate == 1)
        fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 1, 0);
      else if (G_screen_alternate == 2)
        fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 2, 0);
      else
        fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 0, 0);
    }
    else if (G_last_screen == GAME_SCREEN_SAVE_GAME_SELECT)
      fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 0, 0);
    else
      fade_start_transition(GAME_SCREEN_STORY_SELECT, 0, 0, 0);

    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* options screen choice selected */
  else if (G_game_screen == GAME_SCREEN_OPTIONS)
  {
    /* standard options */
    if (G_screen_alternate == SCREEN_OPTIONS_ALTERNATE_STANDARD)
    {
      if (G_screen_choice == SCREEN_STANDARD_OPTIONS_CHOICE_MORE_OPTIONS)
      {
        if (G_last_screen == GAME_SCREEN_TITLE)
          fade_start_transition(GAME_SCREEN_OPTIONS, SCREEN_OPTIONS_ALTERNATE_MORE, 0, 0);
        else
          game_loop_change_screen(GAME_SCREEN_OPTIONS, SCREEN_OPTIONS_ALTERNATE_MORE, 0, 0);

        doremi_play_sfx(SFX_INDEX_MENU_SELECT);
      }
    }
  }
  /* room intro panel skipped */
  else if (G_game_screen == GAME_SCREEN_ROOM_INTRO_PANEL)
  {
    vb_menu_clear_panels_and_overlay();
    game_loop_change_screen(GAME_SCREEN_PLAYING, SCREEN_PLAYING_ALTERNATE_STANDARD, 0, 0);
  }
  /* room complete panel skipped */
  else if (G_game_screen == GAME_SCREEN_ROOM_COMPLETE_PANEL)
  {
    fade_start_transition(GAME_SCREEN_ROOM_COMPLETE_PANEL, 
                          SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_CLOSING, 0, 0);
  }
  /* pause panel choice selected */
  else if (G_game_screen == GAME_SCREEN_PAUSE_PANEL)
  {
    if (G_screen_choice == SCREEN_PAUSE_PANEL_CHOICE_BACK_TO_GAME)
    {
      vb_menu_clear_panels_and_overlay();
      game_loop_change_screen(GAME_SCREEN_PLAYING, SCREEN_PLAYING_ALTERNATE_BACK_TO_GAME, 0, 0);
      doremi_play_sfx(SFX_INDEX_PAUSE);
    }
    else if (G_screen_choice == SCREEN_PAUSE_PANEL_CHOICE_RETRY_ROOM)
    {
      game_loop_change_screen(GAME_SCREEN_RETRY_PANEL, 0, 0, 0);
      doremi_play_sfx(SFX_INDEX_MARBLE_DROP);
    }
    else if (G_screen_choice == SCREEN_PAUSE_PANEL_CHOICE_OPTIONS)
    {
      game_loop_change_screen(GAME_SCREEN_OPTIONS, SCREEN_OPTIONS_ALTERNATE_STANDARD, 0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
    else if (G_screen_choice == SCREEN_PAUSE_PANEL_CHOICE_HELP)
    {
      game_loop_change_screen(GAME_SCREEN_HELP, 0, 0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
    else if (G_screen_choice == SCREEN_PAUSE_PANEL_CHOICE_QUIT_TO_ROOM_SELECT)
    {
      fade_start_transition(GAME_SCREEN_ROOM_SELECT, 
                            G_current_room / LEVEL_ROOMS_PER_WORLD, 
                            0, G_current_room % LEVEL_ROOMS_PER_WORLD);
      doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
    }
  }
  /* retry panel choice selected */
  else if (G_game_screen == GAME_SCREEN_RETRY_PANEL)
  {
    if (G_screen_choice == SCREEN_RETRY_PANEL_CHOICE_YES)
    {
      fade_start_transition(GAME_SCREEN_PLAYING, SCREEN_PLAYING_ALTERNATE_RESET_ROOM, 0, 0);
      doremi_play_sfx(SFX_INDEX_MENU_SELECT);
    }
    else if (G_screen_choice == SCREEN_RETRY_PANEL_CHOICE_NO)
    {
      game_loop_change_screen(GAME_SCREEN_PAUSE_PANEL, 0, 
                              0, SCREEN_PAUSE_PANEL_CHOICE_RETRY_ROOM);
      doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
    }
  }

  return 0;
}

/*******************************************************************************
** menu_player_input_button_a_pressed()
*******************************************************************************/
short int menu_player_input_button_a_pressed()
{
  /* title screen - set cursor to the quit location */
  if (G_game_screen == GAME_SCREEN_TITLE)
  {
    G_screen_choice = SCREEN_TITLE_CHOICE_QUIT;
  }
  /* save game select screen exited */
  else if (G_game_screen == GAME_SCREEN_SAVE_GAME_SELECT)
  {
    fade_start_transition(GAME_SCREEN_TITLE, 0, 0, SCREEN_TITLE_CHOICE_PLAY_GAME);
    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* confirm clear save game screen exited */
  else if (G_game_screen == GAME_SCREEN_CONFIRM_CLEAR_SAVE_GAME)
  {
    game_loop_change_screen(GAME_SCREEN_SAVE_GAME_SELECT, 0, 
                            G_save_game_current_slot - 1, 
                            SCREEN_SAVE_GAME_SELECT_CHOICE_CLEAR_SAVE_GAME);
    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* world select screen exited */
  else if (G_game_screen == GAME_SCREEN_WORLD_SELECT)
  {
    fade_start_transition(GAME_SCREEN_SAVE_GAME_SELECT, 0, 
                          G_save_game_current_slot - 1, 
                          SCREEN_SAVE_GAME_SELECT_CHOICE_WORLD_SELECT);
    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* room select screen exited */
  else if (G_game_screen == GAME_SCREEN_ROOM_SELECT)
  {
    fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 
                          G_screen_alternate / SCREEN_WORLD_SELECT_NUM_CHOICES, 
                          G_screen_alternate % SCREEN_WORLD_SELECT_NUM_CHOICES);
    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* story select screen exited */
  else if (G_game_screen == GAME_SCREEN_STORY_SELECT)
  {
    fade_start_transition(GAME_SCREEN_SAVE_GAME_SELECT, 0, 
                          G_save_game_current_slot - 1, 
                          SCREEN_SAVE_GAME_SELECT_CHOICE_STORY_SELECT);
    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* story scene exited */
  else if (G_game_screen == GAME_SCREEN_STORY_SCENE)
  {
    if (G_last_screen == GAME_SCREEN_STORY_SELECT)
      fade_start_transition(GAME_SCREEN_STORY_SELECT, 0, 0, G_screen_alternate);
    else if (G_last_screen == GAME_SCREEN_ROOM_COMPLETE_PANEL)
    {
      if (G_screen_alternate == 0)
        fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 0, 0);
      else if (G_screen_alternate == 1)
        fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 1, 0);
      else if (G_screen_alternate == 2)
        fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 2, 0);
      else
        fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 0, 0);
    }
    else if (G_last_screen == GAME_SCREEN_SAVE_GAME_SELECT)
      fade_start_transition(GAME_SCREEN_WORLD_SELECT, 0, 0, 0);
    else
      fade_start_transition(GAME_SCREEN_STORY_SELECT, 0, 0, 0);

    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* options screen exited */
  else if (G_game_screen == GAME_SCREEN_OPTIONS)
  {
    if (G_screen_alternate == SCREEN_OPTIONS_ALTERNATE_STANDARD)
    {
      options_save();

      if (G_last_screen == GAME_SCREEN_TITLE)
        fade_start_transition(GAME_SCREEN_TITLE, 0, 0, SCREEN_TITLE_CHOICE_OPTIONS);
      else if (G_last_screen == GAME_SCREEN_PAUSE_PANEL)
        game_loop_change_screen(GAME_SCREEN_PAUSE_PANEL, 0, 0, SCREEN_PAUSE_PANEL_CHOICE_OPTIONS);
      else
        fade_start_transition(GAME_SCREEN_TITLE, 0, 0, SCREEN_TITLE_CHOICE_OPTIONS);
    }
    else if (G_screen_alternate == SCREEN_OPTIONS_ALTERNATE_MORE)
    {
      if (G_last_screen == GAME_SCREEN_TITLE)
      {
        fade_start_transition(GAME_SCREEN_OPTIONS, SCREEN_OPTIONS_ALTERNATE_STANDARD, 
                              0, SCREEN_STANDARD_OPTIONS_CHOICE_MORE_OPTIONS);
      }
      else
      {
        game_loop_change_screen(GAME_SCREEN_OPTIONS, SCREEN_OPTIONS_ALTERNATE_STANDARD, 
                                0, SCREEN_STANDARD_OPTIONS_CHOICE_MORE_OPTIONS);
      }
    }

    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* help screen exited */
  else if (G_game_screen == GAME_SCREEN_HELP)
  {
    if (G_last_screen == GAME_SCREEN_TITLE)
      fade_start_transition(GAME_SCREEN_TITLE, 0, 0, SCREEN_TITLE_CHOICE_HELP);
    else if (G_last_screen == GAME_SCREEN_PAUSE_PANEL)
      game_loop_change_screen(GAME_SCREEN_PAUSE_PANEL, 0, 0, SCREEN_PAUSE_PANEL_CHOICE_HELP);
    else
      fade_start_transition(GAME_SCREEN_TITLE, 0, 0, SCREEN_TITLE_CHOICE_HELP);

    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* credits screen exited */
  else if (G_game_screen == GAME_SCREEN_CREDITS)
  {
    fade_start_transition(GAME_SCREEN_TITLE, 0, 0, SCREEN_TITLE_CHOICE_CREDITS);

    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }
  /* room intro panel skipped */
  else if (G_game_screen == GAME_SCREEN_ROOM_INTRO_PANEL)
  {
    vb_menu_clear_panels_and_overlay();
    game_loop_change_screen(GAME_SCREEN_PLAYING, SCREEN_PLAYING_ALTERNATE_STANDARD, 0, 0);
  }
  /* room complete panel skipped */
  else if (G_game_screen == GAME_SCREEN_ROOM_COMPLETE_PANEL)
  {
    fade_start_transition(GAME_SCREEN_ROOM_COMPLETE_PANEL, 
                          SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_CLOSING, 0, 0);
  }
  /* pause panel closed */
  else if (G_game_screen == GAME_SCREEN_PAUSE_PANEL)
  {
    game_loop_change_screen(GAME_SCREEN_PLAYING, SCREEN_PLAYING_ALTERNATE_BACK_TO_GAME, 0, 0);
    vb_menu_clear_panels_and_overlay();
    doremi_play_sfx(SFX_INDEX_PAUSE);
  }
  /* retry panel closed */
  else if (G_game_screen == GAME_SCREEN_RETRY_PANEL)
  {
    game_loop_change_screen(GAME_SCREEN_PAUSE_PANEL, 0, 
                            0, SCREEN_PAUSE_PANEL_CHOICE_RETRY_ROOM);
    doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
  }

  return 0;
}

/*******************************************************************************
** menu_player_input_button_start_pressed()
*******************************************************************************/
short int menu_player_input_button_start_pressed()
{
  /* if currently playing the game, open the pause panel */
  if (G_game_screen == GAME_SCREEN_PLAYING)
  {
    G_saved_timer_count = G_timer_count;
    game_loop_change_screen(GAME_SCREEN_PAUSE_PANEL, 0, 0, 0);
    doremi_play_sfx(SFX_INDEX_PAUSE);
  }

  return 0;
}

