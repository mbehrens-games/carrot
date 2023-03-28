/*******************************************************************************
** options.c (options)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "controls.h"
#include "doremi.h"
#include "global.h"
#include "graphics.h"
#include "options.h"
#include "path.h"
#include "video.h"

/*******************************************************************************
** options_load()
*******************************************************************************/
short int options_load()
{
  FILE* fp;
  char  signature[4];
  char  type[4];

  unsigned char options_data[OPTIONS_NUM_ENTRIES];

  /* open options file */
  fp = fopen(G_path_options, "rb");

  /* if file did not open, return */
  if (fp == NULL)
    return 0;

  /* read signature */
  if (fread(signature, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if ((signature[0] != 'C') || 
      (signature[1] != 'A') || 
      (signature[2] != 'R') || 
      (signature[3] != 'R'))
  {
    fclose(fp);
    return 1;
  }

  /* read type */
  if (fread(type, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if ((type[0] != 'O') || 
      (type[1] != 'P') || 
      (type[2] != 'T') || 
      (type[3] != 'N'))
  {
    fclose(fp);
    return 1;
  }

  /* read options data */
  if (fread(options_data, 1, OPTIONS_NUM_ENTRIES, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close options file */
  fclose(fp);

  /* apply options data */

  /* window size */
  if (options_data[OPTIONS_ENTRY_WINDOW_SIZE] == 0)
    graphics_set_window_size(GRAPHICS_RESOLUTION_480P);
  else if (options_data[OPTIONS_ENTRY_WINDOW_SIZE] == 1)
    graphics_set_window_size(GRAPHICS_RESOLUTION_720P);
  else if (options_data[OPTIONS_ENTRY_WINDOW_SIZE] == 2)
    graphics_set_window_size(GRAPHICS_RESOLUTION_768P);
  else if (options_data[OPTIONS_ENTRY_WINDOW_SIZE] == 3)
    graphics_set_window_size(GRAPHICS_RESOLUTION_1080P);

  /* fullscreen */
  if (options_data[OPTIONS_ENTRY_FULLSCREEN] == 0)
    graphics_initialize_fullscreen(0);
  else if (options_data[OPTIONS_ENTRY_FULLSCREEN] == 1)
    graphics_initialize_fullscreen(1);

  /* vsync */
  if (options_data[OPTIONS_ENTRY_VSYNC] == 0)
    graphics_initialize_vsync(0);
  else if (options_data[OPTIONS_ENTRY_VSYNC] == 1)
    graphics_initialize_vsync(1);

  /* upscaling mode */
  if (options_data[OPTIONS_ENTRY_UPSCALING_MODE] == 0)
    video_set_upscaling_mode(VIDEO_UPSCALING_MODE_LINEAR);
  else if (options_data[OPTIONS_ENTRY_UPSCALING_MODE] == 1)
    video_set_upscaling_mode(VIDEO_UPSCALING_MODE_PIXELS);
  else if (options_data[OPTIONS_ENTRY_UPSCALING_MODE] == 2)
    video_set_upscaling_mode(VIDEO_UPSCALING_MODE_SCANLINES);

  /* brightness */
  if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 0)
    video_set_brightness(0);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 1)
    video_set_brightness(1);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 2)
    video_set_brightness(2);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 3)
    video_set_brightness(3);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 4)
    video_set_brightness(4);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 5)
    video_set_brightness(5);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 6)
    video_set_brightness(6);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 7)
    video_set_brightness(7);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 8)
    video_set_brightness(8);

  /* sound volume */
  if (options_data[OPTIONS_ENTRY_SOUND_VOLUME] == 0)
    doremi_set_volume(0);
  else if (options_data[OPTIONS_ENTRY_SOUND_VOLUME] == 1)
    doremi_set_volume(1);
  else if (options_data[OPTIONS_ENTRY_SOUND_VOLUME] == 2)
    doremi_set_volume(2);
  else if (options_data[OPTIONS_ENTRY_SOUND_VOLUME] == 3)
    doremi_set_volume(3);
  else if (options_data[OPTIONS_ENTRY_SOUND_VOLUME] == 4)
    doremi_set_volume(4);
  else if (options_data[OPTIONS_ENTRY_SOUND_VOLUME] == 5)
    doremi_set_volume(5);
  else if (options_data[OPTIONS_ENTRY_SOUND_VOLUME] == 6)
    doremi_set_volume(6);
  else if (options_data[OPTIONS_ENTRY_SOUND_VOLUME] == 7)
    doremi_set_volume(7);
  else if (options_data[OPTIONS_ENTRY_SOUND_VOLUME] == 8)
    doremi_set_volume(8);

  /* game button layout */
  if (options_data[OPTIONS_ENTRY_GAME_BUTTON_LAYOUT] == 0)
    controls_set_game_button_layout(CONTROLS_GAME_BUTTON_LAYOUT_BA);
  else if (options_data[OPTIONS_ENTRY_GAME_BUTTON_LAYOUT] == 1)
    controls_set_game_button_layout(CONTROLS_GAME_BUTTON_LAYOUT_YB);

  /* menu button layout */
  if (options_data[OPTIONS_ENTRY_MENU_BUTTON_LAYOUT] == 0)
    controls_set_menu_button_layout(CONTROLS_MENU_BUTTON_LAYOUT_BA);
  else if (options_data[OPTIONS_ENTRY_MENU_BUTTON_LAYOUT] == 1)
    controls_set_menu_button_layout(CONTROLS_MENU_BUTTON_LAYOUT_AB);

  /* menu key layout */
  if (options_data[OPTIONS_ENTRY_MENU_KEY_LAYOUT] == 0)
    controls_set_menu_key_layout(CONTROLS_MENU_KEY_LAYOUT_ZX);
  else if (options_data[OPTIONS_ENTRY_MENU_KEY_LAYOUT] == 1)
    controls_set_menu_key_layout(CONTROLS_MENU_KEY_LAYOUT_XZ);

  return 0;
}

/*******************************************************************************
** options_save()
*******************************************************************************/
short int options_save()
{
  int i;

  FILE* fp;
  char  signature[4];
  char  type[4];

  unsigned char options_data[OPTIONS_NUM_ENTRIES];

  /* initialize options data */
  for (i = 0; i < OPTIONS_NUM_ENTRIES; i++)
    options_data[i] = 0;

  /* generate options data */

  /* window size */
  if (G_graphics_resolution == GRAPHICS_RESOLUTION_480P)
    options_data[OPTIONS_ENTRY_WINDOW_SIZE] = 0;
  else if (G_graphics_resolution == GRAPHICS_RESOLUTION_720P)
    options_data[OPTIONS_ENTRY_WINDOW_SIZE] = 1;
  else if (G_graphics_resolution == GRAPHICS_RESOLUTION_768P)
    options_data[OPTIONS_ENTRY_WINDOW_SIZE] = 2;
  else if (G_graphics_resolution == GRAPHICS_RESOLUTION_1080P)
    options_data[OPTIONS_ENTRY_WINDOW_SIZE] = 3;
  else
    options_data[OPTIONS_ENTRY_WINDOW_SIZE] = 0;

  /* fullscreen */
  if (G_flag_window_fullscreen == 0)
    options_data[OPTIONS_ENTRY_FULLSCREEN] = 0;
  else if (G_flag_window_fullscreen == 1)
    options_data[OPTIONS_ENTRY_FULLSCREEN] = 1;
  else
    options_data[OPTIONS_ENTRY_FULLSCREEN] = 0;

  /* vsync */
  if (G_flag_window_vsync == 0)
    options_data[OPTIONS_ENTRY_VSYNC] = 0;
  else if (G_flag_window_vsync == 1)
    options_data[OPTIONS_ENTRY_VSYNC] = 1;
  else
    options_data[OPTIONS_ENTRY_VSYNC] = 0;

  /* brightness */
  if (G_brightness_index == 0)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 0;
  else if (G_brightness_index == 1)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 1;
  else if (G_brightness_index == 2)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 2;
  else if (G_brightness_index == 3)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 3;
  else if (G_brightness_index == 4)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 4;
  else if (G_brightness_index == 5)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 5;
  else if (G_brightness_index == 6)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 6;
  else if (G_brightness_index == 7)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 7;
  else if (G_brightness_index == 8)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 8;
  else
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 0;

  /* upscaling mode */
  if (G_upscaling_mode == VIDEO_UPSCALING_MODE_LINEAR)
    options_data[OPTIONS_ENTRY_UPSCALING_MODE] = 0;
  else if (G_upscaling_mode == VIDEO_UPSCALING_MODE_PIXELS)
    options_data[OPTIONS_ENTRY_UPSCALING_MODE] = 1;
  else if (G_upscaling_mode == VIDEO_UPSCALING_MODE_SCANLINES)
    options_data[OPTIONS_ENTRY_UPSCALING_MODE] = 2;
  else
    options_data[OPTIONS_ENTRY_UPSCALING_MODE] = 0;

  /* sound volume */
  if (G_sound_volume == 0)
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 0;
  else if (G_sound_volume == 16)
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 1;
  else if (G_sound_volume == 32)
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 2;
  else if (G_sound_volume == 48)
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 3;
  else if (G_sound_volume == 64)
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 4;
  else if (G_sound_volume == 80)
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 5;
  else if (G_sound_volume == 96)
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 6;
  else if (G_sound_volume == 112)
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 7;
  else if (G_sound_volume == 128)
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 8;
  else
    options_data[OPTIONS_ENTRY_SOUND_VOLUME] = 8;

  /* game button layout */
  if (G_game_button_layout == CONTROLS_GAME_BUTTON_LAYOUT_BA)
    options_data[OPTIONS_ENTRY_GAME_BUTTON_LAYOUT] = 0;
  else if (G_game_button_layout == CONTROLS_GAME_BUTTON_LAYOUT_YB)
    options_data[OPTIONS_ENTRY_GAME_BUTTON_LAYOUT] = 1;
  else
    options_data[OPTIONS_ENTRY_GAME_BUTTON_LAYOUT] = 0;

  /* menu button layout */
  if (G_menu_button_layout == CONTROLS_MENU_BUTTON_LAYOUT_BA)
    options_data[OPTIONS_ENTRY_MENU_BUTTON_LAYOUT] = 0;
  else if (G_menu_button_layout == CONTROLS_MENU_BUTTON_LAYOUT_AB)
    options_data[OPTIONS_ENTRY_MENU_BUTTON_LAYOUT] = 1;
  else
    options_data[OPTIONS_ENTRY_MENU_BUTTON_LAYOUT] = 0;

  /* menu key layout */
  if (G_menu_key_layout == CONTROLS_MENU_KEY_LAYOUT_ZX)
    options_data[OPTIONS_ENTRY_MENU_KEY_LAYOUT] = 0;
  else if (G_menu_key_layout == CONTROLS_MENU_KEY_LAYOUT_XZ)
    options_data[OPTIONS_ENTRY_MENU_KEY_LAYOUT] = 1;
  else
    options_data[OPTIONS_ENTRY_MENU_KEY_LAYOUT] = 0;

  /* open options file */
  fp = fopen(G_path_options, "wb");

  /* if file did not open, return */
  if (fp == NULL)
    return 0;

  /* write signature */
  signature[0] = 'C';
  signature[1] = 'A';
  signature[2] = 'R';
  signature[3] = 'R';

  if (fwrite(signature, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write type */
  type[0] = 'O';
  type[1] = 'P';
  type[2] = 'T';
  type[3] = 'N';

  if (fwrite(type, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write options data */
  if (fwrite(options_data, 1, OPTIONS_NUM_ENTRIES, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close options file */
  fclose(fp);

  return 0;
}
