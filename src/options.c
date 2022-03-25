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
      (type[3] != 'I'))
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

  /* volume */
  if (options_data[OPTIONS_ENTRY_VOLUME] == 0)
    doremi_set_volume(0);
  else if (options_data[OPTIONS_ENTRY_VOLUME] == 1)
    doremi_set_volume(1);
  else if (options_data[OPTIONS_ENTRY_VOLUME] == 2)
    doremi_set_volume(2);
  else if (options_data[OPTIONS_ENTRY_VOLUME] == 3)
    doremi_set_volume(3);
  else if (options_data[OPTIONS_ENTRY_VOLUME] == 4)
    doremi_set_volume(4);
  else if (options_data[OPTIONS_ENTRY_VOLUME] == 5)
    doremi_set_volume(5);
  else if (options_data[OPTIONS_ENTRY_VOLUME] == 6)
    doremi_set_volume(6);
  else if (options_data[OPTIONS_ENTRY_VOLUME] == 7)
    doremi_set_volume(7);
  else if (options_data[OPTIONS_ENTRY_VOLUME] == 8)
    doremi_set_volume(8);

  /* button layout */
  if (options_data[OPTIONS_ENTRY_BUTTON_LAYOUT] == 0)
    controls_set_button_layout(CONTROLS_BUTTON_LAYOUT_BA);
  else if (options_data[OPTIONS_ENTRY_BUTTON_LAYOUT] == 1)
    controls_set_button_layout(CONTROLS_BUTTON_LAYOUT_SPLIT_YB_BA);
  else if (options_data[OPTIONS_ENTRY_BUTTON_LAYOUT] == 2)
    controls_set_button_layout(CONTROLS_BUTTON_LAYOUT_YB);

  /* upscaling mode */
  if (options_data[OPTIONS_ENTRY_UPSCALING_MODE] == 0)
    video_set_upscaling_mode(VIDEO_UPSCALING_MODE_LINEAR);
  else if (options_data[OPTIONS_ENTRY_UPSCALING_MODE] == 1)
    video_set_upscaling_mode(VIDEO_UPSCALING_MODE_TV);

  /* brightness */
  if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 0)
    video_set_black_level(0);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 1)
    video_set_black_level(1);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 2)
    video_set_black_level(2);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 3)
    video_set_black_level(3);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 4)
    video_set_black_level(4);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 5)
    video_set_black_level(5);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 6)
    video_set_black_level(6);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 7)
    video_set_black_level(7);
  else if (options_data[OPTIONS_ENTRY_BRIGHTNESS] == 8)
    video_set_black_level(8);

  /* contrast */
  if (options_data[OPTIONS_ENTRY_CONTRAST] == 0)
    video_set_white_level(0);
  else if (options_data[OPTIONS_ENTRY_CONTRAST] == 1)
    video_set_white_level(1);
  else if (options_data[OPTIONS_ENTRY_CONTRAST] == 2)
    video_set_white_level(2);
  else if (options_data[OPTIONS_ENTRY_CONTRAST] == 3)
    video_set_white_level(3);
  else if (options_data[OPTIONS_ENTRY_CONTRAST] == 4)
    video_set_white_level(4);
  else if (options_data[OPTIONS_ENTRY_CONTRAST] == 5)
    video_set_white_level(5);
  else if (options_data[OPTIONS_ENTRY_CONTRAST] == 6)
    video_set_white_level(6);
  else if (options_data[OPTIONS_ENTRY_CONTRAST] == 7)
    video_set_white_level(7);
  else if (options_data[OPTIONS_ENTRY_CONTRAST] == 8)
    video_set_white_level(8);

  /* tint */
  if (options_data[OPTIONS_ENTRY_TINT] == 0)
    video_set_hue(0);
  else if (options_data[OPTIONS_ENTRY_TINT] == 1)
    video_set_hue(1);
  else if (options_data[OPTIONS_ENTRY_TINT] == 2)
    video_set_hue(2);
  else if (options_data[OPTIONS_ENTRY_TINT] == 3)
    video_set_hue(3);
  else if (options_data[OPTIONS_ENTRY_TINT] == 4)
    video_set_hue(4);
  else if (options_data[OPTIONS_ENTRY_TINT] == 5)
    video_set_hue(5);
  else if (options_data[OPTIONS_ENTRY_TINT] == 6)
    video_set_hue(6);
  else if (options_data[OPTIONS_ENTRY_TINT] == 7)
    video_set_hue(7);
  else if (options_data[OPTIONS_ENTRY_TINT] == 8)
    video_set_hue(8);

  /* saturation */
  if (options_data[OPTIONS_ENTRY_SATURATION] == 0)
    video_set_saturation(0);
  else if (options_data[OPTIONS_ENTRY_SATURATION] == 1)
    video_set_saturation(1);
  else if (options_data[OPTIONS_ENTRY_SATURATION] == 2)
    video_set_saturation(2);
  else if (options_data[OPTIONS_ENTRY_SATURATION] == 3)
    video_set_saturation(3);
  else if (options_data[OPTIONS_ENTRY_SATURATION] == 4)
    video_set_saturation(4);
  else if (options_data[OPTIONS_ENTRY_SATURATION] == 5)
    video_set_saturation(5);
  else if (options_data[OPTIONS_ENTRY_SATURATION] == 6)
    video_set_saturation(6);
  else if (options_data[OPTIONS_ENTRY_SATURATION] == 7)
    video_set_saturation(7);
  else if (options_data[OPTIONS_ENTRY_SATURATION] == 8)
    video_set_saturation(8);

  /* gamma */
  if (options_data[OPTIONS_ENTRY_GAMMA] == 0)
    video_set_gamma(0);
  else if (options_data[OPTIONS_ENTRY_GAMMA] == 1)
    video_set_gamma(1);
  else if (options_data[OPTIONS_ENTRY_GAMMA] == 2)
    video_set_gamma(2);
  else if (options_data[OPTIONS_ENTRY_GAMMA] == 3)
    video_set_gamma(3);
  else if (options_data[OPTIONS_ENTRY_GAMMA] == 4)
    video_set_gamma(4);
  else if (options_data[OPTIONS_ENTRY_GAMMA] == 5)
    video_set_gamma(5);
  else if (options_data[OPTIONS_ENTRY_GAMMA] == 6)
    video_set_gamma(6);
  else if (options_data[OPTIONS_ENTRY_GAMMA] == 7)
    video_set_gamma(7);
  else if (options_data[OPTIONS_ENTRY_GAMMA] == 8)
    video_set_gamma(8);

  if (G_upscaling_mode == VIDEO_UPSCALING_MODE_TV)
  {
    /* blur amount */
    if (options_data[OPTIONS_ENTRY_BLUR_AMOUNT] == 0)
      video_set_blur_filter_sigma(0);
    else if (options_data[OPTIONS_ENTRY_BLUR_AMOUNT] == 1)
      video_set_blur_filter_sigma(1);
    else if (options_data[OPTIONS_ENTRY_BLUR_AMOUNT] == 2)
      video_set_blur_filter_sigma(2);
    else if (options_data[OPTIONS_ENTRY_BLUR_AMOUNT] == 3)
      video_set_blur_filter_sigma(3);
    else if (options_data[OPTIONS_ENTRY_BLUR_AMOUNT] == 4)
      video_set_blur_filter_sigma(4);

    /* phosphor mask */
    if (options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] == 0)
      video_set_mask_opacity(0);
    else if (options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] == 1)
      video_set_mask_opacity(1);
    else if (options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] == 2)
      video_set_mask_opacity(2);
    else if (options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] == 3)
      video_set_mask_opacity(3);
    else if (options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] == 4)
      video_set_mask_opacity(4);
  }

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

  /* volume */
  if (G_sound_volume == 0)
    options_data[OPTIONS_ENTRY_VOLUME] = 0;
  else if (G_sound_volume == 16)
    options_data[OPTIONS_ENTRY_VOLUME] = 1;
  else if (G_sound_volume == 32)
    options_data[OPTIONS_ENTRY_VOLUME] = 2;
  else if (G_sound_volume == 48)
    options_data[OPTIONS_ENTRY_VOLUME] = 3;
  else if (G_sound_volume == 64)
    options_data[OPTIONS_ENTRY_VOLUME] = 4;
  else if (G_sound_volume == 80)
    options_data[OPTIONS_ENTRY_VOLUME] = 5;
  else if (G_sound_volume == 96)
    options_data[OPTIONS_ENTRY_VOLUME] = 6;
  else if (G_sound_volume == 112)
    options_data[OPTIONS_ENTRY_VOLUME] = 7;
  else if (G_sound_volume == 128)
    options_data[OPTIONS_ENTRY_VOLUME] = 8;
  else
    options_data[OPTIONS_ENTRY_VOLUME] = 8;

  /* button layout */
  if (G_button_layout == CONTROLS_BUTTON_LAYOUT_BA)
    options_data[OPTIONS_ENTRY_BUTTON_LAYOUT] = 0;
  else if (G_button_layout == CONTROLS_BUTTON_LAYOUT_SPLIT_YB_BA)
    options_data[OPTIONS_ENTRY_BUTTON_LAYOUT] = 1;
  else if (G_button_layout == CONTROLS_BUTTON_LAYOUT_YB)
    options_data[OPTIONS_ENTRY_BUTTON_LAYOUT] = 2;
  else
    options_data[OPTIONS_ENTRY_BUTTON_LAYOUT] = 0;

  /* upscaling mode */
  if (G_upscaling_mode == VIDEO_UPSCALING_MODE_LINEAR)
    options_data[OPTIONS_ENTRY_UPSCALING_MODE] = 0;
  else if (G_upscaling_mode == VIDEO_UPSCALING_MODE_TV)
    options_data[OPTIONS_ENTRY_UPSCALING_MODE] = 1;
  else
    options_data[OPTIONS_ENTRY_UPSCALING_MODE] = 0;

  /* brightness */
  if (G_black_level_index == 0)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 0;
  else if (G_black_level_index == 1)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 1;
  else if (G_black_level_index == 2)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 2;
  else if (G_black_level_index == 3)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 3;
  else if (G_black_level_index == 4)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 4;
  else if (G_black_level_index == 5)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 5;
  else if (G_black_level_index == 6)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 6;
  else if (G_black_level_index == 7)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 7;
  else if (G_black_level_index == 8)
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 8;
  else
    options_data[OPTIONS_ENTRY_BRIGHTNESS] = 0;

  /* contrast */
  if (G_white_level_index == 0)
    options_data[OPTIONS_ENTRY_CONTRAST] = 0;
  else if (G_white_level_index == 1)
    options_data[OPTIONS_ENTRY_CONTRAST] = 1;
  else if (G_white_level_index == 2)
    options_data[OPTIONS_ENTRY_CONTRAST] = 2;
  else if (G_white_level_index == 3)
    options_data[OPTIONS_ENTRY_CONTRAST] = 3;
  else if (G_white_level_index == 4)
    options_data[OPTIONS_ENTRY_CONTRAST] = 4;
  else if (G_white_level_index == 5)
    options_data[OPTIONS_ENTRY_CONTRAST] = 5;
  else if (G_white_level_index == 6)
    options_data[OPTIONS_ENTRY_CONTRAST] = 6;
  else if (G_white_level_index == 7)
    options_data[OPTIONS_ENTRY_CONTRAST] = 7;
  else if (G_white_level_index == 8)
    options_data[OPTIONS_ENTRY_CONTRAST] = 8;
  else
    options_data[OPTIONS_ENTRY_CONTRAST] = 0;

  /* tint */
  if (G_hue_index == 0)
    options_data[OPTIONS_ENTRY_TINT] = 0;
  else if (G_hue_index == 1)
    options_data[OPTIONS_ENTRY_TINT] = 1;
  else if (G_hue_index == 2)
    options_data[OPTIONS_ENTRY_TINT] = 2;
  else if (G_hue_index == 3)
    options_data[OPTIONS_ENTRY_TINT] = 3;
  else if (G_hue_index == 4)
    options_data[OPTIONS_ENTRY_TINT] = 4;
  else if (G_hue_index == 5)
    options_data[OPTIONS_ENTRY_TINT] = 5;
  else if (G_hue_index == 6)
    options_data[OPTIONS_ENTRY_TINT] = 6;
  else if (G_hue_index == 7)
    options_data[OPTIONS_ENTRY_TINT] = 7;
  else if (G_hue_index == 8)
    options_data[OPTIONS_ENTRY_TINT] = 8;
  else
    options_data[OPTIONS_ENTRY_TINT] = 0;

  /* saturation */
  if (G_saturation_index == 0)
    options_data[OPTIONS_ENTRY_SATURATION] = 0;
  else if (G_saturation_index == 1)
    options_data[OPTIONS_ENTRY_SATURATION] = 1;
  else if (G_saturation_index == 2)
    options_data[OPTIONS_ENTRY_SATURATION] = 2;
  else if (G_saturation_index == 3)
    options_data[OPTIONS_ENTRY_SATURATION] = 3;
  else if (G_saturation_index == 4)
    options_data[OPTIONS_ENTRY_SATURATION] = 4;
  else if (G_saturation_index == 5)
    options_data[OPTIONS_ENTRY_SATURATION] = 5;
  else if (G_saturation_index == 6)
    options_data[OPTIONS_ENTRY_SATURATION] = 6;
  else if (G_saturation_index == 7)
    options_data[OPTIONS_ENTRY_SATURATION] = 7;
  else if (G_saturation_index == 8)
    options_data[OPTIONS_ENTRY_SATURATION] = 8;
  else
    options_data[OPTIONS_ENTRY_SATURATION] = 0;

  /* gamma */
  if (G_gamma_index == 0)
    options_data[OPTIONS_ENTRY_GAMMA] = 0;
  else if (G_gamma_index == 1)
    options_data[OPTIONS_ENTRY_GAMMA] = 1;
  else if (G_gamma_index == 2)
    options_data[OPTIONS_ENTRY_GAMMA] = 2;
  else if (G_gamma_index == 3)
    options_data[OPTIONS_ENTRY_GAMMA] = 3;
  else if (G_gamma_index == 4)
    options_data[OPTIONS_ENTRY_GAMMA] = 4;
  else if (G_gamma_index == 5)
    options_data[OPTIONS_ENTRY_GAMMA] = 5;
  else if (G_gamma_index == 6)
    options_data[OPTIONS_ENTRY_GAMMA] = 6;
  else if (G_gamma_index == 7)
    options_data[OPTIONS_ENTRY_GAMMA] = 7;
  else if (G_gamma_index == 8)
    options_data[OPTIONS_ENTRY_GAMMA] = 8;
  else
    options_data[OPTIONS_ENTRY_GAMMA] = 0;

  if (G_upscaling_mode == VIDEO_UPSCALING_MODE_TV)
  {
    /* blur amount */
    if (G_blur_filter_sigma_index == 0)
      options_data[OPTIONS_ENTRY_BLUR_AMOUNT] = 0;
    else if (G_blur_filter_sigma_index == 1)
      options_data[OPTIONS_ENTRY_BLUR_AMOUNT] = 1;
    else if (G_blur_filter_sigma_index == 2)
      options_data[OPTIONS_ENTRY_BLUR_AMOUNT] = 2;
    else if (G_blur_filter_sigma_index == 3)
      options_data[OPTIONS_ENTRY_BLUR_AMOUNT] = 3;
    else if (G_blur_filter_sigma_index == 4)
      options_data[OPTIONS_ENTRY_BLUR_AMOUNT] = 4;
    else
      options_data[OPTIONS_ENTRY_BLUR_AMOUNT] = 0;

    /* phosphor mask */
    if (G_mask_opacity_index == 0)
      options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] = 0;
    else if (G_mask_opacity_index == 1)
      options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] = 1;
    else if (G_mask_opacity_index == 2)
      options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] = 2;
    else if (G_mask_opacity_index == 3)
      options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] = 3;
    else if (G_mask_opacity_index == 4)
      options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] = 4;
    else
      options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] = 0;
  }
  else
  {
    options_data[OPTIONS_ENTRY_BLUR_AMOUNT] = 0;
    options_data[OPTIONS_ENTRY_PHOSPHOR_MASK] = 0;
  }

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
  type[3] = 'I';

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

