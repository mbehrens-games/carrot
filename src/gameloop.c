/*******************************************************************************
** gameloop.c (main game loop functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "doremi.h"
#include "fade.h"
#include "gameloop.h"
#include "global.h"
#include "grid.h"
#include "level.h"
#include "logic.h"
#include "menu.h"
#include "render.h"
#include "savegame.h"
#include "screen.h"
#include "vbmenu.h"
#include "vbsprite.h"
#include "vbtile.h"
#include "video.h"
#include "world.h"

/*******************************************************************************
** game_loop_load_room()
*******************************************************************************/
short int game_loop_load_room(int index)
{
  if ((index < 0) || (index >= LEVEL_NUM_ROOMS))
    return 1;

  /* set current room and world */
  G_current_room = index;
  G_current_world = G_current_room / LEVEL_ROOMS_PER_WORLD;

  /* reset timer count */
  G_timer_count = 0;

  /* reset vbos */
  render_reset_vbos();

  /* initialize tile layer vbos */
  vb_tile_load_background_tilemaps();

  /* initalize grid and world */
  grid_load_objects_from_tilemap();
  world_load_things_from_tilemap();

  return 0;
}

/*******************************************************************************
** game_loop_reset_room()
*******************************************************************************/
short int game_loop_reset_room()
{
  /* reset timer count */
  G_timer_count = 0;

  /* initalize grid and world */
  grid_load_objects_from_tilemap();
  world_load_things_from_tilemap();

  return 0;
}

/*******************************************************************************
** game_loop_change_screen()
*******************************************************************************/
short int game_loop_change_screen(int screen, int alternate, int page, int choice)
{
  /* make sure screen is valid */
  if ((screen < 0) || (screen >= GAME_NUM_SCREENS))
    return 0;

  /* update screen if necessary */
  if (screen != G_game_screen)
  {
    G_last_screen = G_game_screen;
    G_game_screen = screen;
  }

  /* map the gamepad buttons (needed if in the YB/BA button layout) */
  controls_map_buttons();

  /* title screen */
  if (screen == GAME_SCREEN_TITLE)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_TITLE_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_title_screen();
    doremi_play_music(MUSIC_TRACK_MENU);
  }
  /* save game select */
  else if (screen == GAME_SCREEN_SAVE_GAME_SELECT)
  {
    G_screen_alternate = 0;

    if ((page >= 0) && (page < SCREEN_SAVE_GAME_SELECT_NUM_PAGES))
      G_screen_page = page;
    else
      G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_SAVE_GAME_SELECT_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    if (page == 0)
      save_game_set_slot(1);
    else if (page == 1)
      save_game_set_slot(2);
    else if (page == 2)
      save_game_set_slot(3);
    else
      save_game_set_slot(1);

    vb_menu_load_save_game_select_screen();
    doremi_play_music(MUSIC_TRACK_MENU);
  }
  /* confirm clear save game */
  else if (screen == GAME_SCREEN_CONFIRM_CLEAR_SAVE_GAME)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_CONFIRM_CLEAR_SAVE_GAME_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_confirm_clear_save_game_screen();
  }
  /* world select */
  else if (screen == GAME_SCREEN_WORLD_SELECT)
  {
    G_screen_alternate = 0;

    if ((page >= 0) && (page < SCREEN_WORLD_SELECT_NUM_PAGES))
      G_screen_page = page;
    else
      G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_WORLD_SELECT_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_world_select_screen();
    doremi_play_music(MUSIC_TRACK_MENU);
  }
  /* room select */
  else if (screen == GAME_SCREEN_ROOM_SELECT)
  {
    if ((alternate >= 0) && (alternate < SCREEN_ROOM_SELECT_NUM_ALTERNATES))
      G_screen_alternate = alternate;
    else
      G_screen_alternate = 0;

    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_ROOM_SELECT_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_room_select_screen();
  }
  /* story select */
  else if (screen == GAME_SCREEN_STORY_SELECT)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_STORY_SELECT_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_story_select_screen();
  }
  /* story scene */
  else if (screen == GAME_SCREEN_STORY_SCENE)
  {
    if ((alternate >= 0) && (alternate < SCREEN_STORY_SCENE_NUM_ALTERNATES))
      G_screen_alternate = alternate;
    else
      G_screen_alternate = 0;

    G_screen_page = 0;
    G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_story_scene_screen();
    doremi_play_music(MUSIC_TRACK_STORY);
  }
  /* options */
  else if (screen == GAME_SCREEN_OPTIONS)
  {
    if ((alternate >= 0) && (alternate < SCREEN_OPTIONS_NUM_ALTERNATES))
      G_screen_alternate = alternate;
    else
      G_screen_alternate = 0;

    G_screen_page = 0;

    if ((alternate == SCREEN_OPTIONS_ALTERNATE_STANDARD) && 
        (choice >= 0) && (choice < SCREEN_STANDARD_OPTIONS_NUM_CHOICES))
    {
      G_screen_choice = choice;
    }
    else if ( (alternate == SCREEN_OPTIONS_ALTERNATE_VIDEO) && 
              (choice >= 0) && (choice < SCREEN_VIDEO_OPTIONS_NUM_CHOICES))
    {
      G_screen_choice = choice;
    }
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_options_screen();
  }
  /* help */
  else if (screen == GAME_SCREEN_HELP)
  {
    G_screen_alternate = 0;

    if ((page >= 0) && (page < SCREEN_HELP_NUM_PAGES))
      G_screen_page = page;
    else
      G_screen_page = 0;

    G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_help_screen();
  }
  /* credits */
  else if (screen == GAME_SCREEN_CREDITS)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;
    G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_credits_screen();
  }
  /* playing */
  else if (screen == GAME_SCREEN_PLAYING)
  {
    if (alternate == SCREEN_PLAYING_ALTERNATE_LOAD_ROOM)
    {
      G_screen_alternate = SCREEN_PLAYING_ALTERNATE_STANDARD;
      G_screen_page = 0;
      G_screen_choice = 0;

      G_timer_count = 0;
      game_loop_load_room(G_current_room);

      if (G_current_room / 8 == 0)
        doremi_play_music(MUSIC_TRACK_WORLD_01);
      else if (G_current_room / 8 == 1)
        doremi_play_music(MUSIC_TRACK_WORLD_02);
      else if (G_current_room / 8 == 2)
        doremi_play_music(MUSIC_TRACK_WORLD_03);
      else if (G_current_room / 8 == 3)
        doremi_play_music(MUSIC_TRACK_WORLD_04);
      else if (G_current_room / 8 == 4)
        doremi_play_music(MUSIC_TRACK_WORLD_05);
      else if (G_current_room / 8 == 5)
        doremi_play_music(MUSIC_TRACK_WORLD_06);
      else if (G_current_room / 8 == 6)
        doremi_play_music(MUSIC_TRACK_WORLD_07);
      else if (G_current_room / 8 == 7)
        doremi_play_music(MUSIC_TRACK_WORLD_08);
      else if (G_current_room / 8 == 8)
        doremi_play_music(MUSIC_TRACK_WORLD_09);
      else if (G_current_room / 8 == 9)
        doremi_play_music(MUSIC_TRACK_WORLD_10);
      else if (G_current_room / 8 == 10)
        doremi_play_music(MUSIC_TRACK_WORLD_11);
      else if (G_current_room / 8 == 11)
        doremi_play_music(MUSIC_TRACK_WORLD_12);
      else
        doremi_play_music(MUSIC_TRACK_WORLD_01);

      game_loop_change_screen(GAME_SCREEN_ROOM_INTRO_PANEL, 0, 0, 0);
    }
    else if (alternate == SCREEN_PLAYING_ALTERNATE_RESET_ROOM)
    {
      G_screen_alternate = SCREEN_PLAYING_ALTERNATE_STANDARD;
      G_screen_page = 0;
      G_screen_choice = 0;

      G_timer_count = 0;
      game_loop_reset_room();

      vb_sprite_load_grid_objects();
      vb_sprite_load_things();
    }
    else if (alternate == SCREEN_PLAYING_ALTERNATE_BACK_TO_GAME)
    {
      G_screen_alternate = SCREEN_PLAYING_ALTERNATE_STANDARD;
      G_screen_page = 0;
      G_screen_choice = 0;

      G_timer_count = G_saved_timer_count;
    }
    else if (alternate == SCREEN_PLAYING_ALTERNATE_NEXT_ROOM)
    {
      /* compute next room and load it! */
      G_current_room = (G_current_world * LEVEL_ROOMS_PER_WORLD) + ((G_current_room + 1) % LEVEL_ROOMS_PER_WORLD);
      game_loop_change_screen(GAME_SCREEN_PLAYING, SCREEN_PLAYING_ALTERNATE_LOAD_ROOM, 0, 0);
    }
    else
    {
      G_screen_alternate = SCREEN_PLAYING_ALTERNATE_STANDARD;
      G_screen_page = 0;
      G_screen_choice = 0;

      G_timer_count = 0;
    }
  }
  /* room intro panel */
  else if (screen == GAME_SCREEN_ROOM_INTRO_PANEL)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;
    G_screen_choice = 0;

    G_timer_count = 0;

    vb_sprite_load_grid_objects();
    vb_sprite_load_things();
    vb_menu_load_room_intro_panel();
  }
  /* room complete panel */
  else if (screen == GAME_SCREEN_ROOM_COMPLETE_PANEL)
  {
    if (alternate == SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_STANDARD)
    {
      G_screen_alternate = SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_STANDARD;
      G_screen_page = 0;
      G_screen_choice = 0;

      G_timer_count = 0;

      save_game_mark_current_room_complete();

      vb_sprite_load_grid_objects();
      vb_sprite_load_things();
      vb_menu_load_room_complete_panel();
    }
    else if (alternate == SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_CLOSING)
    {
      if (G_flag_world_complete == 2)
      {
        G_flag_world_complete = 0;

        if (G_save_game_percent < 330)
          game_loop_change_screen(GAME_SCREEN_STORY_SCENE, 0, 0, 0);
        else if (G_save_game_percent < 660)
          game_loop_change_screen(GAME_SCREEN_STORY_SCENE, 1, 0, 0);
        else if (G_save_game_percent < 1000)
          game_loop_change_screen(GAME_SCREEN_STORY_SCENE, 2, 0, 0);
        else
          game_loop_change_screen(GAME_SCREEN_STORY_SCENE, 3, 0, 0);
      }
      else if (G_flag_world_complete == 1)
      {
        G_flag_world_complete = 0;

        game_loop_change_screen(GAME_SCREEN_WORLD_SELECT, 0, 
                                G_current_world / SCREEN_WORLD_SELECT_NUM_CHOICES, 
                                G_current_world % SCREEN_WORLD_SELECT_NUM_CHOICES);
      }
      else
        game_loop_change_screen(GAME_SCREEN_PLAYING, SCREEN_PLAYING_ALTERNATE_NEXT_ROOM, 0, 0);
    }
    else
    {
      G_screen_alternate = SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_STANDARD;
      G_screen_page = 0;
      G_screen_choice = 0;

      G_timer_count = 0;

      save_game_mark_current_room_complete();

      vb_sprite_load_grid_objects();
      vb_sprite_load_things();
      vb_menu_load_room_complete_panel();
    }
  }
  /* pause panel */
  else if (screen == GAME_SCREEN_PAUSE_PANEL)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_PAUSE_PANEL_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_pause_panel();
  }
  /* retry panel */
  else if (screen == GAME_SCREEN_RETRY_PANEL)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_RETRY_PANEL_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_retry_panel();
  }
  /* quit command issued */
  else if (screen == GAME_SCREEN_QUIT)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;
    G_screen_choice = 0;

    G_flag_quit_game = 1;
  }

  return 0;
}

/*******************************************************************************
** game_loop_advance_frame()
*******************************************************************************/
short int game_loop_advance_frame()
{
  /* update fade transition if necessary */
  if (G_fade_state == FADE_STATE_FADEOUT)
  {
    fade_update_transition();

    /* if the switch to the fade-in occurred, load destination game screen */
    if (G_fade_state == FADE_STATE_FADEIN)
    {
      game_loop_change_screen(G_fade_screen, G_fade_alternate, 
                              G_fade_page, G_fade_choice);

      if (G_flag_quit_game == 1)
        return 0;
    }
  }
  else if (G_fade_state == FADE_STATE_FADEIN)
  {
    fade_update_transition();
  }

  /* if we are not in a fade transition,        */
  /* process player input and update game state */
  if (G_fade_state == FADE_STATE_OFF)
  {
    /* process player input */
    if (G_game_screen == GAME_SCREEN_PLAYING)
    {
      G_flag_redraw_grid_objects = 0;
      controls_process_player_input_gameplay();
    }
    else
    {
      controls_process_player_input_menus();

      if (G_flag_quit_game == 1)
        return 0;
    }

    /* advance past room intro panel if delay time has elapsed */
    if (G_game_screen == GAME_SCREEN_ROOM_INTRO_PANEL)
    {
      if (G_timer_count >= 120)
        game_loop_change_screen(GAME_SCREEN_PLAYING, SCREEN_PLAYING_ALTERNATE_STANDARD, 0, 0);
    }

    /* advance past room complete panel if delay time has elapsed */
    if (G_game_screen == GAME_SCREEN_ROOM_COMPLETE_PANEL)
    {
      if (G_timer_count >= 120)
      {
        fade_start_transition(GAME_SCREEN_ROOM_COMPLETE_PANEL, 
                              SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_CLOSING, 0, 0);
      }
    }

    /* update game state */
    if (G_game_screen == GAME_SCREEN_PLAYING)
    {
      logic_update_frame();

      /* if all carrots are collected, go to the next room! */
      if (G_carrots_remaining == 0)
      {
        game_loop_change_screen(GAME_SCREEN_ROOM_COMPLETE_PANEL, 
                                SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_STANDARD, 0, 0);
        doremi_play_sfx(SFX_INDEX_LEVEL_COMPLETE);
      }
    }
  }

  /* update vbos */
  if (G_game_screen == GAME_SCREEN_TITLE)
  {
    vb_menu_load_title_screen_cursor();
  }
  else if (G_game_screen == GAME_SCREEN_SAVE_GAME_SELECT)
  {
    vb_menu_load_save_game_select_screen_cursor();
  }
  else if (G_game_screen == GAME_SCREEN_CONFIRM_CLEAR_SAVE_GAME)
  {
    vb_menu_load_confirm_clear_save_game_screen_cursor();
  }
  else if (G_game_screen == GAME_SCREEN_WORLD_SELECT)
  {
    vb_menu_load_world_select_screen_cursor();
  }
  else if (G_game_screen == GAME_SCREEN_ROOM_SELECT)
  {
    vb_menu_load_room_select_screen_cursor();
  }
  else if (G_game_screen == GAME_SCREEN_STORY_SELECT)
  {
    vb_menu_load_story_select_screen_cursor();
  }
  else if (G_game_screen == GAME_SCREEN_OPTIONS)
  {
    vb_menu_load_options_screen_cursor();
  }
  else if (G_game_screen == GAME_SCREEN_HELP)
  {
    vb_menu_load_help_screen_cursor();
  }
  else if (G_game_screen == GAME_SCREEN_PLAYING)
  {
    if ((G_timer_count % 6 == 0) || (G_flag_redraw_grid_objects == 1))
      vb_sprite_load_grid_objects();

    vb_sprite_load_things();
  }
  else if (G_game_screen == GAME_SCREEN_PAUSE_PANEL)
  {
    vb_menu_load_pause_panel_cursor();
  }
  else if (G_game_screen == GAME_SCREEN_RETRY_PANEL)
  {
    vb_menu_load_retry_panel_cursor();
  }

  /* rendering (first pass - tiles & sprites to overscan texture) */
  if ((G_fade_state == FADE_STATE_FADEOUT) || 
      (G_fade_state == FADE_STATE_FADEIN))
  {
    render_scene_fade();
  }
  else
    render_scene_standard();

  /* rendering (second pass - from overscan texture to window) */
  if (G_upscaling_mode == VIDEO_UPSCALING_MODE_LINEAR)
    render_postprocessing_path_A();
  else if (G_upscaling_mode == VIDEO_UPSCALING_MODE_TV)
  {
    if ((G_blur_filter_sigma_index == 0) && (G_mask_opacity_index == 0))
      render_postprocessing_path_ACE();
    else if ((G_blur_filter_sigma_index != 0) && (G_mask_opacity_index == 0))
      render_postprocessing_path_ABCE();
    else if ((G_blur_filter_sigma_index == 0) && (G_mask_opacity_index != 0))
      render_postprocessing_path_ACDE();
    else
      render_postprocessing_path_ABCDE();
  }
  else
    render_postprocessing_path_A();

  /* if we are not in a fade transition,  */
  /* update global timer count            */
  if (G_fade_state == FADE_STATE_OFF)
  {
    G_timer_count += 1;

    if (G_timer_count >= 240)
      G_timer_count = 0;
  }

  return 0;
}

