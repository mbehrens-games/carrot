/*******************************************************************************
** path.h (path variables)
*******************************************************************************/

#ifndef PATH_H
#define PATH_H

extern char*  G_path_base;
extern char*  G_path_gfx_data;
extern char*  G_path_sfx_data;
extern char*  G_path_level_data;
extern char*  G_path_story_data;
extern char*  G_path_shader_data;
extern char*  G_path_ost;
extern char*  G_path_pref;
extern char*  G_path_save_game_1;
extern char*  G_path_save_game_2;
extern char*  G_path_save_game_3;
extern char*  G_path_options;

/* function declarations */
short int path_init_paths();
short int path_obtain_base_paths();
short int path_obtain_preferences_path();
short int path_free_paths();

#endif
