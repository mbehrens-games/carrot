/*******************************************************************************
** shaders.h (opengl shaders)
*******************************************************************************/

#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>

enum
{
  SHADERS_PROGRAM_OV_A = 0,
  SHADERS_PROGRAM_OV_B,
  SHADERS_PROGRAM_OV_C,
  SHADERS_PROGRAM_OV_D,
  SHADERS_PROGRAM_OV_E,
  SHADERS_PROGRAM_UP_A,
  SHADERS_PROGRAM_UP_B,
  SHADERS_PROGRAM_UP_C,
  SHADERS_PROGRAM_UP_D,
  SHADERS_PROGRAM_UP_E,
  SHADERS_NUM_PROGRAMS
};

/* shader program and uniform ids */
extern GLuint G_program_id_OV_A;
extern GLuint G_program_id_OV_B;
extern GLuint G_program_id_OV_C;
extern GLuint G_program_id_OV_D;
extern GLuint G_program_id_OV_E;

extern GLuint G_program_id_UP_A;
extern GLuint G_program_id_UP_B;
extern GLuint G_program_id_UP_C;
extern GLuint G_program_id_UP_D;
extern GLuint G_program_id_UP_E;

extern GLuint G_uniform_OV_A_mvp_matrix_id;
extern GLuint G_uniform_OV_A_texture_sampler_id;

extern GLuint G_uniform_OV_B_mvp_matrix_id;
extern GLuint G_uniform_OV_B_texture_sampler_id;
extern GLuint G_uniform_OV_B_parallax_sampler_id;
extern GLuint G_uniform_OV_B_hori_shift_id;
extern GLuint G_uniform_OV_B_vert_shift_id;
extern GLuint G_uniform_OV_B_vert_weight_id;

extern GLuint G_uniform_OV_C_mvp_matrix_id;
extern GLuint G_uniform_OV_C_texture_sampler_id;

extern GLuint G_uniform_OV_D_mvp_matrix_id;
extern GLuint G_uniform_OV_D_texture_sampler_id;
extern GLuint G_uniform_OV_D_amount_id;

extern GLuint G_uniform_OV_E_mvp_matrix_id;
extern GLuint G_uniform_OV_E_texture_sampler_id;
extern GLuint G_uniform_OV_E_palette_sampler_id;
extern GLuint G_uniform_OV_E_levels_id;

extern GLuint G_uniform_UP_A_mvp_matrix_id;
extern GLuint G_uniform_UP_A_texture_sampler_id;
extern GLuint G_uniform_UP_A_rgb2yiq_matrix_id;
extern GLuint G_uniform_UP_A_yiq2rgb_matrix_id;
extern GLuint G_uniform_UP_A_black_level_id;
extern GLuint G_uniform_UP_A_white_level_id;

extern GLuint G_uniform_UP_B_mvp_matrix_id;
extern GLuint G_uniform_UP_B_texture_sampler_id;

extern GLuint G_uniform_UP_C_mvp_matrix_id;
extern GLuint G_uniform_UP_C_texture_sampler_id;

extern GLuint G_uniform_UP_D_mvp_matrix_id;
extern GLuint G_uniform_UP_D_texture_sampler_id;
extern GLuint G_uniform_UP_D_cubic_matrix_id;

extern GLuint G_uniform_UP_E_mvp_matrix_id;
extern GLuint G_uniform_UP_E_texture_sampler_id;

/* function declarations */
short int shaders_create_programs();
short int shaders_destroy_programs();

#endif
