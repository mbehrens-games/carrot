/*******************************************************************************
** vbsprite.c (vbo population functions - sprites)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "animate.h"
#include "color.h"
#include "global.h"
#include "graphics.h"
#include "grid.h"
#include "palette.h"
#include "state.h"
#include "texture.h"
#include "thing.h"
#include "vbsprite.h"
#include "world.h"

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_NORMAL(pos_x, pos_y, z)           \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_ROTATE_90(pos_x, pos_y, z)        \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_ROTATE_180(pos_x, pos_y, z)       \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_ROTATE_270(pos_x, pos_y, z)       \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_FLIP_HORI(pos_x, pos_y, z)        \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_FLIP_VERT(pos_x, pos_y, z)        \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_FLIP_AND_ROTATE_90(pos_x, pos_y, z)  \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x - 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y - 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                           \
                                                                                  \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x - 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y + 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                           \
                                                                                  \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x + 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y - 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                           \
                                                                                  \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x + 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y + 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_FLIP_AND_ROTATE_270(pos_x, pos_y, z) \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x + 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y + 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                           \
                                                                                  \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x + 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y - 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                           \
                                                                                  \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x - 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y + 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                           \
                                                                                  \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x - 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y - 8;                   \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

/* for the bunny sprites (16x24), the position is lower   */
/* than the center when the sprite is right side up, and  */
/* higher than the center when the sprite is upside down  */
#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X24_NORMAL(pos_x, pos_y, z)           \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y - 16;               \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y - 16;               \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X24_FLIP_HORI(pos_x, pos_y, z)        \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y - 16;               \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y - 16;               \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X24_FLIP_VERT(pos_x, pos_y, z)        \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y + 16;               \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y + 16;               \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X24_FLIP_BOTH(pos_x, pos_y, z)        \
  G_vertex_buffer_sprites[12 * sprite_index + 0]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 1]   = pos_y + 16;               \
  G_vertex_buffer_sprites[12 * sprite_index + 2]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 3]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 4]   = pos_y + 16;               \
  G_vertex_buffer_sprites[12 * sprite_index + 5]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 6]   = pos_x + 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 7]   = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 8]   = z;                        \
                                                                               \
  G_vertex_buffer_sprites[12 * sprite_index + 9]   = pos_x - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 10]  = pos_y - 8;                \
  G_vertex_buffer_sprites[12 * sprite_index + 11]  = z;

#define VB_SPRITE_ADD_TO_TEXTURE_COORD_BUFFER_16X16(ss_x, ss_y)                                     \
  G_texture_coord_buffer_sprites[8 * sprite_index + 0] = G_texture_coord_table[2 * ss_x];           \
  G_texture_coord_buffer_sprites[8 * sprite_index + 1] = G_texture_coord_table[2 * (32 - ss_y)];    \
                                                                                                    \
  G_texture_coord_buffer_sprites[8 * sprite_index + 2] = G_texture_coord_table[2 * (ss_x + 1)];     \
  G_texture_coord_buffer_sprites[8 * sprite_index + 3] = G_texture_coord_table[2 * (32 - ss_y)];    \
                                                                                                    \
  G_texture_coord_buffer_sprites[8 * sprite_index + 4] = G_texture_coord_table[2 * ss_x];           \
  G_texture_coord_buffer_sprites[8 * sprite_index + 5] = G_texture_coord_table[2 * (31 - ss_y)];    \
                                                                                                    \
  G_texture_coord_buffer_sprites[8 * sprite_index + 6] = G_texture_coord_table[2 * (ss_x + 1)];     \
  G_texture_coord_buffer_sprites[8 * sprite_index + 7] = G_texture_coord_table[2 * (31 - ss_y)];

/* the cell is the top-left 8x8 cell on the 16x24 sprite */
#define VB_SPRITE_ADD_TO_TEXTURE_COORD_BUFFER_16X24(cell_x, cell_y)                             \
  G_texture_coord_buffer_sprites[8 * sprite_index + 0] = G_texture_coord_table[cell_x];         \
  G_texture_coord_buffer_sprites[8 * sprite_index + 1] = G_texture_coord_table[64 - cell_y];    \
                                                                                                \
  G_texture_coord_buffer_sprites[8 * sprite_index + 2] = G_texture_coord_table[cell_x + 2];     \
  G_texture_coord_buffer_sprites[8 * sprite_index + 3] = G_texture_coord_table[64 - cell_y];    \
                                                                                                \
  G_texture_coord_buffer_sprites[8 * sprite_index + 4] = G_texture_coord_table[cell_x];         \
  G_texture_coord_buffer_sprites[8 * sprite_index + 5] = G_texture_coord_table[61 - cell_y];    \
                                                                                                \
  G_texture_coord_buffer_sprites[8 * sprite_index + 6] = G_texture_coord_table[cell_x + 2];     \
  G_texture_coord_buffer_sprites[8 * sprite_index + 7] = G_texture_coord_table[61 - cell_y];

#define VB_SPRITE_ADD_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                 \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 0] = G_lighting_coord_table[8 + lighting];   \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 1] = G_palette_coord_table[palette];         \
                                                                                                        \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 2] = G_lighting_coord_table[8 + lighting];   \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 3] = G_palette_coord_table[palette];         \
                                                                                                        \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 4] = G_lighting_coord_table[8 + lighting];   \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 5] = G_palette_coord_table[palette];         \
                                                                                                        \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 6] = G_lighting_coord_table[8 + lighting];   \
  G_lighting_and_palette_buffer_sprites[8 * sprite_index + 7] = G_palette_coord_table[palette];

#define VB_SPRITE_ADD_TO_ELEMENT_BUFFER()                                      \
  G_index_buffer_sprites[6 * sprite_index + 0] = 4 * sprite_index + 0;         \
  G_index_buffer_sprites[6 * sprite_index + 1] = 4 * sprite_index + 2;         \
  G_index_buffer_sprites[6 * sprite_index + 2] = 4 * sprite_index + 1;         \
                                                                               \
  G_index_buffer_sprites[6 * sprite_index + 3] = 4 * sprite_index + 1;         \
  G_index_buffer_sprites[6 * sprite_index + 4] = 4 * sprite_index + 2;         \
  G_index_buffer_sprites[6 * sprite_index + 5] = 4 * sprite_index + 3;

#define VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, mode)  \
  if (sprite_index < GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_END_INDEX)                            \
  {                                                                                                 \
    VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_##mode(pos_x, pos_y, z)                                    \
    VB_SPRITE_ADD_TO_TEXTURE_COORD_BUFFER_16X16(ss_x, ss_y)                                         \
    VB_SPRITE_ADD_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                                 \
    VB_SPRITE_ADD_TO_ELEMENT_BUFFER()                                                               \
                                                                                                    \
    sprite_index += 1;                                                                              \
  }

#define VB_SPRITE_ADD_THING_TO_BUFFERS(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, mode)  \
  if (sprite_index < GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_END_INDEX)                      \
  {                                                                                           \
    VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X16_##mode(pos_x, pos_y, z)                              \
    VB_SPRITE_ADD_TO_TEXTURE_COORD_BUFFER_16X16(ss_x, ss_y)                                   \
    VB_SPRITE_ADD_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                           \
    VB_SPRITE_ADD_TO_ELEMENT_BUFFER()                                                         \
                                                                                              \
    sprite_index += 1;                                                                        \
  }

#define VB_SPRITE_ADD_THING_WITH_WRAPAROUND(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, mode) \
  /* left / right edge of screen wraparound */                                                    \
  if (pos_x - 8 < 0)                                                                              \
  {                                                                                               \
    VB_SPRITE_ADD_THING_TO_BUFFERS( (pos_x + WORLD_WIDTH_IN_PIXELS), pos_y,                       \
                                    z, ss_x, ss_y, lighting, palette, mode)                       \
  }                                                                                               \
  else if (pos_x + 8 >= WORLD_WIDTH_IN_PIXELS)                                                    \
  {                                                                                               \
    VB_SPRITE_ADD_THING_TO_BUFFERS( (pos_x - WORLD_WIDTH_IN_PIXELS), pos_y,                       \
                                    z, ss_x, ss_y, lighting, palette, mode)                       \
  }                                                                                               \
                                                                                                  \
  /* top / bottom edge of screen wraparound */                                                    \
  if (pos_y - 8 < 0)                                                                              \
  {                                                                                               \
    VB_SPRITE_ADD_THING_TO_BUFFERS( pos_x, (pos_y + WORLD_HEIGHT_IN_PIXELS),                      \
                                    z, ss_x, ss_y, lighting, palette, mode)                       \
  }                                                                                               \
  else if (pos_y + 8 >= WORLD_HEIGHT_IN_PIXELS)                                                   \
  {                                                                                               \
    VB_SPRITE_ADD_THING_TO_BUFFERS( pos_x, (pos_y - WORLD_HEIGHT_IN_PIXELS),                      \
                                    z, ss_x, ss_y, lighting, palette, mode)                       \
  }                                                                                               \
                                                                                                  \
  /* corner of screen wraparound */                                                               \
  if ((pos_x - 8 < 0) && (pos_y - 8 < 0))                                                         \
  {                                                                                               \
    VB_SPRITE_ADD_THING_TO_BUFFERS( (pos_x + WORLD_WIDTH_IN_PIXELS),                              \
                                    (pos_y + WORLD_HEIGHT_IN_PIXELS),                             \
                                    z, ss_x, ss_y, lighting, palette, mode)                       \
  }                                                                                               \
  else if ((pos_x + 8 >= WORLD_WIDTH_IN_PIXELS) && (pos_y - 8 < 0))                               \
  {                                                                                               \
    VB_SPRITE_ADD_THING_TO_BUFFERS( (pos_x - WORLD_WIDTH_IN_PIXELS),                              \
                                    (pos_y + WORLD_HEIGHT_IN_PIXELS),                             \
                                    z, ss_x, ss_y, lighting, palette, mode)                       \
  }                                                                                               \
  else if ((pos_x - 8 < 0) && (pos_y + 8 >= WORLD_HEIGHT_IN_PIXELS))                              \
  {                                                                                               \
    VB_SPRITE_ADD_THING_TO_BUFFERS( (pos_x + WORLD_WIDTH_IN_PIXELS),                              \
                                    (pos_y - WORLD_HEIGHT_IN_PIXELS),                             \
                                    z, ss_x, ss_y, lighting, palette, mode)                       \
  }                                                                                               \
  else if ((pos_x + 8 >= WORLD_WIDTH_IN_PIXELS) && (pos_y + 8 >= WORLD_HEIGHT_IN_PIXELS))         \
  {                                                                                               \
    VB_SPRITE_ADD_THING_TO_BUFFERS( (pos_x - WORLD_WIDTH_IN_PIXELS),                              \
                                    (pos_y - WORLD_HEIGHT_IN_PIXELS),                             \
                                    z, ss_x, ss_y, lighting, palette, mode)                       \
  }                                                                                               \
                                                                                                  \
  /* the actual sprite! */                                                                        \
  VB_SPRITE_ADD_THING_TO_BUFFERS(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, mode)

#define VB_SPRITE_ADD_BUNNY_TO_BUFFERS(pos_x, pos_y, z, cell_x, cell_y, lighting, palette, mode)  \
  if (sprite_index < GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_END_INDEX)                          \
  {                                                                                               \
    VB_SPRITE_ADD_TO_VERTEX_BUFFER_16X24_##mode(pos_x, pos_y, z)                                  \
    VB_SPRITE_ADD_TO_TEXTURE_COORD_BUFFER_16X24(cell_x, cell_y)                                   \
    VB_SPRITE_ADD_TO_LIGHTING_AND_PALETTE_BUFFER(lighting, palette)                               \
    VB_SPRITE_ADD_TO_ELEMENT_BUFFER()                                                             \
                                                                                                  \
    sprite_index += 1;                                                                            \
  }

#define VB_SPRITE_ADD_BUNNY_WITH_WRAPAROUND_NORMAL(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, mode)  \
  /* left / right edge of screen wraparound */                                                            \
  if (pos_x - 8 < 0)                                                                                      \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x + WORLD_WIDTH_IN_PIXELS), pos_y,                               \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if (pos_x + 8 >= WORLD_WIDTH_IN_PIXELS)                                                            \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x - WORLD_WIDTH_IN_PIXELS), pos_y,                               \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
                                                                                                          \
  /* top / bottom edge of screen wraparound */                                                            \
  if (pos_y - 16 < 0)                                                                                     \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( pos_x, (pos_y + WORLD_HEIGHT_IN_PIXELS),                              \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if (pos_y + 8 >= WORLD_HEIGHT_IN_PIXELS)                                                           \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( pos_x, (pos_y - WORLD_HEIGHT_IN_PIXELS),                              \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
                                                                                                          \
  /* corner of screen wraparound */                                                                       \
  if ((pos_x - 8 < 0) && (pos_y - 16 < 0))                                                                \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x + WORLD_WIDTH_IN_PIXELS),                                      \
                                    (pos_y + WORLD_HEIGHT_IN_PIXELS),                                     \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if ((pos_x + 8 >= WORLD_WIDTH_IN_PIXELS) && (pos_y - 16 < 0))                                      \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x - WORLD_WIDTH_IN_PIXELS),                                      \
                                    (pos_y + WORLD_HEIGHT_IN_PIXELS),                                     \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if ((pos_x - 8 < 0) && (pos_y + 8 >= WORLD_HEIGHT_IN_PIXELS))                                      \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x + WORLD_WIDTH_IN_PIXELS),                                      \
                                    (pos_y - WORLD_HEIGHT_IN_PIXELS),                                     \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if ((pos_x + 8 >= WORLD_WIDTH_IN_PIXELS) && (pos_y + 8 >= WORLD_HEIGHT_IN_PIXELS))                 \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x - WORLD_WIDTH_IN_PIXELS),                                      \
                                    (pos_y - WORLD_HEIGHT_IN_PIXELS),                                     \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
                                                                                                          \
  /* the actual sprite! */                                                                                \
  VB_SPRITE_ADD_BUNNY_TO_BUFFERS(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, mode)

#define VB_SPRITE_ADD_BUNNY_WITH_WRAPAROUND_FLIPPED(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, mode) \
  /* left / right edge of screen wraparound */                                                            \
  if (pos_x - 8 < 0)                                                                                      \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x + WORLD_WIDTH_IN_PIXELS), pos_y,                               \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if (pos_x + 8 >= WORLD_WIDTH_IN_PIXELS)                                                            \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x - WORLD_WIDTH_IN_PIXELS), pos_y,                               \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
                                                                                                          \
  /* top / bottom edge of screen wraparound */                                                            \
  if (pos_y - 8 < 0)                                                                                      \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( pos_x, (pos_y + WORLD_HEIGHT_IN_PIXELS),                              \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if (pos_y + 16 >= WORLD_HEIGHT_IN_PIXELS)                                                          \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( pos_x, (pos_y - WORLD_HEIGHT_IN_PIXELS),                              \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
                                                                                                          \
  /* corner of screen wraparound */                                                                       \
  if ((pos_x - 8 < 0) && (pos_y - 8 < 0))                                                                 \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x + WORLD_WIDTH_IN_PIXELS),                                      \
                                    (pos_y + WORLD_HEIGHT_IN_PIXELS),                                     \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if ((pos_x + 8 >= WORLD_WIDTH_IN_PIXELS) && (pos_y - 8 < 0))                                       \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x - WORLD_WIDTH_IN_PIXELS),                                      \
                                    (pos_y + WORLD_HEIGHT_IN_PIXELS),                                     \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if ((pos_x - 8 < 0) && (pos_y + 16 >= WORLD_HEIGHT_IN_PIXELS))                                     \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x + WORLD_WIDTH_IN_PIXELS),                                      \
                                    (pos_y - WORLD_HEIGHT_IN_PIXELS),                                     \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
  else if ((pos_x + 8 >= WORLD_WIDTH_IN_PIXELS) && (pos_y + 16 >= WORLD_HEIGHT_IN_PIXELS))                \
  {                                                                                                       \
    VB_SPRITE_ADD_BUNNY_TO_BUFFERS( (pos_x - WORLD_WIDTH_IN_PIXELS),                                      \
                                    (pos_y - WORLD_HEIGHT_IN_PIXELS),                                     \
                                    z, ss_x, ss_y, lighting, palette, mode)                               \
  }                                                                                                       \
                                                                                                          \
  /* the actual sprite! */                                                                                \
  VB_SPRITE_ADD_BUNNY_TO_BUFFERS(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, mode)

#define VB_SPRITE_UPDATE_GRID_OBJECTS_AND_THINGS_SPRITES_IN_VBOS()                                                    \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);                                                          \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                                    \
                  GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX * 12 * sizeof(GLfloat),                        \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS] * 12 * sizeof(GLfloat),        \
                  &G_vertex_buffer_sprites[GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX * 12]);               \
                                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);                                                   \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                                    \
                  GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                         \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS] * 8 * sizeof(GLfloat),         \
                  &G_texture_coord_buffer_sprites[GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX * 8]);         \
                                                                                                                      \
  glBindBuffer(GL_ARRAY_BUFFER, G_lighting_and_palette_buffer_id_sprites);                                            \
  glBufferSubData(GL_ARRAY_BUFFER,                                                                                    \
                  GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX * 8 * sizeof(GLfloat),                         \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS] * 8 * sizeof(GLfloat),         \
                  &G_lighting_and_palette_buffer_sprites[GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX * 8]);  \
                                                                                                                      \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);                                                   \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,                                                                            \
                  GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX * 6 * sizeof(unsigned short),                  \
                  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS] * 6 * sizeof(unsigned short),  \
                  &G_index_buffer_sprites[GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX * 6]);

/*******************************************************************************
** vb_sprite_load_grid_objects_and_things()
*******************************************************************************/
short int vb_sprite_load_grid_objects_and_things()
{
  int k;

  int m;
  int n;

  grid_box* b;

  thing* t;

  int sprite_index;

  int pos_x;
  int pos_y;

  int ss_x;
  int ss_y;

  int lighting;
  int palette;

  float z;

  unsigned int adjusted_timer_count;

  /* reset sprite vbo counts */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS] = 0;

  /* load back objects */
  sprite_index = GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX;

  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b = &G_collision_grid[(n * GRID_WIDTH) + m];

      if (b->back_object == GRID_OBJECT_NONE)
        continue;

      /* determine position */
      pos_x = GRID_BOX_SIZE * m + GRID_BOX_SIZE_HALF;
      pos_y = GRID_BOX_SIZE * n + GRID_BOX_SIZE_HALF;

      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + G_collision_grid[(n * GRID_WIDTH) + m].back_timer_offset) % 240;

      /* determine texture coordinates, lighting, and palette */
      if (b->back_object == GRID_OBJECT_LADDER)
      {
        ss_x = 21;
        ss_y = 27;

        lighting = 0;
        palette = 6;
      }
      else if (b->back_object == GRID_OBJECT_DOOR_BLOCK)
      {
        if (b->back_state == STATE_DOOR_BLOCK_OPENING)
        {
          if (ANIM_CURRENT_FRAME(DOOR_OPENING) == 0)
            ss_x = 28;
          else if (ANIM_CURRENT_FRAME(DOOR_OPENING) == 1)
            ss_x = 29;
          else if (ANIM_CURRENT_FRAME(DOOR_OPENING) == 2)
            ss_x = 30;
          else
            ss_x = 31;
        }
        else
          ss_x = 27;

        ss_y = 28;

        lighting = 0;

        if (b->back_color == COLOR_ORANGE)
          palette = 0;
        else if (b->back_color == COLOR_PINK)
          palette = 1;
        else if (b->back_color == COLOR_BLUE)
          palette = 3;
        else if (b->back_color == COLOR_GREEN)
          palette = 4;
        else
          palette = 0;
      }
      else if (b->back_object == GRID_OBJECT_INVISIBLE_BLOCK)
      {
        ss_x = 27;
        ss_y = 27;

        lighting = 0;

        if (b->back_color == COLOR_ORANGE)
          palette = 0;
        else if (b->back_color == COLOR_PINK)
          palette = 1;
        else if (b->back_color == COLOR_BLUE)
          palette = 3;
        else if (b->back_color == COLOR_GREEN)
          palette = 4;
        else
          palette = 0;
      }
      else if ( (b->back_object == GRID_OBJECT_ARROWS_000) || 
                (b->back_object == GRID_OBJECT_ARROWS_090) || 
                (b->back_object == GRID_OBJECT_ARROWS_180) || 
                (b->back_object == GRID_OBJECT_ARROWS_270))
      {
        if (ANIM_CURRENT_FRAME(ARROWS) == 0)
          ss_x = 23;
        else if (ANIM_CURRENT_FRAME(ARROWS) == 1)
          ss_x = 24;
        else if (ANIM_CURRENT_FRAME(ARROWS) == 2)
          ss_x = 25;
        else
          ss_x = 26;

        ss_y = 28;

        lighting = 0;
        palette = 0;
      }
      else if ( (b->back_object == GRID_OBJECT_ARROWS_045) || 
                (b->back_object == GRID_OBJECT_ARROWS_135) || 
                (b->back_object == GRID_OBJECT_ARROWS_225) || 
                (b->back_object == GRID_OBJECT_ARROWS_315))
      {
        if (ANIM_CURRENT_FRAME(ARROWS) == 0)
          ss_x = 23;
        else if (ANIM_CURRENT_FRAME(ARROWS) == 1)
          ss_x = 24;
        else if (ANIM_CURRENT_FRAME(ARROWS) == 2)
          ss_x = 25;
        else
          ss_x = 26;

        ss_y = 29;

        lighting = 0;
        palette = 0;
      }
      else
        continue;

      /* add this sprite to the buffers */
      if (b->back_object == GRID_OBJECT_ARROWS_090)
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_BACK_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, ROTATE_90)
      }
      else if (b->back_object == GRID_OBJECT_ARROWS_180)
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_BACK_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, FLIP_HORI)
      }
      else if (b->back_object == GRID_OBJECT_ARROWS_270)
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_BACK_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, FLIP_AND_ROTATE_90)
      }
      else if (b->back_object == GRID_OBJECT_ARROWS_135)
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_BACK_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, ROTATE_90)
      }
      else if (b->back_object == GRID_OBJECT_ARROWS_225)
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_BACK_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, ROTATE_180)
      }
      else if (b->back_object == GRID_OBJECT_ARROWS_315)
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_BACK_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, ROTATE_270)
      }
      else
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_BACK_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, NORMAL)
      }
    }
  }

  /* load front objects */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b = &G_collision_grid[(n * GRID_WIDTH) + m];

      if (b->front_object == GRID_OBJECT_NONE)
        continue;

      /* determine position */
      pos_x = GRID_BOX_SIZE * m + GRID_BOX_SIZE_HALF;
      pos_y = GRID_BOX_SIZE * n + GRID_BOX_SIZE_HALF;

      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + G_collision_grid[(n * GRID_WIDTH) + m].front_timer_offset) % 240;

      /* determine texture coordinates, lighting, and palette */
      if (b->front_object == GRID_OBJECT_MARBLE)
      {
        if (b->front_state == STATE_MARBLE_CHANGING)
        {
          ss_x = 22;
          ss_y = 27;

          if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 0)
            lighting = -2;
          else if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 1)
            lighting = -4;
          else if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 2)
            lighting = -6;
          else if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 3)
            lighting = -4;
          else if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 4)
            lighting = -2;
          else
            lighting = 0;

          palette = 7;
        }
        else
        {
          if (b->front_color == COLOR_WILD)
          {
            if (ANIM_CURRENT_FRAME(WILD_CARD) == 0)
              ss_x = 23;
            else if (ANIM_CURRENT_FRAME(WILD_CARD) == 1)
              ss_x = 24;
            else if (ANIM_CURRENT_FRAME(WILD_CARD) == 2)
              ss_x = 25;
            else
              ss_x = 26;
          }
          else
            ss_x = 22;

          ss_y = 27;

          if (b->front_state == STATE_MARBLE_FLASHING)
          {
            if (ANIM_CURRENT_FRAME(MARBLE_FLASHING) == 0)
              lighting = 2;
            else if (ANIM_CURRENT_FRAME(MARBLE_FLASHING) == 1)
              lighting = 4;
            else if (ANIM_CURRENT_FRAME(MARBLE_FLASHING) == 2)
              lighting = 6;
            else
              lighting = 4;
          }
          else
            lighting = 0;

          if (b->front_color == COLOR_METAL)
            palette = 6;
          else if (b->front_color == COLOR_ORANGE)
            palette = 0;
          else if (b->front_color == COLOR_PINK)
            palette = 1;
          else if (b->front_color == COLOR_BLUE)
            palette = 3;
          else if (b->front_color == COLOR_GREEN)
            palette = 4;
          else if (b->front_color == COLOR_WILD)
            palette = 0;
          else
            palette = 0;
        }
      }
      else if (b->front_object == GRID_OBJECT_ICE_SINGLE)
      {
        ss_x = 13;

        if (ANIM_CURRENT_FRAME(ICE) == 0)
          ss_y = 13;
        else if (ANIM_CURRENT_FRAME(ICE) == 1)
          ss_y = 14;
        else if (ANIM_CURRENT_FRAME(ICE) == 2)
          ss_y = 15;
        else
          ss_y = 12;

        lighting = 0;
        palette = 0;
      }
      else if (b->front_object == GRID_OBJECT_ICE_LEFT)
      {
        ss_x = 14;

        if (ANIM_CURRENT_FRAME(ICE) == 0)
          ss_y = 13;
        else if (ANIM_CURRENT_FRAME(ICE) == 1)
          ss_y = 14;
        else if (ANIM_CURRENT_FRAME(ICE) == 2)
          ss_y = 15;
        else
          ss_y = 12;

        lighting = 0;
        palette = 0;
      }
      else if (b->front_object == GRID_OBJECT_ICE_RIGHT)
      {
        ss_x = 15;

        if (ANIM_CURRENT_FRAME(ICE) == 0)
          ss_y = 13;
        else if (ANIM_CURRENT_FRAME(ICE) == 1)
          ss_y = 14;
        else if (ANIM_CURRENT_FRAME(ICE) == 2)
          ss_y = 15;
        else
          ss_y = 12;

        lighting = 0;
        palette = 0;
      }
      else if (b->front_object == GRID_OBJECT_HEART_BLOCK)
      {
        if (b->front_state == STATE_HEART_BLOCK_FLASHING_TOP)
        {
          if (ANIM_CURRENT_FRAME(HEART_BLOCK) == 0)
            ss_x = 17;
          else
            ss_x = 16;
        }
        else if (b->front_state == STATE_HEART_BLOCK_FLASHING_BOTTOM)
        {
          if (ANIM_CURRENT_FRAME(HEART_BLOCK) == 0)
            ss_x = 18;
          else
            ss_x = 16;
        }
        else
          ss_x = 16;

        ss_y = 25;

        lighting = 0;
        palette = 0;
      }
      else if (b->front_object == GRID_OBJECT_DIAMOND_BLOCK)
      {
        if (b->front_state == STATE_DIAMOND_BLOCK_FLASHING_TOP)
        {
          if (ANIM_CURRENT_FRAME(DIAMOND_BLOCK) == 0)
            ss_x = 29;
          else
            ss_x = 28;
        }
        else if (b->front_state == STATE_DIAMOND_BLOCK_FLASHING_BOTTOM)
        {
          if (ANIM_CURRENT_FRAME(DIAMOND_BLOCK) == 0)
            ss_x = 31;
          else
            ss_x = 30;
        }
        else
          ss_x = 27;

        ss_y = 31;

        lighting = 0;
        palette = 0;
      }
      else if (b->front_object == GRID_OBJECT_MAGI_BLOCK_1)
      {
        if (ANIM_CURRENT_FRAME(MAGI_BLOCK_1) == 0)
          ss_x = 24;
        else if (ANIM_CURRENT_FRAME(MAGI_BLOCK_1) == 1)
          ss_x = 25;
        else if (ANIM_CURRENT_FRAME(MAGI_BLOCK_1) == 2)
          ss_x = 26;
        else
          ss_x = 23;

        ss_y = 30;

        lighting = 0;
        palette = 0;
      }
      else if (b->front_object == GRID_OBJECT_MAGI_BLOCK_2)
      {
        if (ANIM_CURRENT_FRAME(MAGI_BLOCK_2) == 0)
          ss_x = 24;
        else if (ANIM_CURRENT_FRAME(MAGI_BLOCK_2) == 1)
          ss_x = 25;
        else if (ANIM_CURRENT_FRAME(MAGI_BLOCK_2) == 2)
          ss_x = 26;
        else
          ss_x = 23;

        ss_y = 30;

        lighting = 0;
        palette = 0;
      }
      else if (b->front_object == GRID_OBJECT_FAST_MAGI_BLOCK_1)
      {
        if (ANIM_CURRENT_FRAME(FAST_MAGI_BLOCK_1) == 0)
          ss_x = 24;
        else if (ANIM_CURRENT_FRAME(FAST_MAGI_BLOCK_1) == 1)
          ss_x = 25;
        else if (ANIM_CURRENT_FRAME(FAST_MAGI_BLOCK_1) == 2)
          ss_x = 26;
        else
          ss_x = 23;

        ss_y = 30;

        lighting = 0;
        palette = 3;
      }
      else if (b->front_object == GRID_OBJECT_FAST_MAGI_BLOCK_2)
      {
        if (ANIM_CURRENT_FRAME(FAST_MAGI_BLOCK_2) == 0)
          ss_x = 24;
        else if (ANIM_CURRENT_FRAME(FAST_MAGI_BLOCK_2) == 1)
          ss_x = 25;
        else if (ANIM_CURRENT_FRAME(FAST_MAGI_BLOCK_2) == 2)
          ss_x = 26;
        else
          ss_x = 23;

        ss_y = 30;

        lighting = 0;
        palette = 3;
      }
      else if (b->front_object == GRID_OBJECT_DOOR_BLOCK)
      {
        ss_x = 27;
        ss_y = 28;

        lighting = 0;

        if (b->front_color == COLOR_ORANGE)
          palette = 0;
        else if (b->front_color == COLOR_PINK)
          palette = 1;
        else if (b->front_color == COLOR_BLUE)
          palette = 3;
        else if (b->front_color == COLOR_GREEN)
          palette = 4;
        else
          palette = 0;
      }
      else if (b->front_object == GRID_OBJECT_VISIBLE_BLOCK)
      {
        if (b->front_state == STATE_VISIBLE_BLOCK_APPEARING)
        {
          if (ANIM_CURRENT_FRAME(INVIS_APPEARING) == 0)
            ss_x = 28;
          else if (ANIM_CURRENT_FRAME(INVIS_APPEARING) == 1)
            ss_x = 29;
          else
            ss_x = 30;
        }
        else
          ss_x = 31;

        ss_y = 27;

        lighting = 0;

        if (b->front_color == COLOR_ORANGE)
          palette = 0;
        else if (b->front_color == COLOR_PINK)
          palette = 1;
        else if (b->front_color == COLOR_BLUE)
          palette = 3;
        else if (b->front_color == COLOR_GREEN)
          palette = 4;
        else
          palette = 0;
      }
      else if ( (b->front_object == GRID_OBJECT_BUBBLE_POT_UP)    || 
                (b->front_object == GRID_OBJECT_BUBBLE_POT_RIGHT) || 
                (b->front_object == GRID_OBJECT_BUBBLE_POT_DOWN)  || 
                (b->front_object == GRID_OBJECT_BUBBLE_POT_LEFT))
      {
        if (ANIM_CURRENT_FRAME(BUBBLE_POT) == 0)
          ss_x = 24;
        else if (ANIM_CURRENT_FRAME(BUBBLE_POT) == 1)
          ss_x = 25;
        else if (ANIM_CURRENT_FRAME(BUBBLE_POT) == 2)
          ss_x = 26;
        else if (ANIM_CURRENT_FRAME(BUBBLE_POT) == 3)
          ss_x = 25;
        else if (ANIM_CURRENT_FRAME(BUBBLE_POT) == 4)
          ss_x = 24;
        else
          ss_x = 23;

        ss_y = 31;

        lighting = 0;
        palette = 0;
      }
      else
        continue;

      /* add this sprite to the buffers */
      if (b->front_object == GRID_OBJECT_BUBBLE_POT_RIGHT)
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_FRONT_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, ROTATE_270)
      }
      else if (b->front_object == GRID_OBJECT_BUBBLE_POT_DOWN)
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_FRONT_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, FLIP_VERT)
      }
      else if (b->front_object == GRID_OBJECT_BUBBLE_POT_LEFT)
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_FRONT_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, FLIP_AND_ROTATE_90)
      }
      else
      {
        VB_SPRITE_ADD_GRID_OBJECT_TO_BUFFERS( pos_x, pos_y, GRAPHICS_Z_LEVEL_GRID_FRONT_OBJECTS, 
                                              ss_x, ss_y, lighting, palette, NORMAL)
      }
    }
  }

  /* load things */
  for (k = 0; k < WORLD_MAX_THINGS; k++)
  {
    t = &G_world_all_things[k];

    if (t->type == THING_TYPE_NONE)
      continue;

    /* determine position */
    pos_x = (t->pos_x + THING_NUM_SUBPIXELS_HALF) / THING_NUM_SUBPIXELS;
    pos_y = (t->pos_y + THING_NUM_SUBPIXELS_HALF) / THING_NUM_SUBPIXELS;

    /* determine adjusted timer count */
    adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

    /* determine texture coordinates, lighting, and palette */
    /* bunny */
    if (t->type == THING_TYPE_BUNNY)
    {
      if (G_bunny_action == BUNNY_ACTION_WALKING)
      {
        if (ANIM_CURRENT_FRAME(BUNNY_WALKING) == 0)
          ss_x = 44;
        else if (ANIM_CURRENT_FRAME(BUNNY_WALKING) == 1)
          ss_x = 42;
        else if (ANIM_CURRENT_FRAME(BUNNY_WALKING) == 2)
          ss_x = 46;
        else
          ss_x = 42;

        ss_y = 48;
      }
      else if ( (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
                (G_bunny_action == BUNNY_ACTION_DECELERATING))
      {
        ss_x = 42;
        ss_y = 48;
      }
      else if ( (G_bunny_action == BUNNY_ACTION_HOPPING)  || 
                (G_bunny_action == BUNNY_ACTION_JUMPING)  || 
                (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE))
      {
        ss_x = 48;
        ss_y = 48;
      }
      else if ( (G_bunny_action == BUNNY_ACTION_FALLING) || 
                (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
      {
        ss_x = 50;
        ss_y = 48;
      }
      else if (G_bunny_action == BUNNY_ACTION_GRABBING)
      {
        if (ANIM_CURRENT_FRAME(BUNNY_GRABBING) == 0)
          ss_x = 52;
        else if (ANIM_CURRENT_FRAME(BUNNY_GRABBING) == 1)
          ss_x = 54;
        else if (ANIM_CURRENT_FRAME(BUNNY_GRABBING) == 2)
          ss_x = 52;
        else
          ss_x = 56;

        ss_y = 51;
      }
      else if (G_bunny_action == BUNNY_ACTION_THROWING)
      {
        if (ANIM_CURRENT_FRAME(BUNNY_THROWING) == 0)
          ss_x = 52;
        else if (ANIM_CURRENT_FRAME(BUNNY_THROWING) == 1)
          ss_x = 54;
        else if (ANIM_CURRENT_FRAME(BUNNY_THROWING) == 2)
          ss_x = 52;
        else
          ss_x = 56;

        ss_y = 48;
      }
      else if (G_bunny_action == BUNNY_ACTION_KICKING)
      {
        if (ANIM_CURRENT_FRAME(BUNNY_KICKING) == 0)
          ss_x = 58;
        else if (ANIM_CURRENT_FRAME(BUNNY_KICKING) == 1)
          ss_x = 60;
        else if (ANIM_CURRENT_FRAME(BUNNY_KICKING) == 2)
          ss_x = 58;
        else
          ss_x = 62;

        ss_y = 48;
      }
      else if (G_bunny_action == BUNNY_ACTION_CLIMBING)
      {
        if (t->vel_y < 0)
        {
          if (ANIM_CURRENT_FRAME(BUNNY_CLIMBING) == 0)
            ss_x = 58;
          else if (ANIM_CURRENT_FRAME(BUNNY_CLIMBING) == 1)
            ss_x = 60;
          else if (ANIM_CURRENT_FRAME(BUNNY_CLIMBING) == 2)
            ss_x = 58;
          else
            ss_x = 62;

          ss_y = 51;
        }
        else if (t->vel_y > 0)
        {
          if (ANIM_CURRENT_FRAME(BUNNY_CLIMBING) == 0)
            ss_x = 58;
          else if (ANIM_CURRENT_FRAME(BUNNY_CLIMBING) == 1)
            ss_x = 62;
          else if (ANIM_CURRENT_FRAME(BUNNY_CLIMBING) == 2)
            ss_x = 58;
          else
            ss_x = 60;

          ss_y = 51;
        }
        else
        {
          ss_x = 58;
          ss_y = 51;
        }
      }
      else
      {
        ss_x = 42;
        ss_y = 48;
      }

      /* adjust sprite sheet coordinates if the bunny is holding something */
      if (G_world_all_things[WORLD_HELD_THING_INDEX].type != THING_TYPE_NONE)
      {
        if ((G_bunny_action == BUNNY_ACTION_STANDING)       || 
            (G_bunny_action == BUNNY_ACTION_ACCELERATING)   || 
            (G_bunny_action == BUNNY_ACTION_WALKING)        || 
            (G_bunny_action == BUNNY_ACTION_DECELERATING)   || 
            (G_bunny_action == BUNNY_ACTION_HOPPING)        || 
            (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
            (G_bunny_action == BUNNY_ACTION_FALLING)        || 
            (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) ||
            (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
        {
          ss_y += 3;
        }
      }

      lighting = 0;
      palette = 0;
    }
    /* pickups */
    else if ( (t->type == THING_TYPE_GEM) && 
              (t->color == COLOR_WILD))
    {
      ss_x = 20;

      if (ANIM_CURRENT_FRAME(PICKUP) == 0)
        ss_y = 24;
      else if (ANIM_CURRENT_FRAME(PICKUP) == 1)
        ss_y = 25;
      else if (ANIM_CURRENT_FRAME(PICKUP) == 2)
        ss_y = 26;
      else
        ss_y = 27;

      lighting = 0;
      palette = 0;
    }
    else if ( (t->type == THING_TYPE_CARROT)        || 
              (t->type == THING_TYPE_BOOK)          || 
              (t->type == THING_TYPE_MUSIC_NOTES)   || 
              (t->type == THING_TYPE_GEM)           || 
              (t->type == THING_TYPE_MAGNET_DOWN)   || 
              (t->type == THING_TYPE_MAGNET_UP)     || 
              (t->type == THING_TYPE_MAGNET_RIGHT)  || 
              (t->type == THING_TYPE_MAGNET_LEFT)   || 
              (t->type == THING_TYPE_KEY)           || 
              (t->type == THING_TYPE_MUSHROOM))
    {
      /* determine sprite sheet x coordinate */
      if (t->type == THING_TYPE_MAGNET_UP)
      {
        if (ANIM_CURRENT_FRAME(PICKUP) == 0)
          ss_x = 13;
        else if (ANIM_CURRENT_FRAME(PICKUP) == 1)
          ss_x = 15;
        else if (ANIM_CURRENT_FRAME(PICKUP) == 2)
          ss_x = 13;
        else
          ss_x = 14;
      }
      else
      {
        if (ANIM_CURRENT_FRAME(PICKUP) == 0)
          ss_x = 13;
        else if (ANIM_CURRENT_FRAME(PICKUP) == 1)
          ss_x = 14;
        else if (ANIM_CURRENT_FRAME(PICKUP) == 2)
          ss_x = 13;
        else
          ss_x = 15;
      }

      /* determine sprite sheet y coordinate */
      if (t->type == THING_TYPE_CARROT)
        ss_y = 16;
      else if (t->type == THING_TYPE_BOOK)
        ss_y = 21;
      else if (t->type == THING_TYPE_MUSIC_NOTES)
        ss_y = 19;
      else if (t->type == THING_TYPE_GEM)
        ss_y = 20;
      else if ( (t->type == THING_TYPE_MAGNET_DOWN) || 
                (t->type == THING_TYPE_MAGNET_UP))
      {
        ss_y = 22;
      }
      else if ( (t->type == THING_TYPE_MAGNET_RIGHT) || 
                (t->type == THING_TYPE_MAGNET_LEFT))
      {
        ss_y = 23;
      }
      else if (t->type == THING_TYPE_KEY)
        ss_y = 17;
      else if (t->type == THING_TYPE_MUSHROOM)
        ss_y = 18;
      else
        ss_y = 16;

      /* determine lighting and palette */
      lighting = 0;

      if (t->color == COLOR_METAL)
        palette = 6;
      else if (t->color == COLOR_ORANGE)
        palette = 0;
      else if (t->color == COLOR_PINK)
        palette = 1;
      else if (t->color == COLOR_BLUE)
        palette = 3;
      else if (t->color == COLOR_GREEN)
        palette = 4;
      else
        palette = 0;
    }
    /* critters (also held & thrown versions) */
    else if ( (t->type == THING_TYPE_CRITTER_WALKER_1)        || 
              (t->type == THING_TYPE_CRITTER_WALKER_2)        || 
              (t->type == THING_TYPE_CRITTER_FLYER_1)         || 
              (t->type == THING_TYPE_CRITTER_FLYER_2)         || 
              (t->type == THING_TYPE_HELD_CRITTER_WALKER_1)   || 
              (t->type == THING_TYPE_HELD_CRITTER_WALKER_2)   || 
              (t->type == THING_TYPE_HELD_CRITTER_FLYER_1)    || 
              (t->type == THING_TYPE_HELD_CRITTER_FLYER_2)    || 
              (t->type == THING_TYPE_THROWN_CRITTER_WALKER_1) || 
              (t->type == THING_TYPE_THROWN_CRITTER_WALKER_2) || 
              (t->type == THING_TYPE_THROWN_CRITTER_FLYER_1)  || 
              (t->type == THING_TYPE_THROWN_CRITTER_FLYER_2))
    {
      /* determine sprite sheet x coordinate */
      if ((t->type == THING_TYPE_CRITTER_WALKER_1)  || 
          (t->type == THING_TYPE_CRITTER_WALKER_2)  || 
          (t->type == THING_TYPE_CRITTER_FLYER_1)   || 
          (t->type == THING_TYPE_CRITTER_FLYER_2))
      {
        if (t->state == STATE_CRITTER_STUNNED)
          ss_x = 19;
        else if (ANIM_CURRENT_FRAME(CRITTER) == 0)
          ss_x = 16;
        else if (ANIM_CURRENT_FRAME(CRITTER) == 1)
          ss_x = 17;
        else if (ANIM_CURRENT_FRAME(CRITTER) == 2)
          ss_x = 16;
        else
          ss_x = 18;
      }
      else if ( (t->type == THING_TYPE_HELD_CRITTER_WALKER_1) || 
                (t->type == THING_TYPE_HELD_CRITTER_WALKER_2) || 
                (t->type == THING_TYPE_HELD_CRITTER_FLYER_1)  || 
                (t->type == THING_TYPE_HELD_CRITTER_FLYER_2))
      {
        if (ANIM_CURRENT_FRAME(CRITTER_HELD) == 0)
          ss_x = 16;
        else if (ANIM_CURRENT_FRAME(CRITTER_HELD) == 1)
          ss_x = 17;
        else if (ANIM_CURRENT_FRAME(CRITTER_HELD) == 2)
          ss_x = 16;
        else
          ss_x = 18;
      }
      else
        ss_x = 19;

      if ((t->type == THING_TYPE_CRITTER_WALKER_2)      || 
          (t->type == THING_TYPE_HELD_CRITTER_WALKER_2) || 
          (t->type == THING_TYPE_THROWN_CRITTER_WALKER_2))
      {
        ss_x += 4;
      }
      else if ( (t->type == THING_TYPE_CRITTER_FLYER_1)       || 
                (t->type == THING_TYPE_HELD_CRITTER_FLYER_1)  || 
                (t->type == THING_TYPE_THROWN_CRITTER_FLYER_1))
      {
        ss_x += 8;
      }
      else if ( (t->type == THING_TYPE_CRITTER_FLYER_2)       || 
                (t->type == THING_TYPE_HELD_CRITTER_FLYER_2)  || 
                (t->type == THING_TYPE_THROWN_CRITTER_FLYER_2))
      {
        ss_x += 12;
      }

      /* determine sprite sheet y coordinate */
      if ((G_current_world >= 0) && (G_current_world < 8))
        ss_y = 16 + G_current_world;
      else if (G_current_world == 8)
        ss_y = 20;
      else if (G_current_world == 9)
        ss_y = 23;
      else if (G_current_world == 10)
        ss_y = 16;
      else if (G_current_world == 11)
        ss_y = 17;
      else
        ss_y = 16;

      lighting = 0;
      palette = 0;
    }
    /* platforms */
    else if ( (t->type == THING_TYPE_BASS_KNOB) || 
              (t->type == THING_TYPE_TREBLE_KNOB))
    {
      if ((t->type == THING_TYPE_BASS_KNOB) && (t->vel_y > 0))
      {
        if (ANIM_CURRENT_FRAME(KNOB_SLOW) == 0)
          ss_x = 16;
        else if (ANIM_CURRENT_FRAME(KNOB_SLOW) == 1)
          ss_x = 18;
        else
          ss_x = 17;
      }
      else if ((t->type == THING_TYPE_BASS_KNOB) && (t->vel_y < 0))
      {
        if (ANIM_CURRENT_FRAME(KNOB_FAST) == 0)
          ss_x = 16;
        else if (ANIM_CURRENT_FRAME(KNOB_FAST) == 1)
          ss_x = 17;
        else
          ss_x = 18;
      }
      else if ((t->type == THING_TYPE_TREBLE_KNOB) && (t->vel_y < 0))
      {
        if (ANIM_CURRENT_FRAME(KNOB_SLOW) == 0)
          ss_x = 16;
        else if (ANIM_CURRENT_FRAME(KNOB_SLOW) == 1)
          ss_x = 17;
        else
          ss_x = 18;
      }
      else if ((t->type == THING_TYPE_TREBLE_KNOB) && (t->vel_y > 0))
      {
        if (ANIM_CURRENT_FRAME(KNOB_FAST) == 0)
          ss_x = 16;
        else if (ANIM_CURRENT_FRAME(KNOB_FAST) == 1)
          ss_x = 18;
        else
          ss_x = 17;
      }
      else
        ss_x = 16;

      ss_y = 24;

      lighting = 0;

      if (t->type == THING_TYPE_BASS_KNOB)
        palette = 2;
      else if (t->type == THING_TYPE_TREBLE_KNOB)
        palette = 5;
      else
        palette = 2;
    }
    else if ( (t->type == THING_TYPE_ROCKET_BLOCK_RIGHT) || 
              (t->type == THING_TYPE_ROCKET_BLOCK_LEFT))
    {
      if (((t->type == THING_TYPE_ROCKET_BLOCK_RIGHT) && (t->vel_x > 0)) || 
          ((t->type == THING_TYPE_ROCKET_BLOCK_LEFT)  && (t->vel_x < 0)))
      {
        if (ANIM_CURRENT_FRAME(ROCKET_BLOCK) == 0)
          ss_x = 18;
        else
          ss_x = 17;
      }
      else if ( ((t->type == THING_TYPE_ROCKET_BLOCK_RIGHT) && (t->vel_x < 0)) || 
                ((t->type == THING_TYPE_ROCKET_BLOCK_LEFT)  && (t->vel_x > 0)))
      {
        if (ANIM_CURRENT_FRAME(ROCKET_BLOCK) == 0)
          ss_x = 16;
        else
          ss_x = 17;
      }
      else
        ss_x = 16;

      if (t->type == THING_TYPE_ROCKET_BLOCK_RIGHT)
        ss_y = 27;
      else if (t->type == THING_TYPE_ROCKET_BLOCK_LEFT)
        ss_y = 26;
      else
        ss_y = 27;

      lighting = 0;
      palette = 0;
    }
    else if ( (t->type == THING_TYPE_ELEVATOR_HORI_RIGHT) || 
              (t->type == THING_TYPE_ELEVATOR_HORI_LEFT))
    {
      if (t->state == STATE_ELEVATOR_TURNING)
        ss_x = 27;
      else if ((t->type == THING_TYPE_ELEVATOR_HORI_RIGHT) && (t->vel_x > 0))
      {
        if (ANIM_CURRENT_FRAME(ELEVATOR_ACTIVE) == 0)
          ss_x = 31;
        else
          ss_x = 30;
      }
      else if ((t->type == THING_TYPE_ELEVATOR_HORI_LEFT) && (t->vel_x < 0))
      {
        if (ANIM_CURRENT_FRAME(ELEVATOR_ACTIVE) == 0)
          ss_x = 29;
        else
          ss_x = 28;
      }
      else if ((t->type == THING_TYPE_ELEVATOR_HORI_RIGHT) && (t->vel_x == 0))
      {
        if (ANIM_CURRENT_FRAME(ELEVATOR_IDLE) == 0)
          ss_x = 30;
        else
          ss_x = 27;
      }
      else if ((t->type == THING_TYPE_ELEVATOR_HORI_LEFT) && (t->vel_x == 0))
      {
        if (ANIM_CURRENT_FRAME(ELEVATOR_IDLE) == 0)
          ss_x = 28;
        else
          ss_x = 27;
      }
      else
        ss_x = 27;

      ss_y = 29;

      lighting = 0;
      palette = 0;
    }
    else if ( (t->type == THING_TYPE_ELEVATOR_VERT_UP) || 
              (t->type == THING_TYPE_ELEVATOR_VERT_DOWN))
    {
      if (t->state == STATE_ELEVATOR_TURNING)
        ss_x = 27;
      else if ((t->type == THING_TYPE_ELEVATOR_VERT_UP) && (t->vel_y < 0))
      {
        if (ANIM_CURRENT_FRAME(ELEVATOR_ACTIVE) == 0)
          ss_x = 29;
        else
          ss_x = 28;
      }
      else if ((t->type == THING_TYPE_ELEVATOR_VERT_DOWN) && (t->vel_y > 0))
      {
        if (ANIM_CURRENT_FRAME(ELEVATOR_ACTIVE) == 0)
          ss_x = 31;
        else
          ss_x = 30;
      }
      else if ((t->type == THING_TYPE_ELEVATOR_VERT_UP) && (t->vel_y == 0))
      {
        if (ANIM_CURRENT_FRAME(ELEVATOR_IDLE) == 0)
          ss_x = 28;
        else
          ss_x = 27;
      }
      else if ((t->type == THING_TYPE_ELEVATOR_VERT_DOWN) && (t->vel_y == 0))
      {
        if (ANIM_CURRENT_FRAME(ELEVATOR_IDLE) == 0)
          ss_x = 30;
        else
          ss_x = 27;
      }
      else
        ss_x = 27;

      ss_y = 30;

      lighting = 0;
      palette = 0;
    }
    else if ( (t->type == THING_TYPE_DROP_BOLT_DOWN)  || 
              (t->type == THING_TYPE_DROP_BOLT_UP)    || 
              (t->type == THING_TYPE_ANTI_BOLT_UP)    || 
              (t->type == THING_TYPE_ANTI_BOLT_DOWN))
    {
      ss_x = 19;

      if (((t->type == THING_TYPE_DROP_BOLT_DOWN) && (t->vel_y > 0)) || 
          ((t->type == THING_TYPE_ANTI_BOLT_DOWN) && (t->vel_y > 0)))
      {
        if (ANIM_CURRENT_FRAME(BOLT_FAST) == 0)
          ss_y = 24;
        else if (ANIM_CURRENT_FRAME(BOLT_FAST) == 1)
          ss_y = 25;
        else if (ANIM_CURRENT_FRAME(BOLT_FAST) == 2)
          ss_y = 26;
        else
          ss_y = 27;
      }
      else if ( ((t->type == THING_TYPE_DROP_BOLT_DOWN) && (t->vel_y < 0)) || 
                ((t->type == THING_TYPE_ANTI_BOLT_DOWN) && (t->vel_y < 0)))
      {
        if (ANIM_CURRENT_FRAME(BOLT_SLOW) == 0)
          ss_y = 24;
        else if (ANIM_CURRENT_FRAME(BOLT_SLOW) == 1)
          ss_y = 27;
        else if (ANIM_CURRENT_FRAME(BOLT_SLOW) == 2)
          ss_y = 26;
        else
          ss_y = 25;
      }
      else if ( ((t->type == THING_TYPE_DROP_BOLT_UP) && (t->vel_y < 0)) || 
                ((t->type == THING_TYPE_ANTI_BOLT_UP) && (t->vel_y < 0)))
      {
        if (ANIM_CURRENT_FRAME(BOLT_FAST) == 0)
          ss_y = 24;
        else if (ANIM_CURRENT_FRAME(BOLT_FAST) == 1)
          ss_y = 27;
        else if (ANIM_CURRENT_FRAME(BOLT_FAST) == 2)
          ss_y = 26;
        else
          ss_y = 25;
      }
      else if ( ((t->type == THING_TYPE_DROP_BOLT_UP) && (t->vel_y > 0)) || 
                ((t->type == THING_TYPE_ANTI_BOLT_UP) && (t->vel_y > 0)))
      {
        if (ANIM_CURRENT_FRAME(BOLT_SLOW) == 0)
          ss_y = 24;
        else if (ANIM_CURRENT_FRAME(BOLT_SLOW) == 1)
          ss_y = 25;
        else if (ANIM_CURRENT_FRAME(BOLT_SLOW) == 2)
          ss_y = 26;
        else
          ss_y = 27;
      }
      else
        ss_y = 24;

      lighting = 0;

      if ((t->type == THING_TYPE_DROP_BOLT_DOWN) || 
          (t->type == THING_TYPE_DROP_BOLT_UP))
      {
        palette = 6;
      }
      else if ( (t->type == THING_TYPE_ANTI_BOLT_UP) || 
                (t->type == THING_TYPE_ANTI_BOLT_DOWN))
      {
        palette = 0;
      }
      else
        palette = 0;
    }
    else if (t->type == THING_TYPE_PLATFORM)
    {
      if (ANIM_CURRENT_FRAME(PLATFORM) == 0)
        ss_x = 20;
      else if (ANIM_CURRENT_FRAME(PLATFORM) == 1)
        ss_x = 21;
      else
        ss_x = 22;

      ss_y = 29;

      lighting = 0;
      palette = 0;
    }
    /* thrown / held marbles */
    else if ( (t->type == THING_TYPE_HELD_MARBLE) || 
              (t->type == THING_TYPE_THROWN_MARBLE))
    {
      if (t->state == STATE_MARBLE_CHANGING)
      {
        ss_x = 22;
        ss_y = 27;

        if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 0)
          lighting = -2;
        else if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 1)
          lighting = -4;
        else if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 2)
          lighting = -6;
        else if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 3)
          lighting = -4;
        else if (ANIM_CURRENT_FRAME(MARBLE_CHANGING) == 4)
          lighting = -2;
        else
          lighting = 0;

        palette = 7;
      }
      else
      {
        if (t->color == COLOR_WILD)
        {
          if (ANIM_CURRENT_FRAME(WILD_CARD) == 0)
            ss_x = 23;
          else if (ANIM_CURRENT_FRAME(WILD_CARD) == 1)
            ss_x = 24;
          else if (ANIM_CURRENT_FRAME(WILD_CARD) == 2)
            ss_x = 25;
          else
            ss_x = 26;
        }
        else
          ss_x = 22;

        ss_y = 27;

        if (t->state == STATE_MARBLE_FLASHING)
        {
          if (ANIM_CURRENT_FRAME(MARBLE_FLASHING) == 0)
            lighting = 2;
          else if (ANIM_CURRENT_FRAME(MARBLE_FLASHING) == 1)
            lighting = 4;
          else if (ANIM_CURRENT_FRAME(MARBLE_FLASHING) == 2)
            lighting = 6;
          else
            lighting = 4;
        }
        else
          lighting = 0;

        if (t->color == COLOR_METAL)
          palette = 6;
        else if (t->color == COLOR_ORANGE)
          palette = 0;
        else if (t->color == COLOR_PINK)
          palette = 1;
        else if (t->color == COLOR_BLUE)
          palette = 3;
        else if (t->color == COLOR_GREEN)
          palette = 4;
        else if (t->color == COLOR_WILD)
          palette = 0;
        else
          palette = 0;
      }
    }
    /* bubbles */
    else if (t->type == THING_TYPE_BUBBLE_HORIZONTAL)
    {
      if (ANIM_CURRENT_FRAME(BUBBLE_FLOATING) == 0)
        ss_x = 20;
      else if (ANIM_CURRENT_FRAME(BUBBLE_FLOATING) == 1)
        ss_x = 21;
      else if (ANIM_CURRENT_FRAME(BUBBLE_FLOATING) == 2)
        ss_x = 20;
      else
        ss_x = 22;

      ss_y = 30;

      lighting = 0;
      palette = 0;
    }
    else if (t->type == THING_TYPE_BUBBLE_VERTICAL)
    {
      if (ANIM_CURRENT_FRAME(BUBBLE_FLOATING) == 0)
      {
        ss_x = 20;
        ss_y = 30;
      }
      else if (ANIM_CURRENT_FRAME(BUBBLE_FLOATING) == 1)
      {
        ss_x = 21;
        ss_y = 31;
      }
      else if (ANIM_CURRENT_FRAME(BUBBLE_FLOATING) == 2)
      {
        ss_x = 20;
        ss_y = 30;
      }
      else
      {
        ss_x = 22;
        ss_y = 31;
      }

      lighting = 0;
      palette = 0;
    }
    else if (t->type == THING_TYPE_BUBBLE_STRESSED)
    {
      ss_x = 20;
      ss_y = 30;

      if (ANIM_CURRENT_FRAME(BUBBLE_STRESSED) == 0)
        lighting = 2;
      else if (ANIM_CURRENT_FRAME(BUBBLE_STRESSED) == 1)
        lighting = 0;
      else if (ANIM_CURRENT_FRAME(BUBBLE_STRESSED) == 2)
        lighting = 2;
      else if (ANIM_CURRENT_FRAME(BUBBLE_STRESSED) == 3)
        lighting = 0;
      else
        lighting = 2;

      palette = 0;
    }
    else if (t->type == THING_TYPE_BUBBLE_POPPED)
    {
      ss_x = 20;
      ss_y = 31;

      lighting = 0;
      palette = 0;
    }
    else
      continue;

    /* compute z level */
    z = GRAPHICS_Z_LEVEL_THINGS - (GRAPHICS_Z_LEVEL_SUBDIVISION_STEP * k);

    /* add this sprite to the buffers */
    if (t->type == THING_TYPE_BUNNY)
    {
      if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
      {
        if (G_bunny_facing == BUNNY_FACING_RIGHT)
        {
          VB_SPRITE_ADD_BUNNY_WITH_WRAPAROUND_NORMAL(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, NORMAL)
        }
        else
        {
          VB_SPRITE_ADD_BUNNY_WITH_WRAPAROUND_NORMAL(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, FLIP_HORI)
        }
      }
      else
      {
        if (G_bunny_facing == BUNNY_FACING_RIGHT)
        {
          VB_SPRITE_ADD_BUNNY_WITH_WRAPAROUND_FLIPPED(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, FLIP_VERT)
        }
        else
        {
          VB_SPRITE_ADD_BUNNY_WITH_WRAPAROUND_FLIPPED(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, FLIP_BOTH)
        }
      }
    }
    else
    {
      if (t->orient == THING_ORIENT_FLIP_HORI)
      {
        VB_SPRITE_ADD_THING_WITH_WRAPAROUND(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, FLIP_HORI)
      }
      else if (t->orient == THING_ORIENT_FLIP_VERT)
      {
        VB_SPRITE_ADD_THING_WITH_WRAPAROUND(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, FLIP_VERT)
      }
      else if (t->orient == THING_ORIENT_ROTATE_180)
      {
        VB_SPRITE_ADD_THING_WITH_WRAPAROUND(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, ROTATE_180)
      }
      else
      {
        VB_SPRITE_ADD_THING_WITH_WRAPAROUND(pos_x, pos_y, z, ss_x, ss_y, lighting, palette, NORMAL)
      }
    }
  }

  /* update grid objects & things sprite layer count */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS] = 
    sprite_index - GRAPHICS_GRID_OBJECTS_AND_THINGS_SPRITES_START_INDEX;

  /* update vbos */
  VB_SPRITE_UPDATE_GRID_OBJECTS_AND_THINGS_SPRITES_IN_VBOS()

  return 0;
}

/*******************************************************************************
** vb_sprite_clear_grid_objects_and_things()
*******************************************************************************/
short int vb_sprite_clear_grid_objects_and_things()
{
  /* reset sprite vbo counts */
  G_sprite_layer_counts[GRAPHICS_SPRITE_LAYER_GRID_OBJECTS_AND_THINGS] = 0;

  return 0;
}

