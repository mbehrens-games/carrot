/*******************************************************************************
** graphics.h (opengl objects)
*******************************************************************************/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <glad/glad.h>

enum
{
  GRAPHICS_RESOLUTION_480P = 0,
  GRAPHICS_RESOLUTION_720P,
  GRAPHICS_RESOLUTION_768P,
  GRAPHICS_RESOLUTION_1080P,
  GRAPHICS_NUM_RESOLUTIONS
};

enum
{
  GRAPHICS_SHADER_PROGRAM_A = 0,
  GRAPHICS_SHADER_PROGRAM_B,
  GRAPHICS_SHADER_PROGRAM_C,
  GRAPHICS_SHADER_PROGRAM_D,
  GRAPHICS_SHADER_PROGRAM_E,
  GRAPHICS_SHADER_PROGRAM_OV1,
  GRAPHICS_SHADER_PROGRAM_OV2,
  GRAPHICS_NUM_SHADER_PROGRAMS
};

#define GRAPHICS_OVERSCAN_OUTPUT_WIDTH        320
#define GRAPHICS_OVERSCAN_OUTPUT_HEIGHT       224

#define GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH   2048
#define GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT  1024

#define GRAPHICS_PLAY_AREA_WIDTH  320
#define GRAPHICS_PLAY_AREA_HEIGHT 224

#define GRAPHICS_PLAY_AREA_RX     (GRAPHICS_PLAY_AREA_WIDTH / 2)
#define GRAPHICS_PLAY_AREA_RY     (GRAPHICS_PLAY_AREA_HEIGHT / 2)

#define GRAPHICS_SAVED_SPRITE_LAYERS  3

/* the maximum number of tiles is determined as follows:  */
/*   number of backdrop tiles (1 screen): 20 * 14 = 280   */
/*   number of sky tiles (1 screen): 10 * 7 = 70          */
/* however, since the backdrop tiles can be split,        */
/* we should double that amount: 2 * 280 = 560            */
/* overall, this is 560 + 70 = 630                        */
#define GRAPHICS_MAX_TILES    750

/* the maximum number of sprites is determined as follows:    */
/*   number of 16x16 grid objects (1 screen): 20 * 14 = 280   */
/* there are 2 grid object layers, so this is 2 * 280 = 560   */
/*   number of things: 80                                     */
/* each thing can potentially have 1, 2 or 4 sprites          */
/* (because things wrap around when moving off the edge of    */
/* the screen), so we have 80 * 4 = 320                       */
/*   number of hud menu pieces (1 screen): 20 * 14 = 280      */
/*   number of hud panel pieces (1 screen): 20 * 14 = 280     */
/*   number of hud text characters (1 screen): 40 * 14 = 560  */
/* overall, this is 560 + 320 + 280 + 280 + 560 = 2000        */
#define GRAPHICS_MAX_SPRITES  2000

/* z levels from 1.25 (farthest) to 0.25 (nearest)                */
/* the standard divisions are:                                    */
/* 1.250, 1.125, 1.000, 0.875, 0.750, 0.625, 0.500, 0.375, 0.250  */
/* with an additional 32 subdivisions each                        */
/* note that the THINGS level takes up three divisions            */
/* also, the MENU and HUD levels are half-divisions               */
#define GRAPHICS_Z_LEVEL_SKY                  1.250f
#define GRAPHICS_Z_LEVEL_BACKDROP             1.125f
#define GRAPHICS_Z_LEVEL_GRID_BACK_OBJECTS    1.000f
#define GRAPHICS_Z_LEVEL_GRID_FRONT_OBJECTS   0.875f
#define GRAPHICS_Z_LEVEL_THINGS               0.750f
#define GRAPHICS_Z_LEVEL_MENU_BACKGROUND      0.375f
#define GRAPHICS_Z_LEVEL_HUD_PANEL            0.3125f
#define GRAPHICS_Z_LEVEL_HUD_TEXT             0.250f

/* the subdivision step is 0.125 / 32 = 0.00390625 */
#define GRAPHICS_Z_LEVEL_SUBDIVISION_STEP     0.00390625f

/* sdl window, various sizes, etc */
extern SDL_Window* G_sdl_window;

extern int G_graphics_resolution;

extern int G_viewport_w;
extern int G_viewport_h;

/* texture coordinate tables */
extern GLfloat G_texture_coord_table[65];
extern GLfloat G_palette_coord_table[64];

/* vbo size variables */
extern int G_num_tiles;
extern int G_num_sprites;
extern int G_num_saved_sprites[GRAPHICS_SAVED_SPRITE_LAYERS];

/* opengl vbo ids */
extern GLuint G_vertex_array_id;

extern GLuint G_vertex_buffer_id_tiles;
extern GLuint G_texture_coord_buffer_id_tiles;
extern GLuint G_palette_coord_buffer_id_tiles;
extern GLuint G_index_buffer_id_tiles;

extern GLuint G_vertex_buffer_id_sprites;
extern GLuint G_texture_coord_buffer_id_sprites;
extern GLuint G_palette_coord_buffer_id_sprites;
extern GLuint G_index_buffer_id_sprites;

/* postprocessing vbo ids */
extern GLuint G_vertex_buffer_id_postprocessing_overscan;
extern GLuint G_vertex_buffer_id_postprocessing_nearest_resize;
extern GLuint G_vertex_buffer_id_postprocessing_cubic_resize;
extern GLuint G_vertex_buffer_id_postprocessing_window;

extern GLuint G_texture_coord_buffer_id_postprocessing_overscan;
extern GLuint G_texture_coord_buffer_id_postprocessing_nearest_resize;
extern GLuint G_texture_coord_buffer_id_postprocessing_cubic_resize;

extern GLuint G_index_buffer_id_postprocessing_all;

/* opengl intermediate textures */
extern GLuint G_texture_id_intermediate_1;
extern GLuint G_texture_id_intermediate_2;

/* opengl framebuffer ids */
extern GLuint G_framebuffer_id_intermediate_1;
extern GLuint G_renderbuffer_id_intermediate_1;

extern GLuint G_framebuffer_id_intermediate_2;
extern GLuint G_renderbuffer_id_intermediate_2;

/* shader program and uniform ids */
extern GLuint G_program_id_A;
extern GLuint G_program_id_B;
extern GLuint G_program_id_C;
extern GLuint G_program_id_D;
extern GLuint G_program_id_E;
extern GLuint G_program_id_OV1;
extern GLuint G_program_id_OV2;

extern GLuint G_uniform_A_mvp_matrix_id;
extern GLuint G_uniform_A_texture_sampler_id;
extern GLuint G_uniform_A_rgb2yiq_matrix_id;
extern GLuint G_uniform_A_yiq2rgb_matrix_id;
extern GLuint G_uniform_A_black_level_id;
extern GLuint G_uniform_A_white_level_id;

extern GLuint G_uniform_B_mvp_matrix_id;
extern GLuint G_uniform_B_texture_sampler_id;

extern GLuint G_uniform_C_mvp_matrix_id;
extern GLuint G_uniform_C_texture_sampler_id;

extern GLuint G_uniform_D_mvp_matrix_id;
extern GLuint G_uniform_D_texture_sampler_id;
extern GLuint G_uniform_D_cubic_matrix_id;

extern GLuint G_uniform_E_mvp_matrix_id;
extern GLuint G_uniform_E_texture_sampler_id;

extern GLuint G_uniform_OV1_mvp_matrix_id;
extern GLuint G_uniform_OV1_texture_sampler_id;
extern GLuint G_uniform_OV1_palette_sampler_id;

extern GLuint G_uniform_OV2_mvp_matrix_id;
extern GLuint G_uniform_OV2_texture_sampler_id;
extern GLuint G_uniform_OV2_palette_sampler_id;
extern GLuint G_uniform_OV2_fade_amount_id;
extern GLuint G_uniform_OV2_subpalette_size_id;

/* vbos and matrices */
extern GLfloat*         G_vertex_buffer_tiles;
extern GLfloat*         G_texture_coord_buffer_tiles;
extern GLfloat*         G_palette_coord_buffer_tiles;
extern unsigned short*  G_index_buffer_tiles;

extern GLfloat*         G_vertex_buffer_sprites;
extern GLfloat*         G_texture_coord_buffer_sprites;
extern GLfloat*         G_palette_coord_buffer_sprites;
extern unsigned short*  G_index_buffer_sprites;

extern GLfloat          G_vertex_buffer_postprocessing_overscan[12];
extern GLfloat          G_vertex_buffer_postprocessing_nearest_resize[12];
extern GLfloat          G_vertex_buffer_postprocessing_cubic_resize[12];
extern GLfloat          G_vertex_buffer_postprocessing_window[12];

extern GLfloat          G_texture_coord_buffer_postprocessing_overscan[8];
extern GLfloat          G_texture_coord_buffer_postprocessing_nearest_resize[8];
extern GLfloat          G_texture_coord_buffer_postprocessing_cubic_resize[8];

extern unsigned short   G_index_buffer_postprocessing_all[6];

extern GLfloat          G_mvp_matrix_overscan[16];
extern GLfloat          G_mvp_matrix_intermediate[16];
extern GLfloat          G_mvp_matrix_window[16];

/* function declarations */
short int graphics_generate_tables();

short int graphics_load_shaders(char* filename);
short int graphics_create_opengl_objects();
short int graphics_destroy_opengl_objects();

short int graphics_setup_overscan_mvp_matrix(int t_x, int t_y);

short int graphics_set_graphics_resolution(int resolution);
short int graphics_read_desktop_dimensions();

short int graphics_setup_viewport_mvp_matrix_and_buffers();

short int graphics_set_window_size(int res);
short int graphics_increase_window_size();
short int graphics_decrease_window_size();

short int graphics_initialize_fullscreen(int flag);
short int graphics_toggle_fullscreen();

short int graphics_initialize_vsync(int flag);
short int graphics_toggle_vsync();

#endif
