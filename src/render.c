/*******************************************************************************
** render.c (opengl rendering)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "fade.h"
#include "global.h"
#include "graphics.h"
#include "palette.h"
#include "parallax.h"
#include "render.h"
#include "shaders.h"
#include "texture.h"
#include "video.h"

/* set opengl settings */
#define RENDER_TILES_OPENGL_SETTINGS()                                         \
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                        \
  glClearDepth(1.0f);                                                          \
                                                                               \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                           \
  glBlendEquation(GL_FUNC_ADD);                                                \
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);                             \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glEnable(GL_DEPTH_TEST);                                                     \
  glDisable(GL_BLEND);                                                         \
  glEnable(GL_CULL_FACE);

#define RENDER_SPRITES_OPENGL_SETTINGS()                                       \
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                        \
  glClearDepth(1.0f);                                                          \
                                                                               \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                           \
  glBlendEquation(GL_FUNC_ADD);                                                \
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);                             \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glEnable(GL_DEPTH_TEST);                                                     \
  glEnable(GL_BLEND);                                                          \
  glDisable(GL_CULL_FACE);

#define RENDER_FADE_OPENGL_SETTINGS()                                          \
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                        \
  glClearDepth(1.0f);                                                          \
                                                                               \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_ONE, GL_ONE);                                                 \
  glBlendEquation(GL_FUNC_ADD);                                                \
  glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);                          \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glDisable(GL_DEPTH_TEST);                                                    \
  glEnable(GL_BLEND);                                                          \
  glEnable(GL_CULL_FACE);

#define RENDER_POSTPROCESSING_OPENGL_SETTINGS()                                \
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                        \
  glClearDepth(1.0f);                                                          \
                                                                               \
  glDepthFunc(GL_LEQUAL);                                                      \
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                           \
  glBlendEquation(GL_FUNC_ADD);                                                \
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);                             \
                                                                               \
  glFrontFace(GL_CCW);                                                         \
  glCullFace(GL_BACK);                                                         \
                                                                               \
  glDisable(GL_DEPTH_TEST);                                                    \
  glDisable(GL_BLEND);                                                         \
  glEnable(GL_CULL_FACE);

/* framebuffer setup */
#define RENDER_SETUP_OVERSCAN_OUTPUT(num)                                         \
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_##num);         \
  glViewport( 0, GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT - GRAPHICS_OVERSCAN_HEIGHT, \
              (GLsizei) GRAPHICS_OVERSCAN_WIDTH,                                  \
              (GLsizei) GRAPHICS_OVERSCAN_HEIGHT);

#define RENDER_SETUP_AND_CLEAR_OVERSCAN_OUTPUT(num)                            \
  RENDER_SETUP_OVERSCAN_OUTPUT(num)                                            \
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_AND_RESET_DEPTH_OVERSCAN_OUTPUT(num)                      \
  RENDER_SETUP_OVERSCAN_OUTPUT(num)                                            \
  glClear(GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(num)                          \
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_##num);      \
  glViewport(0, 0,  (GLsizei) GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH,             \
                    (GLsizei) GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT);           \

#define RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(num)                \
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(num)                                \
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_AND_RESET_DEPTH_INTERMEDIATE_TEXTURE_OUTPUT(num)          \
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(num)                                \
  glClear(GL_DEPTH_BUFFER_BIT);

#define RENDER_SETUP_AND_CLEAR_WINDOW_OUTPUT()                                 \
  glBindFramebuffer(GL_FRAMEBUFFER, 0);                                        \
  glViewport(0, 0, (GLsizei) G_viewport_w, (GLsizei) G_viewport_h);            \
  glClear(GL_COLOR_BUFFER_BIT);

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
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_tiles);       \
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_TILE_RENDERING()                                            \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);                                               \
  glDisableVertexAttribArray(2);

#define RENDER_DRAW_BACKDROP()                                                 \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);              \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_tile_layer_counts[GRAPHICS_TILE_LAYER_BACKDROP],       \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_BACKDROP_TILES_START_INDEX));

#define RENDER_DRAW_SKY()                                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);              \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_tile_layer_counts[GRAPHICS_TILE_LAYER_SKY],            \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_SKY_TILES_START_INDEX));

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
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);     \
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_SPRITE_RENDERING()                                          \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);                                               \
  glDisableVertexAttribArray(2);

#define RENDER_DRAW_GRID_OBJECTS_AND_THINGS()                                               \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);                         \
  glDrawElements( GL_TRIANGLES,                                                             \
                  6 * G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS], \
                  GL_UNSIGNED_SHORT,                                                        \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX));

#define RENDER_DRAW_PANELS()                                                   \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);            \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS],     \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_PANELS_SPRITES_START_INDEX));

#define RENDER_DRAW_OVERLAY()                                                  \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);            \
  glDrawElements( GL_TRIANGLES,                                                \
                  6 * G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY],    \
                  GL_UNSIGNED_SHORT,                                           \
                  (void *) (sizeof(unsigned short) * 6 * GRAPHICS_OVERLAY_SPRITES_START_INDEX));

/* fade */
#define RENDER_BEGIN_DARKENED_PANELS_RENDERING()                               \
  glEnableVertexAttribArray(0);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                   \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_DARKENED_PANELS_RENDERING()                                 \
  glDisableVertexAttribArray(0);

#define RENDER_BEGIN_FADE_RENDERING()                                          \
  glEnableVertexAttribArray(0);                                                \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_overscan);   \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_FADE_RENDERING()                                            \
  glDisableVertexAttribArray(0);

/* postprocessing */
#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_SKY()                               \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_sky);             \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()                          \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_overscan);        \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_NEAREST_RESIZE()                    \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_nearest_resize);  \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_CUBIC_RESIZE()                      \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_cubic_resize);    \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_WINDOW()                            \
  glEnableVertexAttribArray(0);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);          \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                         \
                                                                                    \
  glEnableVertexAttribArray(1);                                                     \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_NEAREST_RESIZE_TO_WINDOW()                            \
  glEnableVertexAttribArray(0);                                                           \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);                \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                               \
                                                                                          \
  glEnableVertexAttribArray(1);                                                           \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_nearest_resize); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_BEGIN_POSTPROCESSING_CUBIC_RESIZE_TO_WINDOW()                            \
  glEnableVertexAttribArray(0);                                                         \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);              \
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);                             \
                                                                                        \
  glEnableVertexAttribArray(1);                                                         \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_cubic_resize); \
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

#define RENDER_END_POSTPROCESSING()                                            \
  glDisableVertexAttribArray(0);                                               \
  glDisableVertexAttribArray(1);

#define RENDER_POSTPROCESSING_DRAW_QUAD()                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_postprocessing_all); \
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

/* overscan shader programs setup */
#define RENDER_SET_SHADER_OV_A_TILES()                                         \
  glUseProgram(G_program_id_OV_A);                                             \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV_A_mvp_matrix_id, 1,                         \
                      GL_FALSE, G_mvp_matrix_tiles);                           \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_rom_data);                         \
  glUniform1i(G_uniform_OV_A_texture_sampler_id, 0);

#define RENDER_SET_SHADER_OV_A_SKY()                                           \
  glUseProgram(G_program_id_OV_A);                                             \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV_A_mvp_matrix_id, 1,                         \
                      GL_FALSE, G_mvp_matrix_overscan);                        \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_rom_data);                         \
  glUniform1i(G_uniform_OV_A_texture_sampler_id, 0);

#define RENDER_SET_SHADER_OV_B(num)                                            \
  glUseProgram(G_program_id_OV_B);                                             \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV_B_mvp_matrix_id, 1,                         \
                      GL_FALSE, G_mvp_matrix_intermediate);                    \
                                                                               \
  glUniform1i(G_uniform_OV_B_hori_shift_id, G_parallax_hori_shift);            \
  glUniform1i(G_uniform_OV_B_vert_shift_id, G_parallax_vert_shift);            \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);               \
  glUniform1i(G_uniform_OV_B_texture_sampler_id, 0);                           \
                                                                               \
  glActiveTexture(GL_TEXTURE1);                                                \
  glBindTexture(GL_TEXTURE_1D, G_texture_id_parallax);                         \
  glUniform1i(G_uniform_OV_B_parallax_sampler_id, 1);

#define RENDER_SET_SHADER_OV_C_SPRITES()                                       \
  glUseProgram(G_program_id_OV_C);                                             \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV_C_mvp_matrix_id, 1,                         \
                      GL_FALSE, G_mvp_matrix_sprites);                         \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_rom_data);                         \
  glUniform1i(G_uniform_OV_C_texture_sampler_id, 0);

#define RENDER_SET_SHADER_OV_C_PANELS_AND_OVERLAY()                            \
  glUseProgram(G_program_id_OV_C);                                             \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV_C_mvp_matrix_id, 1,                         \
                      GL_FALSE, G_mvp_matrix_overscan);                        \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_rom_data);                         \
  glUniform1i(G_uniform_OV_C_texture_sampler_id, 0);

#define RENDER_SET_SHADER_OV_D_PANELS()                                        \
  glUseProgram(G_program_id_OV_D);                                             \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV_D_mvp_matrix_id, 1,                         \
                      GL_FALSE, G_mvp_matrix_overscan);                        \
                                                                               \
  glUniform1f(G_uniform_OV_D_amount_id, G_fade_panels);

#define RENDER_SET_SHADER_OV_D_FADE()                                          \
  glUseProgram(G_program_id_OV_D);                                             \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV_D_mvp_matrix_id, 1,                         \
                      GL_FALSE, G_mvp_matrix_intermediate);                    \
                                                                               \
  glUniform1f(G_uniform_OV_D_amount_id, G_fade_amount);

#define RENDER_SET_SHADER_OV_E(num)                                            \
  glUseProgram(G_program_id_OV_E);                                             \
                                                                               \
  glUniformMatrix4fv( G_uniform_OV_E_mvp_matrix_id, 1,                         \
                      GL_FALSE, G_mvp_matrix_intermediate);                    \
                                                                               \
  glUniform1i(G_uniform_OV_E_levels_id, G_palette_levels);                     \
                                                                               \
  glActiveTexture(GL_TEXTURE0);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);               \
  glUniform1i(G_uniform_OV_E_texture_sampler_id, 0);                           \
                                                                               \
  glActiveTexture(GL_TEXTURE1);                                                \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_palette);                          \
  glUniform1i(G_uniform_OV_E_palette_sampler_id, 1);

/* postprocessing shader programs setup */
#define RENDER_SET_SHADER_UP_A(num)                                                         \
  glUseProgram(G_program_id_UP_A);                                                          \
                                                                                            \
  glUniformMatrix4fv(G_uniform_UP_A_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_intermediate); \
                                                                                            \
  glUniformMatrix3fv(G_uniform_UP_A_rgb2yiq_matrix_id, 1, GL_FALSE, G_rgb2yiq_matrix);      \
  glUniformMatrix3fv(G_uniform_UP_A_yiq2rgb_matrix_id, 1, GL_FALSE, G_yiq2rgb_matrix);      \
  glUniform1f(G_uniform_UP_A_black_level_id, G_black_level);                                \
  glUniform1f(G_uniform_UP_A_white_level_id, G_white_level);                                \
                                                                                            \
  glActiveTexture(GL_TEXTURE0);                                                             \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                            \
  glUniform1i(G_uniform_UP_A_texture_sampler_id, 0);

#define RENDER_SET_SHADER_UP_B(num)                                                         \
  glUseProgram(G_program_id_UP_B);                                                          \
                                                                                            \
  glUniformMatrix4fv(G_uniform_UP_B_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_intermediate); \
                                                                                            \
  glActiveTexture(GL_TEXTURE0);                                                             \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                            \
  glUniform1i(G_uniform_UP_B_texture_sampler_id, 0);

#define RENDER_SET_SHADER_UP_C(num)                                                       \
  glUseProgram(G_program_id_UP_C);                                                        \
                                                                                          \
  glUniformMatrix4fv(G_uniform_UP_C_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_window);     \
                                                                                          \
  glActiveTexture(GL_TEXTURE0);                                                           \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                          \
  glUniform1i(G_uniform_UP_C_texture_sampler_id, 0);

#define RENDER_SET_SHADER_UP_D(num)                                                         \
  glUseProgram(G_program_id_UP_D);                                                          \
                                                                                            \
  glUniformMatrix4fv(G_uniform_UP_D_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_intermediate); \
                                                                                            \
  glUniformMatrix4fv(G_uniform_UP_D_cubic_matrix_id, 1, GL_FALSE, G_cubic_matrix);          \
                                                                                            \
  glActiveTexture(GL_TEXTURE0);                                                             \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                            \
  glUniform1i(G_uniform_UP_D_texture_sampler_id, 0);

#define RENDER_SET_SHADER_UP_E(num)                                                       \
  glUseProgram(G_program_id_UP_E);                                                        \
                                                                                          \
  glUniformMatrix4fv(G_uniform_UP_E_mvp_matrix_id, 1, GL_FALSE, G_mvp_matrix_window);     \
                                                                                          \
  glActiveTexture(GL_TEXTURE0);                                                           \
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_##num);                          \
  glUniform1i(G_uniform_UP_E_texture_sampler_id, 0);

/*******************************************************************************
** render_reset_vbos()
*******************************************************************************/
short int render_reset_vbos()
{
  int k;

  for (k = 0; k < GRAPHICS_NUM_TILE_LAYERS; k++)
    G_tile_layer_counts[k] = 0;

  for (k = 0; k < GRAPHICS_NUM_SPRITE_LAYERS; k++)
    G_sprite_layer_counts[k] = 0;

  return 0;
}

/*******************************************************************************
** render_scene_all()
*******************************************************************************/
short int render_scene_all()
{
  /* setup matrices */
  graphics_setup_tiles_mvp_matrix(0, 0);
  graphics_setup_sprites_mvp_matrix(0, 0);

  /* update parallax scrolling */
  parallax_increment_shift(0, 0);

  /* pass 1 - backdrop tile rendering */
  RENDER_TILES_OPENGL_SETTINGS()
  RENDER_SETUP_AND_CLEAR_OVERSCAN_OUTPUT(2)
  RENDER_SET_SHADER_OV_A_TILES()
  RENDER_BEGIN_TILE_RENDERING()

  if (G_tile_layer_counts[GRAPHICS_TILE_LAYER_BACKDROP] > 0)
  {
    RENDER_DRAW_BACKDROP()
  }

  RENDER_END_TILE_RENDERING()

  /* pass 2 - sky tile rendering */
  RENDER_TILES_OPENGL_SETTINGS()
  RENDER_SETUP_AND_CLEAR_OVERSCAN_OUTPUT(1)
  RENDER_SET_SHADER_OV_A_SKY()
  RENDER_BEGIN_TILE_RENDERING()

  if (G_tile_layer_counts[GRAPHICS_TILE_LAYER_SKY] > 0)
  {
    RENDER_DRAW_SKY()
  }

  RENDER_END_TILE_RENDERING()

  /* pass 3 - sky parallax */
  RENDER_TILES_OPENGL_SETTINGS()
  RENDER_SETUP_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_OV_B(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_SKY()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass 4 - grid objects & things sprite rendering */
  RENDER_SPRITES_OPENGL_SETTINGS()
  RENDER_SETUP_OVERSCAN_OUTPUT(2)
  RENDER_SET_SHADER_OV_C_SPRITES()
  RENDER_BEGIN_SPRITE_RENDERING()

  if (G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS] > 0)
  {
    RENDER_DRAW_GRID_OBJECTS_AND_THINGS()
  }

  RENDER_END_SPRITE_RENDERING()

#if 0
  /* pass 5 - apply panel darkening */
  RENDER_FADE_OPENGL_SETTINGS()
  RENDER_SETUP_AND_RESET_DEPTH_OVERSCAN_OUTPUT(2)
  RENDER_SET_SHADER_OV_D_PANELS()
  RENDER_BEGIN_DARKENED_PANELS_RENDERING()

  if (G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS] > 0)
  {
    RENDER_DRAW_PANELS()
  }

  RENDER_END_DARKENED_PANELS_RENDERING()
#endif

  /* pass 6 - panel & overlay sprite rendering */
  RENDER_SPRITES_OPENGL_SETTINGS()
  RENDER_SETUP_AND_RESET_DEPTH_OVERSCAN_OUTPUT(2)
  RENDER_SET_SHADER_OV_C_PANELS_AND_OVERLAY()
  RENDER_BEGIN_SPRITE_RENDERING()

  if (G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_PANELS] > 0)
  {
    RENDER_DRAW_PANELS()
  }

  if (G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_OVERLAY] > 0)
  {
    RENDER_DRAW_OVERLAY()
  }

  RENDER_END_SPRITE_RENDERING()

  /* pass 7 - fade */
  RENDER_FADE_OPENGL_SETTINGS()
  RENDER_SETUP_AND_RESET_DEPTH_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_OV_D_FADE()
  RENDER_BEGIN_FADE_RENDERING()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_FADE_RENDERING()

  /* pass 8 - convert to rgb */
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()
  RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(1)
  RENDER_SET_SHADER_OV_E(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

/*******************************************************************************
** render_postprocessing_linear()
*******************************************************************************/
short int render_postprocessing_linear()
{
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()

  /* pass A (apply settings) */
  RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_UP_A(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass C (linear upscale to window) */
  RENDER_SETUP_AND_CLEAR_WINDOW_OUTPUT()
  RENDER_SET_SHADER_UP_C(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

/*******************************************************************************
** render_postprocessing_pixels()
*******************************************************************************/
short int render_postprocessing_pixels()
{
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()

  /* pass A (apply settings) */
  RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_UP_A(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass B (nearest upscale) */
  RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(1)
  RENDER_SET_SHADER_UP_B(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_NEAREST_RESIZE()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass C (linear upscale) */
  RENDER_SETUP_AND_CLEAR_WINDOW_OUTPUT()
  RENDER_SET_SHADER_UP_C(1)
  RENDER_BEGIN_POSTPROCESSING_NEAREST_RESIZE_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

/*******************************************************************************
** render_postprocessing_scanlines()
*******************************************************************************/
short int render_postprocessing_scanlines()
{
  RENDER_POSTPROCESSING_OPENGL_SETTINGS()

  /* pass A (apply settings) */
  RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(2)
  RENDER_SET_SHADER_UP_A(1)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_OVERSCAN()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass D (cubic horizontal resize) */
  RENDER_SETUP_AND_CLEAR_INTERMEDIATE_TEXTURE_OUTPUT(1)
  RENDER_SET_SHADER_UP_D(2)
  RENDER_BEGIN_POSTPROCESSING_OVERSCAN_TO_CUBIC_RESIZE()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  /* pass E (scanline vertical resize) */
  RENDER_SETUP_AND_CLEAR_WINDOW_OUTPUT()
  RENDER_SET_SHADER_UP_E(1)
  RENDER_BEGIN_POSTPROCESSING_CUBIC_RESIZE_TO_WINDOW()
  RENDER_POSTPROCESSING_DRAW_QUAD()
  RENDER_END_POSTPROCESSING()

  return 0;
}

