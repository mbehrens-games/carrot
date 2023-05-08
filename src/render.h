/*******************************************************************************
** render.h (opengl rendering)
*******************************************************************************/

#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

/* function declarations */
short int render_reset_vbos();

short int render_scene_all();

short int render_postprocessing_linear();
short int render_postprocessing_pixels();
short int render_postprocessing_scanlines();

#endif
