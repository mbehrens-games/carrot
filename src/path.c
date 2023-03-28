/*******************************************************************************
** path.c (path variables)
*******************************************************************************/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"

char* G_path_base;
char* G_path_gfx_data;
char* G_path_sfx_data;
char* G_path_level_data;
char* G_path_story_data;
char* G_path_shader_data;
char* G_path_ost;
char* G_path_pref;
char* G_path_save_game_1;
char* G_path_save_game_2;
char* G_path_save_game_3;
char* G_path_options;

/*******************************************************************************
** path_init_paths()
*******************************************************************************/
short int path_init_paths()
{
  G_path_base = NULL;
  G_path_gfx_data = NULL;
  G_path_sfx_data = NULL;
  G_path_level_data = NULL;
  G_path_story_data = NULL;
  G_path_shader_data = NULL;
  G_path_ost = NULL;
  G_path_pref = NULL;
  G_path_save_game_1 = NULL;
  G_path_save_game_2 = NULL;
  G_path_save_game_3 = NULL;
  G_path_options = NULL;

  return 0;
}

/*******************************************************************************
** path_obtain_base_paths()
*******************************************************************************/
short int path_obtain_base_paths()
{
  G_path_base = SDL_GetBasePath();

  if (G_path_base == NULL)
  {
    return 1;
  }

  G_path_gfx_data = malloc(sizeof(char) * (strlen(G_path_base) + 12 + 1));
  strcpy(G_path_gfx_data, G_path_base);
  strcpy(G_path_gfx_data + strlen(G_path_base), "carrgrph.dat");

  G_path_sfx_data = malloc(sizeof(char) * (strlen(G_path_base) + 12 + 1));
  strcpy(G_path_sfx_data, G_path_base);
  strcpy(G_path_sfx_data + strlen(G_path_base), "carrsoun.dat");

  G_path_level_data = malloc(sizeof(char) * (strlen(G_path_base) + 12 + 1));
  strcpy(G_path_level_data, G_path_base);
  strcpy(G_path_level_data + strlen(G_path_base), "carrlvls.dat");

  G_path_story_data = malloc(sizeof(char) * (strlen(G_path_base) + 12 + 1));
  strcpy(G_path_story_data, G_path_base);
  strcpy(G_path_story_data + strlen(G_path_base), "carrstry.dat");

  G_path_shader_data = malloc(sizeof(char) * (strlen(G_path_base) + 12 + 1));
  strcpy(G_path_shader_data, G_path_base);
  strcpy(G_path_shader_data + strlen(G_path_base), "shader1b.dat");

  G_path_ost = malloc(sizeof(char) * (strlen(G_path_base) + 3 + 1));
  strcpy(G_path_ost, G_path_base);
  strcpy(G_path_ost + strlen(G_path_base), "xm");
  G_path_ost[strlen(G_path_base) + 2] = G_path_ost[strlen(G_path_base) - 1];
  G_path_ost[strlen(G_path_base) + 3] = '\0';

  return 0;
}

/*******************************************************************************
** path_obtain_preferences_path()
*******************************************************************************/
short int path_obtain_preferences_path()
{
  G_path_pref = SDL_GetPrefPath("Michael Behrens", "The Year of the Carrot");

  if (G_path_pref == NULL)
  {
    return 1;
  }

  G_path_save_game_1 = malloc(sizeof(char) * (strlen(G_path_pref) + 6 + 1));
  strcpy(G_path_save_game_1, G_path_pref);
  strcpy(G_path_save_game_1 + strlen(G_path_pref), "01.sav");

  G_path_save_game_2 = malloc(sizeof(char) * (strlen(G_path_pref) + 6 + 1));
  strcpy(G_path_save_game_2, G_path_pref);
  strcpy(G_path_save_game_2 + strlen(G_path_pref), "02.sav");

  G_path_save_game_3 = malloc(sizeof(char) * (strlen(G_path_pref) + 6 + 1));
  strcpy(G_path_save_game_3, G_path_pref);
  strcpy(G_path_save_game_3 + strlen(G_path_pref), "03.sav");

  G_path_options = malloc(sizeof(char) * (strlen(G_path_pref) + 6 + 1));
  strcpy(G_path_options, G_path_pref);
  strcpy(G_path_options + strlen(G_path_pref), "options.sav");

  return 0;
}

/*******************************************************************************
** path_free_paths()
*******************************************************************************/
short int path_free_paths()
{
  if (G_path_base != NULL)
  {
    SDL_free(G_path_base);
    G_path_base = NULL;
  }

  if (G_path_gfx_data != NULL)
  {
    free(G_path_gfx_data);
    G_path_gfx_data = NULL;
  }

  if (G_path_sfx_data != NULL)
  {
    free(G_path_sfx_data);
    G_path_sfx_data = NULL;
  }

  if (G_path_level_data != NULL)
  {
    free(G_path_level_data);
    G_path_level_data = NULL;
  }

  if (G_path_story_data != NULL)
  {
    free(G_path_story_data);
    G_path_story_data = NULL;
  }

  if (G_path_shader_data != NULL)
  {
    free(G_path_shader_data);
    G_path_shader_data = NULL;
  }

  if (G_path_ost != NULL)
  {
    free(G_path_ost);
    G_path_ost = NULL;
  }

  if (G_path_pref != NULL)
  {
    SDL_free(G_path_pref);
    G_path_pref = NULL;
  }

  if (G_path_save_game_1 != NULL)
  {
    free(G_path_save_game_1);
    G_path_save_game_1 = NULL;
  }

  if (G_path_save_game_2 != NULL)
  {
    free(G_path_save_game_2);
    G_path_save_game_2 = NULL;
  }

  if (G_path_save_game_3 != NULL)
  {
    free(G_path_save_game_3);
    G_path_save_game_3 = NULL;
  }

  if (G_path_options != NULL)
  {
    free(G_path_options);
    G_path_options = NULL;
  }

  return 0;
}

