/*******************************************************************************
** render.c (opengl rendering)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "fade.h"
#include "global.h"
#include "graphics.h"
#include "render.h"
#include "texture.h"
#include "video.h"

/* overscan rendering */
#define RENDER_OVERSCAN_TILES_OPENGL_SETTINGS()                                \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                           \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glEnable(GL_DEPTH_TEST);                                                     \
  glDisable(GL_BLEND);                                                         \
  glEnable(GL_CULL_FACE);

#define RENDER_OVERSCAN_SPRITES_OPENGL_SETTINGS()                              \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                           \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glDisable(GL_DEPTH_TEST);                                                    \
  glEnable(GL_BLEND);                                                          \
  glDisable(GL_CULL_FACE);

#define RENDER_SETUP_OVERSCAN_OUTPUT()                                                    \
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                                   \
  glClearDepth(1.0f);                                                                     \
                                                                                          \
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_1);                     \
  glViewport( 0, GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT - GRAPHICS_OVERSCAN_OUTPUT_HEIGHT,  \
              (GLsizei) GRAPHICS_OVERSCAN_OUTPUT_WIDTH,                                   \
              (GLsizei) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT);                                 \
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define RENDER_CHANGE_TEXTURE(program, name)                                   \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_ids[name]);                           \
  glUniform1i(G_uniform_##program##_texture_sampler_id, 0);

#define RENDER_SETUP_PALETTE(program)                                          \
  glActiveTexture(GL_TEXTURE1);                                                \
  glBindTexture(GL_TEXTURE_2D, G_palette_id);                                  \
  glUniform1i(G_uniform_##program##_palette_sampler_id, 1);

/* overscan shader programs */
#define RENDER_SET_SHADER_OV1()                                                \
  glUseProgram(G_program_id_OV1);                                              \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV1_mvp_matrix_id, 1,                          \
                      GL_FALSE, G_mvp_matrix_overscan);

#define RENDER_SET_SHADER_OV2()                                                \
  glUseProgram(G_program_id_OV2);                                              \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV2_mvp_matrix_id, 1,                          \
                      GL_FALSE, G_mvp_matrix_overscan);                        \
                                                                               \
  glUniform1i(G_uniform_OV2_fade_amount_id, G_fade_amount);                    \
  glUniform1i(G_uniform_OV2_subpalette_size_id, G_subpalette_size);            \

/* tiles */
#define RENDER_BEGIN_TILE_RENDERING()                                          \
  glEnableVertexAttribArray(0);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_tiles);                     \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                    \
                                                                               \
  glEnableVertexAttribArray(1);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_tiles);              \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);                    \
                                                                               \
  glEnableVertexAttribArray(2);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_palette_coord_buffer_id_tiles);              \
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_TILE_RENDERING()                                            \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);                                               \
  glDisableVertexAttribArray(2);

#define RENDER_DRAW_TILES()                                                    \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);              \
  glDrawElements(GL_TRIANGLES, G_num_tiles * 6, GL_UNSIGNED_SHORT, NULL);

/* sprites */
#define RENDER_BEGIN_SPRITE_RENDERING()                                        \
  glEnableVertexAttribArray(0);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                   \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                    \
                                                                               \
  glEnableVertexAttribArray(1);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);            \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);                    \
                                                                               \
  glEnableVertexAttribArray(2);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_palette_coord_buffer_id_sprites);            \
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_SPRITE_RENDERING()                                          \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);                                               \
  glDisableVertexAttribArray(2);

#define RENDER_DRAW_SPRITES()                                                  \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                   \
  glBufferSubData(GL_ARRAY_BUFFER, 0,                                          \
                  G_num_sprites * 12 * sizeof(GLfloat),                        \
                  G_vertex_buffer_sprites);                                    \
                                                                               \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);            \
  glBufferSubData(GL_ARRAY_BUFFER, 0,                                          \
                  G_num_sprites * 8 * sizeof(GLfloat),                         \
                  G_texture_coord_buffer_sprites);                             \
                                                                               \
  glBindBuffer(GL_ARRAY_BUFFER, G_palette_coord_buffer_id_sprites);            \
  glBufferSubData(GL_ARRAY_BUFFER, 0,                                          \
                  G_num_sprites * 4 * sizeof(GLfloat),                         \
                  G_palette_coord_buffer_sprites);                             \
                                                                               \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);            \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,                                  \
                  G_num_sprites * 6 * sizeof(unsigned short),                  \
                  G_index_buffer_sprites);                                     \
                                                                               \
  glDrawElements(GL_TRIANGLES, G_num_sprites * 6, GL_UNSIGNED_SHORT, NULL); 

/* postprocessing rendering */
#define RENDER_POSTPROCESSING_OPENGL_SETTINGS()                                \
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                        \
  glClearDepth(1.0f);                                                          \
                                                                               \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                           \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glDisable(GL_DEPTH_TEST);                                                    \
  glDisable(GL_BLEND);                                                         \
  glEnable(GL_CULL_FACE);

#define RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(num)                          \
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_##num);      \
  glViewport(0, 0,  (GLsizei) GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH,             \
                    (GLsizei) GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT);           \
  glClear(GL_COLOR_BUFFER_BIT);

#define RENDER_SETUP_WINDOW_OUTPUT()                                           \
  glBindFramebuffer(GL_FRAMEBUFFER, 0);                                        \
  glViewport(0, 0, (GLsizei) G_viewport_w, (GLsizei) G_viewport_h);            \
  glClear(GL_COLOR_BUFFER_BIT);

#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()                          \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_overscan);        \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_RESIZED()                           \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_resized);         \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_RESIZED_TO_RESIZED()                            \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_resized);         \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_resized);  \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_WINDOW()                            \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);          \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_RESIZED_TO_WINDOW()                             \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);          \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_resized);  \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_POSTPROCESSING()                                            \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);

#define RENDER_POSTPROCESSING_DRAW_QUAD()                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_postprocessing_all); \
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

/* postprocessing shader programs */
#define RENDER_SET_SHADER_A(num)                                                          \
  glUseProgram(G_program_id_A);                                                           \
                                                                                          \
  glUniformMatrix4fv(G_uniform_A_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_intermediate);  \
                                                                                          \
  glUniformMatrix3fv(G_uniform_A_rgb2yiq_matrix_id, 1, GL_FALSE, G_rgb2yiq_matrix);       \
  glUniformMatrix3fv(G_uniform_A_yiq2rgb_matrix_id, 1, GL_FALSE, G_yiq2rgb_matrix);       \
  glUniform1f(G_uniform_A_black_level_id, G_black_level);                                 \
  glUniform1f(G_uniform_A_white_level_id, G_white_level);                                 \
  glUniform1f(G_uniform_A_hue_id, G_hue);                                                 \
  glUniform1f(G_uniform_A_saturation_id, G_saturation);                                   \
  glUniform1f(G_uniform_A_gamma_id, G_gamma);                                             \
                                                                                          \
  glActiveTexture(GL_TEXTURE0);                                                           \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                          \
  glUniform1i(G_uniform_A_texture_sampler_id, 0);

#define RENDER_SET_SHADER_B(num)                                                          \
  glUseProgram(G_program_id_B);                                                           \
                                                                                          \
  glUniformMatrix4fv(G_uniform_B_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_intermediate);  \
                                                                                          \
  glUniform1fv(G_uniform_B_blur_filter_id, BLUR_FILTER_BOUND, G_blur_filter);             \
                                                                                          \
  glActiveTexture(GL_TEXTURE0);                                                           \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                          \
  glUniform1i(G_uniform_B_texture_sampler_id, 0);

#define RENDER_SET_SHADER_C(num)                                                          \
  glUseProgram(G_program_id_C);                                                           \
                                                                                          \
  glUniformMatrix4fv(G_uniform_C_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_intermediate);  \
                                                                                          \
  glUniformMatrix4fv(G_uniform_C_cubic_matrix_id, 1, GL_FALSE, G_cubic_matrix);           \
                                                                                          \
  glActiveTexture(GL_TEXTURE0);                                                           \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                          \
  glUniform1i(G_uniform_C_texture_sampler_id, 0);

#define RENDER_SET_SHADER_D1(num)                                                         \
  glUseProgram(G_program_id_D1);                                                          \
                                                                                          \
  glUniformMatrix4fv(G_uniform_D1_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_intermediate); \
                                                                                          \
  glUniform1f(G_uniform_D1_mask_opacity_id, G_mask_opacity);                              \
                                                                                          \
  glActiveTexture(GL_TEXTURE0);                                                           \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                          \
  glUniform1i(G_uniform_D1_texture_sampler_id, 0);

#define RENDER_SET_SHADER_D2(num)                                                         \
  glUseProgram(G_program_id_D2);                                                          \
                                                                                          \
  glUniformMatrix4fv(G_uniform_D2_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_intermediate); \
                                                                                          \
  glUniform1f(G_uniform_D2_mask_opacity_id, G_mask_opacity);                              \
                                                                                          \
  glActiveTexture(GL_TEXTURE0);                                                           \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                          \
  glUniform1i(G_uniform_D2_texture_sampler_id, 0);

#define RENDER_SET_SHADER_E(num)                                                          \
  glUseProgram(G_program_id_E);                                                           \
                                                                                          \
  glUniformMatrix4fv(G_uniform_E_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_window);        \
                                                                                          \
  glActiveTexture(GL_TEXTURE0);                                                           \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                          \
  glUniform1i(G_uniform_E_texture_sampler_id, 0);

#define RENDER_SET_SHADER_L(num)                                                          \
  glUseProgram(G_program_id_L);                                                           \
                                                                                          \
  glUniformMatrix4fv(G_uniform_L_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_window);        \
                                                                                          \
  glActiveTexture(GL_TEXTURE0);                                                           \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                          \
  glUniform1i(G_uniform_L_texture_sampler_id, 0);

/*******************************************************************************
** render_reset_vbos()
*******************************************************************************/
short int render_reset_vbos()
{
  int k;

  G_num_tiles = 0;
  G_num_sprites = 0;

  for (k = 0; k < GRAPHICS_SAVED_SPRITE_LAYERS; k++)
    G_num_saved_sprites[k] = 0;

  return 0;
}

/*******************************************************************************
** render_scene_standard()
*******************************************************************************/
short int render_scene_standard()
{
  /* start rendering */
  RENDER_SETUP_OVERSCAN_OUTPUT()

  /* initialize mvp matrix for backdrop tiles */
  graphics_setup_overscan_mvp_matrix(0, 0);

  /* start of tile rendering */
  RENDER_OVERSCAN_TILES_OPENGL_SETTINGS()
  RENDER_SET_SHADER_OV1()
  RENDER_BEGIN_TILE_RENDERING()

  /* set textures */
  RENDER_SETUP_PALETTE(OV1)
  RENDER_CHANGE_TEXTURE(OV1, 0)

  /* render tiles */
  if (G_num_tiles > 0)
  {
    RENDER_DRAW_TILES()
  }

  /* end of tile rendering */
  RENDER_END_TILE_RENDERING()

  /* start of sprite rendering */
  RENDER_OVERSCAN_SPRITES_OPENGL_SETTINGS()
  RENDER_SET_SHADER_OV1()
  RENDER_BEGIN_SPRITE_RENDERING()

  /* set textures */
  RENDER_SETUP_PALETTE(OV1)
  RENDER_CHANGE_TEXTURE(OV1, 0)

  /* render sprites */
  if (G_num_sprites > 0)
  {
    RENDER_DRAW_SPRITES()
  }

  /* end of sprite rendering */
  RENDER_END_SPRITE_RENDERING()

  return 0;
}

/*******************************************************************************
** render_scene_fade()
*******************************************************************************/
short int render_scene_fade()
{
  /* start rendering */
  RENDER_SETUP_OVERSCAN_OUTPUT()

  /* initialize mvp matrix for backdrop tiles */
  graphics_setup_overscan_mvp_matrix(0, 0);

  /* start of tile rendering */
  RENDER_OVERSCAN_TILES_OPENGL_SETTINGS()
  RENDER_SET_SHADER_OV2()
  RENDER_BEGIN_TILE_RENDERING()

  /* set textures */
  RENDER_SETUP_PALETTE(OV2)
  RENDER_CHANGE_TEXTURE(OV2, 0)

  /* render tiles */
  if (G_num_tiles > 0)
  {
    RENDER_DRAW_TILES()
  }

  /* end of tile rendering */
  RENDER_END_TILE_RENDERING()

  /* start of sprite rendering */
  RENDER_OVERSCAN_SPRITES_OPENGL_SETTINGS()
  RENDER_SET_SHADER_OV2()
  RENDER_BEGIN_SPRITE_RENDERING()

  /* set textures */
  RENDER_SETUP_PALETTE(OV2)
  RENDER_CHANGE_TEXTURE(OV2, 0)

  /* render sprites */
  if (G_num_sprites > 0)
  {
    RENDER_DRAW_SPRITES()
  }

  /* end of sprite rendering */
  RENDER_END_SPRITE_RENDERING()

  return 0;
}

/*******************************************************************************
** render_postprocessing_path_A()
*******************************************************************************/
short int render_postprocessing_path_A()
{
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()

  /* pass A (apply settings) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_A(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass L (linear upscale to window) */
  RENDER_SETUP_WINDOW_OUTPUT()
  RENDER_SET_SHADER_L(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

/*******************************************************************************
** render_postprocessing_path_ACE()
*******************************************************************************/
short int render_postprocessing_path_ACE()
{
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()

  /* pass A (apply settings) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_A(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass C (cubic horizontal resize) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(1)
  RENDER_SET_SHADER_C(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_RESIZED()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass E (scanline vertical resize) */
  RENDER_SETUP_WINDOW_OUTPUT()
  RENDER_SET_SHADER_E(1)
  RENDER_BEGIN_POSTPROCESSING_RESIZED_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

/*******************************************************************************
** render_postprocessing_path_ABCE()
*******************************************************************************/
short int render_postprocessing_path_ABCE()
{
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()

  /* pass A (apply settings) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_A(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass B (blur filter) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(1)
  RENDER_SET_SHADER_B(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass C (cubic horizontal resize) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_C(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_RESIZED()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass E (scanline vertical resize) */
  RENDER_SETUP_WINDOW_OUTPUT()
  RENDER_SET_SHADER_E(2)
  RENDER_BEGIN_POSTPROCESSING_RESIZED_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

/*******************************************************************************
** render_postprocessing_path_ACDE()
*******************************************************************************/
short int render_postprocessing_path_ACDE()
{
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()

  /* pass A (apply settings) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_A(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass C (cubic horizontal resize) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(1)
  RENDER_SET_SHADER_C(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_RESIZED()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass D (phosphor mask) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(2)

  if ((G_graphics_resolution == GRAPHICS_RESOLUTION_480P) || 
      (G_graphics_resolution == GRAPHICS_RESOLUTION_720P) || 
      (G_graphics_resolution == GRAPHICS_RESOLUTION_768P))
  {
    RENDER_SET_SHADER_D1(1)
  }
  else if (G_graphics_resolution == GRAPHICS_RESOLUTION_1080P)
  {
    RENDER_SET_SHADER_D2(1)
  }
  else
  {
    RENDER_SET_SHADER_D1(1)
  }

  RENDER_BEGIN_POSTPROCESSING_RESIZED_TO_RESIZED()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass E (scanline vertical resize) */
  RENDER_SETUP_WINDOW_OUTPUT()
  RENDER_SET_SHADER_E(2)
  RENDER_BEGIN_POSTPROCESSING_RESIZED_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

/*******************************************************************************
** render_postprocessing_path_ABCDE()
*******************************************************************************/
short int render_postprocessing_path_ABCDE()
{
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()

  /* pass A (apply settings) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_A(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass B (blur filter) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(1)
  RENDER_SET_SHADER_B(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass C (cubic horizontal resize) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_C(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_RESIZED()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass D (phosphor mask) */
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(1)

  if ((G_graphics_resolution == GRAPHICS_RESOLUTION_480P) || 
      (G_graphics_resolution == GRAPHICS_RESOLUTION_720P) || 
      (G_graphics_resolution == GRAPHICS_RESOLUTION_768P))
  {
    RENDER_SET_SHADER_D1(2)
  }
  else if (G_graphics_resolution == GRAPHICS_RESOLUTION_1080P)
  {
    RENDER_SET_SHADER_D2(2)
  }
  else
  {
    RENDER_SET_SHADER_D1(2)
  }

  RENDER_BEGIN_POSTPROCESSING_RESIZED_TO_RESIZED()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass E (scanline vertical resize) */
  RENDER_SETUP_WINDOW_OUTPUT()
  RENDER_SET_SHADER_E(1)
  RENDER_BEGIN_POSTPROCESSING_RESIZED_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

