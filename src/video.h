/*******************************************************************************
** video.h (video options)
*******************************************************************************/

#ifndef VIDEO_H
#define VIDEO_H

#include <glad/glad.h>

enum
{
  VIDEO_UPSCALING_MODE_LINEAR = 0,
  VIDEO_UPSCALING_MODE_PIXELS,
  VIDEO_UPSCALING_MODE_SCANLINES,
  VIDEO_NUM_UPSCALING_MODES
};

extern GLfloat G_cubic_matrix[16];

extern GLfloat G_rgb2yiq_matrix[9];
extern GLfloat G_yiq2rgb_matrix[9];

extern int G_upscaling_mode;

extern GLfloat G_black_level;
extern GLfloat G_white_level;

extern int G_brightness_index;

/* function declarations */
short int video_init_variables();

short int video_set_upscaling_mode(int mode);
short int video_cycle_upscaling_mode_right();
short int video_cycle_upscaling_mode_left();

short int video_set_brightness(int index);
short int video_increase_brightness();
short int video_decrease_brightness();

#endif
