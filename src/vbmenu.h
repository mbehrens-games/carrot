/*******************************************************************************
** vbmenu.h (vbo population functions - menus)
*******************************************************************************/

#ifndef VB_MENU_H
#define VB_MENU_H

/* function declarations */
short int vb_menu_load_title_panels();
short int vb_menu_load_title_overlay();

short int vb_menu_load_save_game_select_panels();
short int vb_menu_load_save_game_select_overlay();

short int vb_menu_load_confirm_clear_save_game_panels();
short int vb_menu_load_confirm_clear_save_game_overlay();

short int vb_menu_load_world_select_panels();
short int vb_menu_load_world_select_overlay();

short int vb_menu_load_room_select_panels();
short int vb_menu_load_room_select_overlay();

short int vb_menu_load_story_select_panels();
short int vb_menu_load_story_select_overlay();

short int vb_menu_load_story_scene_panels();
short int vb_menu_load_story_scene_overlay();

short int vb_menu_load_options_panels();
short int vb_menu_load_options_overlay();

short int vb_menu_load_help_panels();
short int vb_menu_load_help_overlay();

short int vb_menu_load_credits_panels();
short int vb_menu_load_credits_overlay();

short int vb_menu_load_room_intro_panels();
short int vb_menu_load_room_intro_overlay();

short int vb_menu_load_room_complete_panels();
short int vb_menu_load_room_complete_overlay();

short int vb_menu_load_pause_panels();
short int vb_menu_load_pause_overlay();

short int vb_menu_load_retry_panels();
short int vb_menu_load_retry_overlay();

short int vb_menu_clear_panels_and_overlay();

#endif
