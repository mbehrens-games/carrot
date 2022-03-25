/*******************************************************************************
** level.c (level)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "level.h"

/* global variables */
unsigned char G_sky_tilemaps[LEVEL_NUM_SKYS][LEVEL_SKY_NUM_TILES];
unsigned char G_backdrop_tilemaps[LEVEL_NUM_BACKDROPS][LEVEL_ROOM_NUM_TILES];
unsigned char G_room_tilemaps[LEVEL_NUM_ROOMS][LEVEL_ROOM_NUM_TILES];

char G_world_names[LEVEL_NUM_WORLDS][LEVEL_WORLD_NAME_SIZE];
char G_room_names[LEVEL_NUM_ROOMS][LEVEL_ROOM_NAME_SIZE];

/*******************************************************************************
** level_init_arrays()
*******************************************************************************/
short int level_init_arrays()
{
  int m;
  int n;

  /* initialize sky tilemaps */
  for (m = 0; m < LEVEL_NUM_SKYS; m++)
  {
    for (n = 0; n < LEVEL_SKY_NUM_TILES; n++)
    {
      G_sky_tilemaps[m][n] = LEVEL_SKY_TILE_1;
    }
  }

  /* initialize backdrop tilemaps */
  for (m = 0; m < LEVEL_NUM_BACKDROPS; m++)
  {
    for (n = 0; n < LEVEL_ROOM_NUM_TILES; n++)
    {
      G_backdrop_tilemaps[m][n] = LEVEL_BACKDROP_TILE_MIDDLE;
    }
  }

  /* initialize room tilemaps */
  for (m = 0; m < LEVEL_NUM_ROOMS; m++)
  {
    for (n = 0; n < LEVEL_ROOM_NUM_TILES; n++)
    {
      G_room_tilemaps[m][n] = LEVEL_ROOM_TILE_BLANK;
    }
  }

  /* initialize name arrays */
  for (m = 0; m < LEVEL_NUM_WORLDS; m++)
  {
    for (n = 0; n < LEVEL_WORLD_NAME_SIZE; n++)
    {
      G_world_names[m][n] = '\0';
    }
  }

  for (m = 0; m < LEVEL_NUM_ROOMS; m++)
  {
    for (n = 0; n < LEVEL_ROOM_NAME_SIZE; n++)
    {
      G_room_names[m][n] = '\0';
    }
  }

  return 0;
}

/*******************************************************************************
** level_load_all_from_file()
*******************************************************************************/
short int level_load_all_from_file(char* filename)
{
  FILE* fp;

  int   i;
  int   j;

  char  signature[4];
  char  type[4];

  if (filename == NULL)
    return 1;

  /* initialize data */
  level_init_arrays();

  /* open file */
  fp = fopen(filename, "rb");

  /* if file did not open, return error */
  if (fp == NULL)
    return 1;

  /* read signature */
  if (fread(signature, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if ((signature[0] != 'C') || 
      (signature[1] != 'A') || 
      (signature[2] != 'R') || 
      (signature[3] != 'R'))
  {
    fclose(fp);
    return 1;
  }

  /* read type */
  if (fread(type, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if ((type[0] != 'L') || 
      (type[1] != 'V') || 
      (type[2] != 'L') || 
      (type[3] != 'S'))
  {
    fclose(fp);
    return 1;
  }

  /* read skys */
  for (i = 0; i < LEVEL_NUM_SKYS; i++)
  {
    for (j = 0; j < LEVEL_SKY_HEIGHT; j++)
    {
      /* read this row from the file */
      if (fread(&G_sky_tilemaps[i][j * LEVEL_SKY_WIDTH], 1, LEVEL_SKY_WIDTH, fp) == 0)
      {
        fclose(fp);
        return 1;
      }
    }

    /* convert any invalid sky tile values to "1" */
    for (j = 0; j < LEVEL_SKY_NUM_TILES; j++)
    {
      if ((G_sky_tilemaps[i][j] < LEVEL_SKY_TILE_1) || 
          (G_sky_tilemaps[i][j] > LEVEL_SKY_TILE_8))
      {
        G_sky_tilemaps[i][j] = LEVEL_SKY_TILE_1;
      }
    }
  }

  /* read backdrops */
  for (i = 0; i < LEVEL_NUM_BACKDROPS; i++)
  {
    for (j = 0; j < LEVEL_ROOM_HEIGHT; j++)
    {
      /* read this row from the file */
      if (fread(&G_backdrop_tilemaps[i][j * LEVEL_ROOM_WIDTH], 1, LEVEL_ROOM_WIDTH, fp) == 0)
      {
        fclose(fp);
        return 1;
      }
    }

    /* convert any invalid backdrop tile values to "middle" */
    for (j = 0; j < LEVEL_ROOM_HEIGHT * LEVEL_ROOM_WIDTH; j++)
    {
      if ((G_backdrop_tilemaps[i][j] < LEVEL_BACKDROP_TILE_OPEN) ||
          (G_backdrop_tilemaps[i][j] > LEVEL_BACKDROP_TILE_INNER_CORNER_BOTTOM_RIGHT))
      {
        G_backdrop_tilemaps[i][j] = LEVEL_BACKDROP_TILE_MIDDLE;
      }
    }
  }

  /* read rooms */
  for (i = 0; i < LEVEL_NUM_ROOMS; i++)
  {
    for (j = 0; j < LEVEL_ROOM_HEIGHT; j++)
    {
      /* read this row from the file */
      if (fread(&G_room_tilemaps[i][j * LEVEL_ROOM_WIDTH], 1, LEVEL_ROOM_WIDTH, fp) == 0)
      {
        fclose(fp);
        return 1;
      }
    }

    /* convert any invalid room tile values to "blank" */
    for (j = 0; j < LEVEL_ROOM_HEIGHT * LEVEL_ROOM_WIDTH; j++)
    {
      if ((G_room_tilemaps[i][j] < LEVEL_ROOM_TILE_BLANK) ||
          (G_room_tilemaps[i][j] > LEVEL_ROOM_TILE_ARROWS_315))
      {
        G_room_tilemaps[i][j] = LEVEL_ROOM_TILE_BLANK;
      }
    }
  }

  /* read world names */
  for (i = 0; i < LEVEL_NUM_WORLDS; i++)
  {
    /* read this name from the file */
    if (fread(&G_world_names[i][0], 1, LEVEL_WORLD_NAME_SIZE, fp) == 0)
    {
      fclose(fp);
      return 1;
    }
  }

  /* read room names */
  for (i = 0; i < LEVEL_NUM_ROOMS; i++)
  {
    /* read this name from the file */
    if (fread(&G_room_names[i][0], 1, LEVEL_ROOM_NAME_SIZE, fp) == 0)
    {
      fclose(fp);
      return 1;
    }
  }

  /* decode text data, and convert any      */
  /* invalid characters to null terminators */

  /* the ascii range used is 32 to 127 */
  /* the coded values are from 1 to 96 */
  for (i = 0; i < LEVEL_NUM_WORLDS; i++)
  {
    for (j = 0; j < LEVEL_WORLD_NAME_SIZE; j++)
    {
      if ((G_world_names[i][j] >= 1) && (G_world_names[i][j] <= 96))
        G_world_names[i][j] = G_world_names[i][j] + 31;
      else
        G_world_names[i][j] = '\0';
    }
  }

  for (i = 0; i < LEVEL_NUM_ROOMS; i++)
  {
    for (j = 0; j < LEVEL_ROOM_NAME_SIZE; j++)
    {
      if ((G_room_names[i][j] >= 1) && (G_room_names[i][j] <= 96))
        G_room_names[i][j] = G_room_names[i][j] + 31;
      else
        G_room_names[i][j] = '\0';
    }
  }

  /* close file */
  fclose(fp);

  return 0;
}

