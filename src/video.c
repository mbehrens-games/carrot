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

#define BRIGHTNESS_STEP 0.015625f /* 1/64 */

#define BRIGHTNESS_TABLE_NUM_VALUES 9

/* settings tables */
GLfloat G_black_level_table[9] = 
          { 0.0f,
            1 * BRIGHTNESS_STEP,
            2 * BRIGHTNESS_STEP,
            3 * BRIGHTNESS_STEP,
            4 * BRIGHTNESS_STEP,
            5 * BRIGHTNESS_STEP,
            6 * BRIGHTNESS_STEP,
            7 * BRIGHTNESS_STEP,
            8 * BRIGHTNESS_STEP};

GLfloat G_white_level_table[9] = 
          { 1.0f - 8 * BRIGHTNESS_STEP,
            1.0f - 7 * BRIGHTNESS_STEP,
            1.0f - 6 * BRIGHTNESS_STEP,
            1.0f - 5 * BRIGHTNESS_STEP,
            1.0f - 4 * BRIGHTNESS_STEP,
            1.0f - 3 * BRIGHTNESS_STEP,
            1.0f - 2 * BRIGHTNESS_STEP,
            1.0f - 1 * BRIGHTNESS_STEP,
            1.0f};

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

int G_brightness_index;

/*******************************************************************************
** video_init_variables()
*******************************************************************************/
short int video_init_variables()
{
  G_upscaling_mode = VIDEO_UPSCALING_MODE_LINEAR;

  G_brightness_index = BRIGHTNESS_TABLE_NUM_VALUES / 2;

  G_black_level = G_black_level_table[G_brightness_index];
  G_white_level = G_white_level_table[G_brightness_index];

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

  return 0;
}

/*******************************************************************************
** video_set_brightness()
*******************************************************************************/
short int video_set_brightness(int index)
{
  if ((index >= 0) && (index <= BRIGHTNESS_TABLE_NUM_VALUES))
  {
    G_brightness_index = index;
    G_black_level = G_black_level_table[G_brightness_index];
    G_white_level = G_white_level_table[G_brightness_index];
  }

  return 0;
}

/*******************************************************************************
** video_increase_brightness()
*******************************************************************************/
short int video_increase_brightness()
{
  if (G_brightness_index < BRIGHTNESS_TABLE_NUM_VALUES - 1)
  {
    G_brightness_index += 1;
    G_black_level = G_black_level_table[G_brightness_index];
    G_white_level = G_white_level_table[G_brightness_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** video_decrease_brightness()
*******************************************************************************/
short int video_decrease_brightness()
{
  if (G_brightness_index > 0)
  {
    G_brightness_index -= 1;
    G_black_level = G_black_level_table[G_brightness_index];
    G_white_level = G_white_level_table[G_brightness_index];
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}
