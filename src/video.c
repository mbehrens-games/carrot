/*******************************************************************************
** video.c (video options)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "doremi.h"
#include "video.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define BLACK_LEVEL_STEP  0.015625f /* 1/64 */
#define WHITE_LEVEL_STEP  0.015625f /* 1/64 */

#define HUE_STEP    (TWO_PI / 144)  /* 2.5 degrees */

#define SATURATION_STEP   0.125f  /* 1/8 */

#define GAMMA_STEP        0.05f
#define NTSC_GAMMA        2.2f

#define BLUR_FILTER_SIGMA_BASE  0.375f  /* 3/8  */
#define BLUR_FILTER_SIGMA_STEP  0.0625f /* 1/16 */

#define MASK_OPACITY_STEP     0.125f  /* 1/8 */

#define BLACK_LEVEL_TABLE_NUM_VALUES  9
#define WHITE_LEVEL_TABLE_NUM_VALUES  9
#define HUE_TABLE_NUM_VALUES    9
#define SATURATION_TABLE_NUM_VALUES   9
#define GAMMA_TABLE_NUM_VALUES        9

#define BLUR_FILTER_SIGMA_TABLE_NUM_VALUES  5
#define MASK_OPACITY_TABLE_NUM_VALUES       5

/* settings tables */
GLfloat G_black_level_table[9] = 
          { 0.0f,
            1 * BLACK_LEVEL_STEP,
            2 * BLACK_LEVEL_STEP,
            3 * BLACK_LEVEL_STEP,
            4 * BLACK_LEVEL_STEP,
            5 * BLACK_LEVEL_STEP,
            6 * BLACK_LEVEL_STEP,
            7 * BLACK_LEVEL_STEP,
            8 * BLACK_LEVEL_STEP};

GLfloat G_white_level_table[9] = 
          { 1.0f - 8 * WHITE_LEVEL_STEP,
            1.0f - 7 * WHITE_LEVEL_STEP,
            1.0f - 6 * WHITE_LEVEL_STEP,
            1.0f - 5 * WHITE_LEVEL_STEP,
            1.0f - 4 * WHITE_LEVEL_STEP,
            1.0f - 3 * WHITE_LEVEL_STEP,
            1.0f - 2 * WHITE_LEVEL_STEP,
            1.0f - 1 * WHITE_LEVEL_STEP,
            1.0f};

GLfloat G_hue_table[9] = 
          { -4.0f * HUE_STEP,
            -3.0f * HUE_STEP,
            -2.0f * HUE_STEP,
            -1.0f * HUE_STEP,
             0.0f,
             1.0f * HUE_STEP,
             2.0f * HUE_STEP,
             3.0f * HUE_STEP,
             4.0f * HUE_STEP};

GLfloat G_saturation_table[9] = 
          { 1.0f - 4 * SATURATION_STEP,
            1.0f - 3 * SATURATION_STEP,
            1.0f - 2 * SATURATION_STEP,
            1.0f - 1 * SATURATION_STEP,
            1.0f,
            1.0f + 1 * SATURATION_STEP,
            1.0f + 2 * SATURATION_STEP,
            1.0f + 3 * SATURATION_STEP,
            1.0f + 4 * SATURATION_STEP};

GLfloat G_gamma_table[9] = 
          { NTSC_GAMMA,
            NTSC_GAMMA + 1 * GAMMA_STEP,
            NTSC_GAMMA + 2 * GAMMA_STEP,
            NTSC_GAMMA + 3 * GAMMA_STEP,
            NTSC_GAMMA + 4 * GAMMA_STEP,
            NTSC_GAMMA + 5 * GAMMA_STEP,
            NTSC_GAMMA + 6 * GAMMA_STEP,
            NTSC_GAMMA + 7 * GAMMA_STEP,
            NTSC_GAMMA + 8 * GAMMA_STEP};

GLfloat G_blur_filter_sigma_table[5] = 
          { BLUR_FILTER_SIGMA_BASE,
            BLUR_FILTER_SIGMA_BASE + 1 * BLUR_FILTER_SIGMA_STEP,
            BLUR_FILTER_SIGMA_BASE + 2 * BLUR_FILTER_SIGMA_STEP,
            BLUR_FILTER_SIGMA_BASE + 3 * BLUR_FILTER_SIGMA_STEP,
            BLUR_FILTER_SIGMA_BASE + 4 * BLUR_FILTER_SIGMA_STEP};

GLfloat G_mask_opacity_table[5] = 
          { 1.0f,
            1.0f - 1 * MASK_OPACITY_STEP,
            1.0f - 2 * MASK_OPACITY_STEP,
            1.0f - 3 * MASK_OPACITY_STEP,
            1.0f - 4 * MASK_OPACITY_STEP};

/* blur filter */
GLfloat G_blur_filter[BLUR_FILTER_BOUND];

/* cubic filter (mitchell netravali) */
GLfloat G_cubic_matrix[16] = 
          { -0.38888888888889f,  0.83333333333333f, -0.5f,  0.05555555555556f,
             1.16666666666667f, -2.0f,               0.0f,  0.88888888888889f,
            -1.16666666666667f,  1.5f,               0.5f,  0.05555555555556f,
             0.38888888888889f, -0.33333333333333f,  0.0f,  0.0f};

/* matrices */
GLfloat G_rgb2yiq_matrix[9] = 
          { 0.299f,  0.596f,  0.211f,
            0.587f, -0.274f, -0.523f,
            0.114f, -0.322f,  0.312f};
GLfloat G_yiq2rgb_matrix[9] = 
          { 1.000f,  1.000f,  1.000f,
            0.956f, -0.272f, -1.106f,
            0.619f, -0.647f,  1.703f};

/* settings variables */
int G_upscaling_mode;

GLfloat G_black_level;
GLfloat G_white_level;
GLfloat G_hue;
GLfloat G_saturation;
GLfloat G_gamma;
GLfloat G_blur_filter_sigma;
GLfloat G_mask_opacity;

int G_black_level_index;
int G_white_level_index;
int G_hue_index;
int G_saturation_index;
int G_gamma_index;
int G_blur_filter_sigma_index;
int G_mask_opacity_index;

/*******************************************************************************
** video_compute_blur_filter()
*******************************************************************************/
short int video_compute_blur_filter()
{
  int i;

  float sigma;
  float sum;

  /* blur filter */
  sigma = G_blur_filter_sigma;

  for (i = 0; i < BLUR_FILTER_BOUND; i++)
  {
    G_blur_filter[i] = exp(-((i - BLUR_FILTER_M / 2) * (i - BLUR_FILTER_M / 2)) / (2 * sigma * sigma));
  }

  /* normalization */
  sum = G_blur_filter[BLUR_FILTER_M / 2];

  for (i = 0; i < BLUR_FILTER_BOUND - 1; i++)
    sum += 2 * G_blur_filter[i];

  for (i = 0; i < BLUR_FILTER_BOUND; i++)
    G_blur_filter[i] /= sum;

  return 0;
}

/*******************************************************************************
** video_init_variables()
*******************************************************************************/
short int video_init_variables()
{
  G_upscaling_mode = VIDEO_UPSCALING_MODE_LINEAR;

  G_black_level_index = BLACK_LEVEL_TABLE_NUM_VALUES / 2;
  G_white_level_index = WHITE_LEVEL_TABLE_NUM_VALUES / 2;
  G_hue_index = HUE_TABLE_NUM_VALUES / 2;
  G_saturation_index = SATURATION_TABLE_NUM_VALUES / 2;
  G_gamma_index = GAMMA_TABLE_NUM_VALUES / 2;

  G_blur_filter_sigma_index = 0;
  G_mask_opacity_index = 0;

  G_black_level = G_black_level_table[G_black_level_index];
  G_white_level = G_white_level_table[G_white_level_index];
  G_hue = G_hue_table[G_hue_index];
  G_saturation = G_saturation_table[G_saturation_index];
  G_gamma = G_gamma_table[G_gamma_index];

  G_blur_filter_sigma = G_blur_filter_sigma_table[G_blur_filter_sigma_index];
  G_mask_opacity = G_mask_opacity_table[G_mask_opacity_index];

  video_compute_blur_filter();

  return 0;
}

/*******************************************************************************
** video_set_upscaling_mode()
*******************************************************************************/
short int video_set_upscaling_mode(int mode)
{
  if ((mode >= 0) && (mode < VIDEO_NUM_UPSCALING_MODES))
  {
    G_upscaling_mode = mode;
  }

  /* make sure blur filter and phosphor mask settings */
  /* are off if we are not on the TV video mode       */
  if (G_upscaling_mode != VIDEO_UPSCALING_MODE_TV)
  {
    G_blur_filter_sigma_index = 0;
    G_blur_filter_sigma = G_blur_filter_sigma_table[G_blur_filter_sigma_index];

    G_mask_opacity_index = 0;
    G_mask_opacity = G_mask_opacity_table[G_mask_opacity_index];
  }

  return 0;
}

/*******************************************************************************
** video_cycle_upscaling_mode_right()
*******************************************************************************/
short int video_cycle_upscaling_mode_right()
{
  if (G_upscaling_mode < VIDEO_NUM_UPSCALING_MODES - 1)
  {
    G_upscaling_mode += 1;
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  /* make sure blur filter and phosphor mask settings */
  /* are off if we are not on the TV video mode       */
  if (G_upscaling_mode != VIDEO_UPSCALING_MODE_TV)
  {
    G_blur_filter_sigma_index = 0;
    G_blur_filter_sigma = G_blur_filter_sigma_table[G_blur_filter_sigma_index];

    G_mask_opacity_index = 0;
    G_mask_opacity = G_mask_opacity_table[G_mask_opacity_index];
  }

  return 0;
}

/*******************************************************************************
** video_cycle_upscaling_mode_left()
*******************************************************************************/
short int video_cycle_upscaling_mode_left()
{
  if (G_upscaling_mode > 0)
  {
    G_upscaling_mode -= 1;
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  /* make sure blur filter and phosphor mask settings */
  /* are off if we are not on the TV video mode       */
  if (G_upscaling_mode != VIDEO_UPSCALING_MODE_TV)
  {
    G_blur_filter_sigma_index = 0;
    G_blur_filter_sigma = G_blur_filter_sigma_table[G_blur_filter_sigma_index];

    G_mask_opacity_index = 0;
    G_mask_opacity = G_mask_opacity_table[G_mask_opacity_index];
  }

  return 0;
}

/*******************************************************************************
** video_set_black_level()
*******************************************************************************/
short int video_set_black_level(int index)
{
  if ((index >= 0) && (index <= BLACK_LEVEL_TABLE_NUM_VALUES))
  {
    G_black_level_index = index;
    G_black_level = G_black_level_table[G_black_level_index];
  }

  return 0;
}

/*******************************************************************************
** video_increase_black_level()
*******************************************************************************/
short int video_increase_black_level()
{
  if (G_black_level_index < BLACK_LEVEL_TABLE_NUM_VALUES - 1)
  {
    G_black_level_index += 1;
    G_black_level = G_black_level_table[G_black_level_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** video_decrease_black_level()
*******************************************************************************/
short int video_decrease_black_level()
{
  if (G_black_level_index > 0)
  {
    G_black_level_index -= 1;
    G_black_level = G_black_level_table[G_black_level_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** video_set_white_level()
*******************************************************************************/
short int video_set_white_level(int index)
{
  if ((index >= 0) && (index <= WHITE_LEVEL_TABLE_NUM_VALUES))
  {
    G_white_level_index = index;
    G_white_level = G_white_level_table[G_white_level_index];
  }

  return 0;
}

/*******************************************************************************
** video_increase_white_level()
*******************************************************************************/
short int video_increase_white_level()
{
  if (G_white_level_index < WHITE_LEVEL_TABLE_NUM_VALUES - 1)
  {
    G_white_level_index += 1;
    G_white_level = G_white_level_table[G_white_level_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** video_decrease_white_level()
*******************************************************************************/
short int video_decrease_white_level()
{
  if (G_white_level_index > 0)
  {
    G_white_level_index -= 1;
    G_white_level = G_white_level_table[G_white_level_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** video_set_hue()
*******************************************************************************/
short int video_set_hue(int index)
{
  if ((index >= 0) && (index <= HUE_TABLE_NUM_VALUES))
  {
    G_hue_index = index;
    G_hue = G_hue_table[G_hue_index];
  }

  return 0;
}

/*******************************************************************************
** video_increase_hue()
*******************************************************************************/
short int video_increase_hue()
{
  if (G_hue_index < HUE_TABLE_NUM_VALUES - 1)
  {
    G_hue_index += 1;
    G_hue = G_hue_table[G_hue_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** video_decrease_hue()
*******************************************************************************/
short int video_decrease_hue()
{
  if (G_hue_index > 0)
  {
    G_hue_index -= 1;
    G_hue = G_hue_table[G_hue_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** video_set_saturation()
*******************************************************************************/
short int video_set_saturation(int index)
{
  if ((index >= 0) && (index <= SATURATION_TABLE_NUM_VALUES))
  {
    G_saturation_index = index;
    G_saturation = G_saturation_table[G_saturation_index];
  }

  return 0;
}

/*******************************************************************************
** video_increase_saturation()
*******************************************************************************/
short int video_increase_saturation()
{
  if (G_saturation_index < SATURATION_TABLE_NUM_VALUES - 1)
  {
    G_saturation_index += 1;
    G_saturation = G_saturation_table[G_saturation_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** video_decrease_saturation()
*******************************************************************************/
short int video_decrease_saturation()
{
  if (G_saturation_index > 0)
  {
    G_saturation_index -= 1;
    G_saturation = G_saturation_table[G_saturation_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** video_set_gamma()
*******************************************************************************/
short int video_set_gamma(int index)
{
  if ((index >= 0) && (index <= GAMMA_TABLE_NUM_VALUES))
  {
    G_gamma_index = index;
    G_gamma = G_gamma_table[G_gamma_index];
  }

  return 0;
}

/*******************************************************************************
** video_increase_gamma()
*******************************************************************************/
short int video_increase_gamma()
{
  if (G_gamma_index < GAMMA_TABLE_NUM_VALUES - 1)
  {
    G_gamma_index += 1;
    G_gamma = G_gamma_table[G_gamma_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** video_decrease_gamma()
*******************************************************************************/
short int video_decrease_gamma()
{
  if (G_gamma_index > 0)
  {
    G_gamma_index -= 1;
    G_gamma = G_gamma_table[G_gamma_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** video_set_blur_filter_sigma()
*******************************************************************************/
short int video_set_blur_filter_sigma(int index)
{
  /* make sure we are in the TV video mode */
  if (G_upscaling_mode != VIDEO_UPSCALING_MODE_TV)
    return 0;

  if ((index >= 0) && (index <= BLUR_FILTER_SIGMA_TABLE_NUM_VALUES))
  {
    G_blur_filter_sigma_index = index;
    G_blur_filter_sigma = G_blur_filter_sigma_table[G_blur_filter_sigma_index];
    video_compute_blur_filter();
  }

  return 0;
}

/*******************************************************************************
** video_increase_blur_filter_sigma()
*******************************************************************************/
short int video_increase_blur_filter_sigma()
{
  /* make sure we are in the TV video mode */
  if (G_upscaling_mode != VIDEO_UPSCALING_MODE_TV)
    return 0;

  if (G_blur_filter_sigma_index < BLUR_FILTER_SIGMA_TABLE_NUM_VALUES - 1)
  {
    G_blur_filter_sigma_index += 1;
    G_blur_filter_sigma = G_blur_filter_sigma_table[G_blur_filter_sigma_index];
    video_compute_blur_filter();
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** video_decrease_blur_filter_sigma()
*******************************************************************************/
short int video_decrease_blur_filter_sigma()
{
  /* make sure we are in the TV video mode */
  if (G_upscaling_mode != VIDEO_UPSCALING_MODE_TV)
    return 0;

  if (G_blur_filter_sigma_index > 0)
  {
    G_blur_filter_sigma_index -= 1;
    G_blur_filter_sigma = G_blur_filter_sigma_table[G_blur_filter_sigma_index];
    video_compute_blur_filter();
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** video_set_mask_opacity()
*******************************************************************************/
short int video_set_mask_opacity(int index)
{
  /* make sure we are in the TV video mode */
  if (G_upscaling_mode != VIDEO_UPSCALING_MODE_TV)
    return 0;

  if ((index >= 0) && (index <= MASK_OPACITY_TABLE_NUM_VALUES))
  {
    G_mask_opacity_index = index;
    G_mask_opacity = G_mask_opacity_table[G_mask_opacity_index];
  }

  return 0;
}

/*******************************************************************************
** video_increase_mask_opacity()
*******************************************************************************/
short int video_increase_mask_opacity()
{
  /* make sure we are in the TV video mode */
  if (G_upscaling_mode != VIDEO_UPSCALING_MODE_TV)
    return 0;

  if (G_mask_opacity_index < MASK_OPACITY_TABLE_NUM_VALUES - 1)
  {
    G_mask_opacity_index += 1;
    G_mask_opacity = G_mask_opacity_table[G_mask_opacity_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** video_decrease_mask_opacity()
*******************************************************************************/
short int video_decrease_mask_opacity()
{
  /* make sure we are in the TV video mode */
  if (G_upscaling_mode != VIDEO_UPSCALING_MODE_TV)
    return 0;

  if (G_mask_opacity_index > 0)
  {
    G_mask_opacity_index -= 1;
    G_mask_opacity = G_mask_opacity_table[G_mask_opacity_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

