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

  /* map the gamepad buttons & keyboard keys (this is */
  /* needed when we switch to/from the menu screens)  */
  controls_map_gamepad();
  controls_map_keyboard();

  /* title screen */
  if (G_game_screen == GAME_SCREEN_TITLE)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_TITLE_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_tile_load_menu_background();
    vb_sprite_clear_grid_objects_and_things();
    vb_menu_load_title_panels();
    vb_menu_load_title_overlay();

    doremi_play_music(MUSIC_TRACK_MENU);
  }
  /* save game select */
  else if (G_game_screen == GAME_SCREEN_SAVE_GAME_SELECT)
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

    vb_tile_load_menu_background();
    vb_sprite_clear_grid_objects_and_things();
    vb_menu_load_save_game_select_panels();
    vb_menu_load_save_game_select_overlay();

    doremi_play_music(MUSIC_TRACK_MENU);
  }
  /* confirm clear save game */
  else if (G_game_screen == GAME_SCREEN_CONFIRM_CLEAR_SAVE_GAME)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_CONFIRM_CLEAR_SAVE_GAME_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_tile_load_menu_background();
    vb_sprite_clear_grid_objects_and_things();
    vb_menu_load_confirm_clear_save_game_panels();
    vb_menu_load_confirm_clear_save_game_overlay();
  }
  /* world select */
  else if (G_game_screen == GAME_SCREEN_WORLD_SELECT)
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

    vb_tile_load_menu_background();
    vb_sprite_clear_grid_objects_and_things();
    vb_menu_load_world_select_panels();
    vb_menu_load_world_select_overlay();
    doremi_play_music(MUSIC_TRACK_MENU);
  }
  /* room select */
  else if (G_game_screen == GAME_SCREEN_ROOM_SELECT)
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

    vb_tile_load_menu_background();
    vb_sprite_clear_grid_objects_and_things();
    vb_menu_load_room_select_panels();
    vb_menu_load_room_select_overlay();
  }
  /* story select */
  else if (G_game_screen == GAME_SCREEN_STORY_SELECT)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_STORY_SELECT_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_tile_load_menu_background();
    vb_sprite_clear_grid_objects_and_things();
    vb_menu_load_story_select_panels();
    vb_menu_load_story_select_overlay();
  }
  /* story scene */
  else if (G_game_screen == GAME_SCREEN_STORY_SCENE)
  {
    if ((alternate >= 0) && (alternate < SCREEN_STORY_SCENE_NUM_ALTERNATES))
      G_screen_alternate = alternate;
    else
      G_screen_alternate = 0;

    G_screen_page = 0;
    G_screen_choice = 0;

    G_timer_count = 0;

    vb_tile_load_menu_background();
    vb_sprite_clear_grid_objects_and_things();
    vb_menu_load_story_scene_panels();
    vb_menu_load_story_scene_overlay();

    doremi_play_music(MUSIC_TRACK_STORY);
  }
  /* options */
  else if (G_game_screen == GAME_SCREEN_OPTIONS)
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
    else if ( (alternate == SCREEN_OPTIONS_ALTERNATE_MORE) && 
              (choice >= 0) && (choice < SCREEN_MORE_OPTIONS_NUM_CHOICES))
    {
      G_screen_choice = choice;
    }
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    if (G_last_screen == GAME_SCREEN_TITLE)
      vb_tile_load_menu_background();

    vb_menu_load_options_panels();
    vb_menu_load_options_overlay();
  }
  /* help */
  else if (G_game_screen == GAME_SCREEN_HELP)
  {
    G_screen_alternate = 0;

    if ((page >= 0) && (page < SCREEN_HELP_NUM_PAGES))
      G_screen_page = page;
    else
      G_screen_page = 0;

    G_screen_choice = 0;

    G_timer_count = 0;

    if (G_last_screen == GAME_SCREEN_TITLE)
      vb_tile_load_menu_background();

    vb_menu_load_help_panels();
    vb_menu_load_help_overlay();
  }
  /* credits */
  else if (G_game_screen == GAME_SCREEN_CREDITS)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;
    G_screen_choice = 0;

    G_timer_count = 0;

    vb_tile_load_menu_background();
    vb_sprite_clear_grid_objects_and_things();
    vb_menu_load_credits_panels();
    vb_menu_load_credits_overlay();
  }
  /* playing */
  else if (G_game_screen == GAME_SCREEN_PLAYING)
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

      vb_sprite_load_grid_objects_and_things();
      vb_menu_clear_panels_and_overlay();
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
  else if (G_game_screen == GAME_SCREEN_ROOM_INTRO_PANEL)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;
    G_screen_choice = 0;

    G_timer_count = 0;

    vb_sprite_load_grid_objects_and_things();
    vb_menu_load_room_intro_panels();
    vb_menu_load_room_intro_overlay();
  }
  /* room complete panel */
  else if (G_game_screen == GAME_SCREEN_ROOM_COMPLETE_PANEL)
  {
    if (alternate == SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_STANDARD)
    {
      G_screen_alternate = SCREEN_ROOM_COMPLETE_PANEL_ALTERNATE_STANDARD;
      G_screen_page = 0;
      G_screen_choice = 0;

      save_game_mark_current_room_complete();

      vb_sprite_load_grid_objects_and_things();
      vb_menu_load_room_complete_panels();
      vb_menu_load_room_complete_overlay();

      G_timer_count = 0;
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

      save_game_mark_current_room_complete();

      vb_sprite_load_grid_objects_and_things();
      vb_menu_load_room_complete_panels();
      vb_menu_load_room_complete_overlay();

      G_timer_count = 0;
    }
  }
  /* pause panel */
  else if (G_game_screen == GAME_SCREEN_PAUSE_PANEL)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_PAUSE_PANEL_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_pause_panels();
    vb_menu_load_pause_overlay();
  }
  /* retry panel */
  else if (G_game_screen == GAME_SCREEN_RETRY_PANEL)
  {
    G_screen_alternate = 0;
    G_screen_page = 0;

    if ((choice >= 0) && (choice < SCREEN_RETRY_PANEL_NUM_CHOICES))
      G_screen_choice = choice;
    else
      G_screen_choice = 0;

    G_timer_count = 0;

    vb_menu_load_retry_panels();
    vb_menu_load_retry_overlay();
  }
  /* quit command issued */
  else if (G_game_screen == GAME_SCREEN_QUIT)
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
    vb_menu_load_title_overlay();
  else if (G_game_screen == GAME_SCREEN_SAVE_GAME_SELECT)
    vb_menu_load_save_game_select_overlay();
  else if (G_game_screen == GAME_SCREEN_CONFIRM_CLEAR_SAVE_GAME)
    vb_menu_load_confirm_clear_save_game_overlay();
  else if (G_game_screen == GAME_SCREEN_WORLD_SELECT)
    vb_menu_load_world_select_overlay();
  else if (G_game_screen == GAME_SCREEN_ROOM_SELECT)
    vb_menu_load_room_select_overlay();
  else if (G_game_screen == GAME_SCREEN_STORY_SELECT)
    vb_menu_load_story_select_overlay();
  else if (G_game_screen == GAME_SCREEN_STORY_SCENE)
    vb_menu_load_story_scene_overlay();
  else if (G_game_screen == GAME_SCREEN_OPTIONS)
    vb_menu_load_options_overlay();
  else if (G_game_screen == GAME_SCREEN_HELP)
    vb_menu_load_help_overlay();
  else if (G_game_screen == GAME_SCREEN_CREDITS)
    vb_menu_load_credits_overlay();
  else if (G_game_screen == GAME_SCREEN_PLAYING)
  {
    vb_sprite_load_grid_objects_and_things();
    vb_menu_clear_panels_and_overlay();
  }
  else if (G_game_screen == GAME_SCREEN_ROOM_INTRO_PANEL)
    vb_menu_load_room_intro_overlay();
  else if (G_game_screen == GAME_SCREEN_ROOM_COMPLETE_PANEL)
    vb_menu_load_room_complete_overlay();
  else if (G_game_screen == GAME_SCREEN_PAUSE_PANEL)
    vb_menu_load_pause_overlay();
  else if (G_game_screen == GAME_SCREEN_RETRY_PANEL)
    vb_menu_load_retry_overlay();

  /* rendering */
  render_scene_all();

  if (G_upscaling_mode == VIDEO_UPSCALING_MODE_LINEAR)
    render_postprocessing_linear();
  else if (G_upscaling_mode == VIDEO_UPSCALING_MODE_PIXELS)
    render_postprocessing_pixels();
  else if (G_upscaling_mode == VIDEO_UPSCALING_MODE_SCANLINES)
    render_postprocessing_scanlines();
  else
    render_postprocessing_linear();

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

