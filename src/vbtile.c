/*******************************************************************************
** vbtile.c (vbo population functions - tiles)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "graphics.h"
#include "level.h"
#include "vbtile.h"

#define VB_TILE_ADD_TO_VERTEX_BUFFER_32X32(pos_x, pos_y, z)                    \
  G_vertex_buffer_tiles[12 * G_num_tiles + 0]   = pos_x - 16;                  \
  G_vertex_buffer_tiles[12 * G_num_tiles + 1]   = pos_y - 16;                  \
  G_vertex_buffer_tiles[12 * G_num_tiles + 2]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 3]   = pos_x + 16;                  \
  G_vertex_buffer_tiles[12 * G_num_tiles + 4]   = pos_y - 16;                  \
  G_vertex_buffer_tiles[12 * G_num_tiles + 5]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 6]   = pos_x - 16;                  \
  G_vertex_buffer_tiles[12 * G_num_tiles + 7]   = pos_y + 16;                  \
  G_vertex_buffer_tiles[12 * G_num_tiles + 8]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 9]   = pos_x + 16;                  \
  G_vertex_buffer_tiles[12 * G_num_tiles + 10]  = pos_y + 16;                  \
  G_vertex_buffer_tiles[12 * G_num_tiles + 11]  = z;

#define VB_TILE_ADD_TO_VERTEX_BUFFER_16X16(pos_x, pos_y, z)                    \
  G_vertex_buffer_tiles[12 * G_num_tiles + 0]   = pos_x - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 1]   = pos_y - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 2]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 3]   = pos_x + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 4]   = pos_y - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 5]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 6]   = pos_x - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 7]   = pos_y + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 8]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 9]   = pos_x + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 10]  = pos_y + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 11]  = z;

#define VB_TILE_ADD_TO_VERTEX_BUFFER_16X16_SPLIT_TOP(pos_x, pos_y, z)          \
  G_vertex_buffer_tiles[12 * G_num_tiles + 0]   = pos_x - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 1]   = pos_y - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 2]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 3]   = pos_x + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 4]   = pos_y - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 5]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 6]   = pos_x - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 7]   = pos_y;                       \
  G_vertex_buffer_tiles[12 * G_num_tiles + 8]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 9]   = pos_x + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 10]  = pos_y;                       \
  G_vertex_buffer_tiles[12 * G_num_tiles + 11]  = z;

#define VB_TILE_ADD_TO_VERTEX_BUFFER_16X16_SPLIT_BOTTOM(pos_x, pos_y, z)       \
  G_vertex_buffer_tiles[12 * G_num_tiles + 0]   = pos_x - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 1]   = pos_y;                       \
  G_vertex_buffer_tiles[12 * G_num_tiles + 2]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 3]   = pos_x + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 4]   = pos_y;                       \
  G_vertex_buffer_tiles[12 * G_num_tiles + 5]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 6]   = pos_x - 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 7]   = pos_y + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 8]   = z;                           \
                                                                               \
  G_vertex_buffer_tiles[12 * G_num_tiles + 9]   = pos_x + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 10]  = pos_y + 8;                   \
  G_vertex_buffer_tiles[12 * G_num_tiles + 11]  = z;

#define VB_TILE_ADD_TO_TEXTURE_COORD_BUFFER_32X32(ss_x, ss_y)                                   \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 0] = G_texture_coord_table[4 * ss_x];          \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 1] = G_texture_coord_table[4 * (16 - ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 2] = G_texture_coord_table[4 * (ss_x + 1)];    \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 3] = G_texture_coord_table[4 * (16 - ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 4] = G_texture_coord_table[4 * ss_x];          \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 5] = G_texture_coord_table[4 * (15 - ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 6] = G_texture_coord_table[4 * (ss_x + 1)];    \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 7] = G_texture_coord_table[4 * (15 - ss_y)];

#define VB_TILE_ADD_TO_TEXTURE_COORD_BUFFER_16X16(ss_x, ss_y)                                   \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 0] = G_texture_coord_table[2 * ss_x];          \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 1] = G_texture_coord_table[2 * (32 - ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 2] = G_texture_coord_table[2 * (ss_x + 1)];    \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 3] = G_texture_coord_table[2 * (32 - ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 4] = G_texture_coord_table[2 * ss_x];          \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 5] = G_texture_coord_table[2 * (31 - ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 6] = G_texture_coord_table[2 * (ss_x + 1)];    \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 7] = G_texture_coord_table[2 * (31 - ss_y)];

#define VB_TILE_ADD_TO_TEXTURE_COORD_BUFFER_16X16_SPLIT_TOP(ss_x, ss_y)                         \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 0] = G_texture_coord_table[2 * ss_x];          \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 1] = G_texture_coord_table[2 * (32 - ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 2] = G_texture_coord_table[2 * (ss_x + 1)];    \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 3] = G_texture_coord_table[2 * (32 - ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 4] = G_texture_coord_table[2 * ss_x];          \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 5] = G_texture_coord_table[63 - (2 * ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 6] = G_texture_coord_table[2 * (ss_x + 1)];    \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 7] = G_texture_coord_table[63 - (2 * ss_y)];

#define VB_TILE_ADD_TO_TEXTURE_COORD_BUFFER_16X16_SPLIT_BOTTOM(ss_x, ss_y)                      \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 0] = G_texture_coord_table[2 * ss_x];          \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 1] = G_texture_coord_table[63 - (2 * ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 2] = G_texture_coord_table[2 * (ss_x + 1)];    \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 3] = G_texture_coord_table[63 - (2 * ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 4] = G_texture_coord_table[2 * ss_x];          \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 5] = G_texture_coord_table[2 * (31 - ss_y)];   \
                                                                                                \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 6] = G_texture_coord_table[2 * (ss_x + 1)];    \
  G_texture_coord_buffer_tiles[8 * G_num_tiles + 7] = G_texture_coord_table[2 * (31 - ss_y)];

#define VB_TILE_ADD_TO_PALETTE_COORD_BUFFER(lighting, palette)                                              \
  G_palette_coord_buffer_tiles[4 * G_num_tiles + 0] = G_palette_coord_table[4 + (8 * palette) + lighting];  \
                                                                                                            \
  G_palette_coord_buffer_tiles[4 * G_num_tiles + 1] = G_palette_coord_table[4 + (8 * palette) + lighting];  \
                                                                                                            \
  G_palette_coord_buffer_tiles[4 * G_num_tiles + 2] = G_palette_coord_table[4 + (8 * palette) + lighting];  \
                                                                                                            \
  G_palette_coord_buffer_tiles[4 * G_num_tiles + 3] = G_palette_coord_table[4 + (8 * palette) + lighting];

#define VB_TILE_ADD_TO_ELEMENT_BUFFER()                                        \
  G_index_buffer_tiles[6 * G_num_tiles + 0] = 4 * G_num_tiles + 0;             \
  G_index_buffer_tiles[6 * G_num_tiles + 1] = 4 * G_num_tiles + 2;             \
  G_index_buffer_tiles[6 * G_num_tiles + 2] = 4 * G_num_tiles + 1;             \
                                                                               \
  G_index_buffer_tiles[6 * G_num_tiles + 3] = 4 * G_num_tiles + 1;             \
  G_index_buffer_tiles[6 * G_num_tiles + 4] = 4 * G_num_tiles + 2;             \
  G_index_buffer_tiles[6 * G_num_tiles + 5] = 4 * G_num_tiles + 3;

#define VB_TILE_ADD_BACKDROP_TILE_TO_BUFFERS(pos_x, pos_y, z, ss_x, ss_y, lighting, palette)  \
  if (G_num_tiles < GRAPHICS_MAX_TILES)                                                       \
  {                                                                                           \
    VB_TILE_ADD_TO_VERTEX_BUFFER_16X16(pos_x, pos_y, z)                                       \
    VB_TILE_ADD_TO_TEXTURE_COORD_BUFFER_16X16(ss_x, ss_y)                                     \
    VB_TILE_ADD_TO_PALETTE_COORD_BUFFER(lighting, palette)                                    \
    VB_TILE_ADD_TO_ELEMENT_BUFFER()                                                           \
                                                                                              \
    G_num_tiles += 1;                                                                         \
  }

#define VB_TILE_ADD_BACKDROP_TILE_TO_BUFFERS_SHADOWED(pos_x, pos_y, z, ss_x, ss_y, lighting, palette) \
  if (G_num_tiles < GRAPHICS_MAX_TILES)                                                               \
  {                                                                                                   \
    VB_TILE_ADD_TO_VERTEX_BUFFER_16X16_SPLIT_TOP(pos_x, pos_y, z)                                     \
    VB_TILE_ADD_TO_TEXTURE_COORD_BUFFER_16X16_SPLIT_TOP(ss_x, ss_y)                                   \
    VB_TILE_ADD_TO_PALETTE_COORD_BUFFER((lighting - 1), palette)                                      \
    VB_TILE_ADD_TO_ELEMENT_BUFFER()                                                                   \
                                                                                                      \
    G_num_tiles += 1;                                                                                 \
  }                                                                                                   \
                                                                                                      \
  if (G_num_tiles < GRAPHICS_MAX_TILES)                                                               \
  {                                                                                                   \
    VB_TILE_ADD_TO_VERTEX_BUFFER_16X16_SPLIT_BOTTOM(pos_x, pos_y, z)                                  \
    VB_TILE_ADD_TO_TEXTURE_COORD_BUFFER_16X16_SPLIT_BOTTOM(ss_x, ss_y)                                \
    VB_TILE_ADD_TO_PALETTE_COORD_BUFFER(lighting, palette)                                            \
    VB_TILE_ADD_TO_ELEMENT_BUFFER()                                                                   \
                                                                                                      \
    G_num_tiles += 1;                                                                                 \
  }

#define VB_TILE_ADD_SKY_TILE_TO_BUFFERS(pos_x, pos_y, z, ss_x, ss_y, lighting, palette) \
  if (G_num_tiles < GRAPHICS_MAX_TILES)                                                 \
  {                                                                                     \
    VB_TILE_ADD_TO_VERTEX_BUFFER_32X32(pos_x, pos_y, z)                                 \
    VB_TILE_ADD_TO_TEXTURE_COORD_BUFFER_32X32(ss_x, ss_y)                               \
    VB_TILE_ADD_TO_PALETTE_COORD_BUFFER(lighting, palette)                              \
    VB_TILE_ADD_TO_ELEMENT_BUFFER()                                                     \
                                                                                        \
    G_num_tiles += 1;                                                                   \
  }

#define VB_TILE_UPDATE_STATIC_TILE_BUFFERS()                                   \
  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_tiles);                     \
  glBufferSubData(GL_ARRAY_BUFFER, 0,                                          \
                  G_num_tiles * 12 * sizeof(GLfloat),                          \
                  G_vertex_buffer_tiles);                                      \
                                                                               \
  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_tiles);              \
  glBufferSubData(GL_ARRAY_BUFFER, 0,                                          \
                  G_num_tiles * 8 * sizeof(GLfloat),                           \
                  G_texture_coord_buffer_tiles);                               \
                                                                               \
  glBindBuffer(GL_ARRAY_BUFFER, G_palette_coord_buffer_id_tiles);              \
  glBufferSubData(GL_ARRAY_BUFFER, 0,                                          \
                  G_num_tiles * 4 * sizeof(GLfloat),                           \
                  G_palette_coord_buffer_tiles);                               \
                                                                               \
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);              \
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,                                  \
                  G_num_tiles * 6 * sizeof(unsigned short),                    \
                  G_index_buffer_tiles);

/*******************************************************************************
** vb_tile_load_background_tilemaps()
*******************************************************************************/
short int vb_tile_load_background_tilemaps()
{
  int m;
  int n;

  int pos_x;
  int pos_y;

  int ss_x;
  int ss_y;

  int lighting;
  int palette;

  int above_box_y;

  unsigned char* sky_tilemap;
  unsigned char* backdrop_tilemap;
  unsigned char* room_tilemap;

  /* make sure current room is valid */
  if ((G_current_room < 0) || (G_current_room >= LEVEL_NUM_ROOMS))
    return 1;

  /* set sky tilemap */
  if (G_current_world == 0)
    sky_tilemap = &G_sky_tilemaps[0][0];
  else if (G_current_world == 1)
    sky_tilemap = &G_sky_tilemaps[1][0];
  else if (G_current_world == 2)
    sky_tilemap = &G_sky_tilemaps[2][0];
  else if (G_current_world == 3)
    sky_tilemap = &G_sky_tilemaps[3][0];
  else if (G_current_world == 4)
    sky_tilemap = &G_sky_tilemaps[4][0];
  else if (G_current_world == 5)
    sky_tilemap = &G_sky_tilemaps[5][0];
  else if (G_current_world == 6)
    sky_tilemap = &G_sky_tilemaps[6][0];
  else if (G_current_world == 7)
    sky_tilemap = &G_sky_tilemaps[7][0];
  else if (G_current_world == 8)
    sky_tilemap = &G_sky_tilemaps[5][0];
  else if (G_current_world == 9)
    sky_tilemap = &G_sky_tilemaps[2][0];
  else if (G_current_world == 10)
    sky_tilemap = &G_sky_tilemaps[3][0];
  else if (G_current_world == 11)
    sky_tilemap = &G_sky_tilemaps[6][0];
  else
    sky_tilemap = &G_sky_tilemaps[0][0];

  /* set backdrop tilemap */
  if ((G_current_room >= 0) && (G_current_room < LEVEL_NUM_ROOMS))
    backdrop_tilemap = &G_backdrop_tilemaps[G_current_room % LEVEL_NUM_BACKDROPS][0];
  else
    backdrop_tilemap = &G_backdrop_tilemaps[0][0]; 

  /* set room tilemap */
  if ((G_current_room >= 0) && (G_current_room < LEVEL_NUM_ROOMS))
    room_tilemap = &G_room_tilemaps[G_current_room][0];
  else
    room_tilemap = &G_room_tilemaps[0][0];

  /* initialize tile count */
  G_num_tiles = 0;

  /* load backdrop tilemap to vbo */
  for (n = 0; n < LEVEL_ROOM_HEIGHT; n++)
  {
    for (m = 0; m < LEVEL_ROOM_WIDTH; m++)
    {
      /* determine position */
      pos_x = LEVEL_ROOM_TILE_SIZE * m + LEVEL_ROOM_TILE_SIZE_HALF;
      pos_y = LEVEL_ROOM_TILE_SIZE * n + LEVEL_ROOM_TILE_SIZE_HALF;

      /* determine texture coordinates */
      /* if there is a block on this tile, use the block instead */
      if ((room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BLOCK_SINGLE)  ||
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BLOCK_LEFT)    ||
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BLOCK_RIGHT))
      {
        if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BLOCK_SINGLE)
          ss_x = 13;
        else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BLOCK_LEFT)
          ss_x = 14;
        else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BLOCK_RIGHT)
          ss_x = 15;

        if ((G_current_world >= 0) && (G_current_world < 12))
          ss_y = G_current_world;
        else
          ss_y = 0;

        /* determine lighting and palette */
        lighting = 0;
        palette = 0;

        /* add this tile to the buffers */
        VB_TILE_ADD_BACKDROP_TILE_TO_BUFFERS(pos_x, pos_y, GRAPHICS_Z_LEVEL_BACKDROP, 
                                            ss_x, ss_y, lighting, palette)
      }
      /* otherwise, use the appropriate backdrop tile */
      else
      {
        if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_MIDDLE)
          ss_x = 0;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_SIDE_TOP)
          ss_x = 1;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_SIDE_BOTTOM)
          ss_x = 2;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_SIDE_LEFT)
          ss_x = 3;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_SIDE_RIGHT)
          ss_x = 4;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_OUTER_CORNER_TOP_LEFT)
          ss_x = 5;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_OUTER_CORNER_TOP_RIGHT)
          ss_x = 6;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_OUTER_CORNER_BOTTOM_LEFT)
          ss_x = 7;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_OUTER_CORNER_BOTTOM_RIGHT)
          ss_x = 8;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_INNER_CORNER_TOP_LEFT)
          ss_x = 9;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_INNER_CORNER_TOP_RIGHT)
          ss_x = 10;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_INNER_CORNER_BOTTOM_LEFT)
          ss_x = 11;
        else if (backdrop_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_BACKDROP_TILE_INNER_CORNER_BOTTOM_RIGHT)
          ss_x = 12;
        else
          continue;

        if ((G_current_world >= 0) && (G_current_world < 12))
          ss_y = G_current_world;
        else
          ss_y = 0;

        /* determine lighting and palette */
        lighting = 0;
        palette = 0;

        /* if there is a permanent block above this tile, use the shadowed version */
        above_box_y = (n - 1 + LEVEL_ROOM_HEIGHT) % LEVEL_ROOM_HEIGHT;

        if ((room_tilemap[(above_box_y * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BLOCK_SINGLE)  || 
            (room_tilemap[(above_box_y * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BLOCK_LEFT)    || 
            (room_tilemap[(above_box_y * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BLOCK_RIGHT)   || 
            (room_tilemap[(above_box_y * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ICE_SINGLE)    || 
            (room_tilemap[(above_box_y * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ICE_LEFT)      || 
            (room_tilemap[(above_box_y * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ICE_RIGHT)     || 
            (room_tilemap[(above_box_y * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_HEART_BLOCK)   || 
            (room_tilemap[(above_box_y * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_DIAMOND_BLOCK))
        {
          VB_TILE_ADD_BACKDROP_TILE_TO_BUFFERS_SHADOWED( pos_x, pos_y, GRAPHICS_Z_LEVEL_BACKDROP, 
                                                        ss_x, ss_y, lighting, palette)
        }
        else
        {
          VB_TILE_ADD_BACKDROP_TILE_TO_BUFFERS(pos_x, pos_y, GRAPHICS_Z_LEVEL_BACKDROP, 
                                              ss_x, ss_y, lighting, palette)
        }
      }
    }
  }

  /* load sky tilemap to vbo */
  for (n = 0; n < LEVEL_SKY_HEIGHT; n++)
  {
    for (m = 0; m < LEVEL_SKY_WIDTH; m++)
    {
      /* determine position */
      pos_x = LEVEL_SKY_TILE_SIZE * m + LEVEL_SKY_TILE_SIZE_HALF;
      pos_y = LEVEL_SKY_TILE_SIZE * n + LEVEL_SKY_TILE_SIZE_HALF;

      /* determine texture coordinates */
      if (sky_tilemap[(n * LEVEL_SKY_WIDTH) + m] == LEVEL_SKY_TILE_1)
        ss_y = 0;
      else if (sky_tilemap[(n * LEVEL_SKY_WIDTH) + m] == LEVEL_SKY_TILE_2)
        ss_y = 1;
      else if (sky_tilemap[(n * LEVEL_SKY_WIDTH) + m] == LEVEL_SKY_TILE_3)
        ss_y = 2;
      else if (sky_tilemap[(n * LEVEL_SKY_WIDTH) + m] == LEVEL_SKY_TILE_4)
        ss_y = 3;
      else if (sky_tilemap[(n * LEVEL_SKY_WIDTH) + m] == LEVEL_SKY_TILE_5)
        ss_y = 4;
      else if (sky_tilemap[(n * LEVEL_SKY_WIDTH) + m] == LEVEL_SKY_TILE_6)
        ss_y = 5;
      else if (sky_tilemap[(n * LEVEL_SKY_WIDTH) + m] == LEVEL_SKY_TILE_7)
        ss_y = 6;
      else if (sky_tilemap[(n * LEVEL_SKY_WIDTH) + m] == LEVEL_SKY_TILE_8)
        ss_y = 7;
      else
        ss_y = 0;

      if ((G_current_world >= 0) && (G_current_world < 8))
        ss_x = 8 + G_current_world;
      else if (G_current_world == 8)
        ss_x = 13;
      else if (G_current_world == 9)
        ss_x = 10;
      else if (G_current_world == 10)
        ss_x = 11;
      else if (G_current_world == 11)
        ss_x = 14;
      else
        ss_x = 8;

      /* determine lighting and palette */
      lighting = 0;

      if ((G_current_world >= 0) && (G_current_world < 8))
        palette = 0;
      else if (G_current_world == 8)
        palette = 1;
      else if (G_current_world == 9)
        palette = 3;
      else if (G_current_world == 10)
        palette = 5;
      else if (G_current_world == 11)
        palette = 5;
      else
        palette = 0;

      /* add this tile to the buffers */
      VB_TILE_ADD_SKY_TILE_TO_BUFFERS(pos_x, pos_y, GRAPHICS_Z_LEVEL_SKY, 
                                      ss_x, ss_y, lighting, palette)
    }
  }

  /* update vbo */
  VB_TILE_UPDATE_STATIC_TILE_BUFFERS()

  return 0;
}

