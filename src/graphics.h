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
  GRAPHICS_TILE_LAYER_BACKDROP = 0,
  GRAPHICS_TILE_LAYER_SKY,
  GRAPHICS_NUM_TILE_LAYERS
};

enum
{
  GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS = 0,
  GRAPHICS_SPRITE_LAYER_PANELS,
  GRAPHICS_SPRITE_LAYER_OVERLAY,
  GRAPHICS_NUM_SPRITE_LAYERS
};

#define GRAPHICS_OVERSCAN_WIDTH   320
#define GRAPHICS_OVERSCAN_HEIGHT  224

#define GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH   2048
#define GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT  1024

/* the maximum number of tiles is determined as follows:  */
/*   number of backdrop tiles (1 screen): 20 * 14 = 280   */
/*   number of sky tiles (1 screen): 10 * 7 = 70          */
/* however, for the menu backgrounds, the sky gradient    */
/* is in 16x16 tiles, for 20 * 14 = 280 total             */
/* overall, this is 280 + 280 = 560                       */
#define GRAPHICS_MAX_BACKDROP_TILES   280
#define GRAPHICS_MAX_SKY_TILES        280

#define GRAPHICS_MAX_TILES   (GRAPHICS_MAX_BACKDROP_TILES +                    \
                              GRAPHICS_MAX_SKY_TILES)

#define GRAPHICS_BACKDROP_TILES_START_INDEX 0
#define GRAPHICS_BACKDROP_TILES_END_INDEX   ( GRAPHICS_BACKDROP_TILES_START_INDEX + \
                                              GRAPHICS_MAX_BACKDROP_TILES)

#define GRAPHICS_SKY_TILES_START_INDEX      GRAPHICS_BACKDROP_TILES_END_INDEX
#define GRAPHICS_SKY_TILES_END_INDEX        ( GRAPHICS_SKY_TILES_START_INDEX + \
                                              GRAPHICS_MAX_SKY_TILES)

/* the maximum number of sprites is determined as follows:    */
/*   number of 16x16 grid objects (1 screen): 20 * 14 = 280   */
/*     (with 2 grid object layers, this is 2 * 280 = 560)     */
/*   number of things: 80                                     */
/*     each thing can potentially have 1, 2 or 4 sprites (due */
/*     to edge of screen wraparound), so we have 80 * 4 = 320 */
/*   number of panel pieces (1 screen): 20 * 14 = 280         */
/*   number of text characters (1 screen): 40 * 14 = 560      */
/* overall, this is 560 + 320 + 280 + 560 = 1720              */
#define GRAPHICS_MAX_GRID_OBJECTS_AND_THINGS_SPRITES  880
#define GRAPHICS_MAX_PANELS_SPRITES                   280
#define GRAPHICS_MAX_OVERLAY_SPRITES                  560

#define GRAPHICS_MAX_SPRITES  ( GRAPHICS_MAX_GRID_OBJECTS_AND_THINGS_SPRITES + \
                                GRAPHICS_MAX_PANELS_SPRITES +                  \
                                GRAPHICS_MAX_OVERLAY_SPRITES)

#define GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX  0
#define GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_END_INDEX    ( GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX + \
                                                                GRAPHICS_MAX_GRID_OBJECTS_AND_THINGS_SPRITES)

#define GRAPHICS_PANELS_SPRITES_START_INDEX   GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_END_INDEX
#define GRAPHICS_PANELS_SPRITES_END_INDEX     ( GRAPHICS_PANELS_SPRITES_START_INDEX + \
                                                GRAPHICS_MAX_PANELS_SPRITES)

#define GRAPHICS_OVERLAY_SPRITES_START_INDEX  GRAPHICS_PANELS_SPRITES_END_INDEX
#define GRAPHICS_OVERLAY_SPRITES_END_INDEX    ( GRAPHICS_OVERLAY_SPRITES_START_INDEX + \
                                                GRAPHICS_MAX_OVERLAY_SPRITES)

/* the near and far planes are from 0.125 (near) to 1.375 (far)   */
/* the z levels for the backdrop, grid objects, and things are    */
/* in the range of 0.25 to 1.25. this region can be viewed as     */
/* being divided into 8 divisions of 0.125 each:                  */
/* 1.250, 1.125, 1.000, 0.875, 0.750, 0.625, 0.500, 0.375, 0.250  */
/* each has an additional 32 subdivisions each (used for things)  */
#define GRAPHICS_Z_LEVEL_SKY  1.3125f

#define GRAPHICS_Z_LEVEL_BACKDROP           1.250f
#define GRAPHICS_Z_LEVEL_GRID_BACK_OBJECTS  1.000f
#define GRAPHICS_Z_LEVEL_GRID_FRONT_OBJECTS 0.875f
#define GRAPHICS_Z_LEVEL_THINGS             0.750f

/* the z levels for the panels and overlay shouldn't  */
/* matter, as the depth buffer should be reset and    */
/* the depth test should be off when we draw them.    */
#define GRAPHICS_Z_LEVEL_PANELS   0.21875f
#define GRAPHICS_Z_LEVEL_OVERLAY  0.1875f

/* the subdivision step is 0.125 / 32 = 0.00390625 */
#define GRAPHICS_Z_LEVEL_SUBDIVISION_STEP   0.00390625f

/* for the postprocessing, the z level of the fullscreen  */
/* quad is set to the midpoint of the near and far planes */
#define GRAPHICS_Z_LEVEL_FULL_SCREEN_QUAD   0.75f

/* sdl window, various sizes, etc */
extern SDL_Window* G_sdl_window;

extern int G_graphics_resolution;

extern int G_viewport_w;
extern int G_viewport_h;

/* vbo size variables */
extern int G_tile_layer_counts[GRAPHICS_NUM_TILE_LAYERS];
extern int G_sprite_layer_counts[GRAPHICS_NUM_SPRITE_LAYERS];

/* opengl vbo ids */
extern GLuint G_vertex_array_id;

extern GLuint G_vertex_buffer_id_tiles;
extern GLuint G_texture_coord_buffer_id_tiles;
extern GLuint G_lighting_and_palette_buffer_id_tiles;
extern GLuint G_index_buffer_id_tiles;

extern GLuint G_vertex_buffer_id_sprites;
extern GLuint G_texture_coord_buffer_id_sprites;
extern GLuint G_lighting_and_palette_buffer_id_sprites;
extern GLuint G_index_buffer_id_sprites;

/* postprocessing vbo ids */
extern GLuint G_vertex_buffer_id_postprocessing_sky;
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

/* vbos and matrices */
extern GLfloat*         G_vertex_buffer_tiles;
extern GLfloat*         G_texture_coord_buffer_tiles;
extern GLfloat*         G_lighting_and_palette_buffer_tiles;
extern unsigned short*  G_index_buffer_tiles;

extern GLfloat*         G_vertex_buffer_sprites;
extern GLfloat*         G_texture_coord_buffer_sprites;
extern GLfloat*         G_lighting_and_palette_buffer_sprites;
extern unsigned short*  G_index_buffer_sprites;

extern GLfloat          G_vertex_buffer_postprocessing_sky[12];
extern GLfloat          G_vertex_buffer_postprocessing_overscan[12];
extern GLfloat          G_vertex_buffer_postprocessing_nearest_resize[12];
extern GLfloat          G_vertex_buffer_postprocessing_cubic_resize[12];
extern GLfloat          G_vertex_buffer_postprocessing_window[12];

extern GLfloat          G_texture_coord_buffer_postprocessing_overscan[8];
extern GLfloat          G_texture_coord_buffer_postprocessing_nearest_resize[8];
extern GLfloat          G_texture_coord_buffer_postprocessing_cubic_resize[8];

extern unsigned short   G_index_buffer_postprocessing_all[6];

extern GLfloat          G_mvp_matrix_tiles[16];
extern GLfloat          G_mvp_matrix_sprites[16];

extern GLfloat          G_mvp_matrix_overscan[16];
extern GLfloat          G_mvp_matrix_intermediate[16];
extern GLfloat          G_mvp_matrix_window[16];

/* function declarations */
short int graphics_create_opengl_objects();
short int graphics_destroy_opengl_objects();

short int graphics_setup_tiles_mvp_matrix(int t_x, int t_y);
short int graphics_setup_sprites_mvp_matrix(int t_x, int t_y);

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
