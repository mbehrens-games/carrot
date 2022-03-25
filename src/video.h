/*******************************************************************************
** video.h (video options)
*******************************************************************************/

#ifndef VIDEO_H
#define VIDEO_H

#include <glad/glad.h>

#define BLUR_FILTER_M     4
#define BLUR_FILTER_BOUND 3 /* array size: (M / 2) + 1 */

enum
{
  VIDEO_UPSCALING_MODE_LINEAR = 0,
  VIDEO_UPSCALING_MODE_TV,
  VIDEO_NUM_UPSCALING_MODES
};

extern GLfloat G_blur_filter[BLUR_FILTER_BOUND];

extern GLfloat G_cubic_matrix[16];

extern GLfloat G_rgb2yiq_matrix[9];
extern GLfloat G_yiq2rgb_matrix[9];

extern int G_upscaling_mode;

extern GLfloat G_black_level;
extern GLfloat G_white_level;
extern GLfloat G_hue;
extern GLfloat G_saturation;
extern GLfloat G_gamma;
extern GLfloat G_blur_filter_sigma;
extern GLfloat G_mask_opacity;

extern int G_black_level_index;
extern int G_white_level_index;
extern int G_hue_index;
extern int G_saturation_index;
extern int G_gamma_index;
extern int G_blur_filter_sigma_index;
extern int G_mask_opacity_index;

/* function declarations */
short int video_compute_blur_filter();
short int video_init_variables();

short int video_set_upscaling_mode(int mode);
short int video_cycle_upscaling_mode_right();
short int video_cycle_upscaling_mode_left();

short int video_set_black_level(int index);
short int video_increase_black_level();
short int video_decrease_black_level();

short int video_set_white_level(int index);
short int video_increase_white_level();
short int video_decrease_white_level();

short int video_set_hue(int index);
short int video_increase_hue();
short int video_decrease_hue();

short int video_set_saturation(int index);
short int video_increase_saturation();
short int video_decrease_saturation();

short int video_set_gamma(int index);
short int video_increase_gamma();
short int video_decrease_gamma();

short int video_set_blur_filter_sigma(int index);
short int video_increase_blur_filter_sigma();
short int video_decrease_blur_filter_sigma();

short int video_set_mask_opacity(int index);
short int video_increase_mask_opacity();
short int video_decrease_mask_opacity();

#endif
