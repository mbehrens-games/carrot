/*******************************************************************************
** vbmenu.h (vbo population functions - menus)
*******************************************************************************/

#ifndef VB_MENU_H
#define VB_MENU_H

/* function declarations */
short int vb_menu_load_title_screen();
short int vb_menu_load_title_screen_cursor();

short int vb_menu_load_save_game_select_screen();
short int vb_menu_load_save_game_select_screen_cursor();

short int vb_menu_load_confirm_clear_save_game_screen();
short int vb_menu_load_confirm_clear_save_game_screen_cursor();

short int vb_menu_load_world_select_screen();
short int vb_menu_load_world_select_screen_cursor();

short int vb_menu_load_room_select_screen();
short int vb_menu_load_room_select_screen_cursor();

short int vb_menu_load_story_select_screen();
short int vb_menu_load_story_select_screen_cursor();

short int vb_menu_load_story_scene_screen();

short int vb_menu_load_options_screen();
short int vb_menu_load_options_screen_cursor();

short int vb_menu_load_help_screen();
short int vb_menu_load_help_screen_cursor();

short int vb_menu_load_credits_screen();

short int vb_menu_load_room_intro_panel();

short int vb_menu_load_room_complete_panel();

short int vb_menu_load_pause_panel();
short int vb_menu_load_pause_panel_cursor();

short int vb_menu_load_retry_panel();
short int vb_menu_load_retry_panel_cursor();

short int vb_menu_close_panel();

#endif
