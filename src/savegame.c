/*******************************************************************************
** savegame.c (save games)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "path.h"
#include "savegame.h"

int G_save_game_current_slot;
int G_save_game_percent;

unsigned char G_save_game_data[SAVE_GAME_NUM_WORLDS];

/*******************************************************************************
** save_game_read_file()
*******************************************************************************/
short int save_game_read_file()
{
  int   i;

  FILE* fp;
  char  signature[4];
  char  type[4];

  /* determine save game to read */
  if (G_save_game_current_slot == 1)
    fp = fopen(G_path_save_game_1, "rb");
  else if (G_save_game_current_slot == 2)
    fp = fopen(G_path_save_game_2, "rb");
  else if (G_save_game_current_slot == 3)
    fp = fopen(G_path_save_game_3, "rb");
  else
    return 1;

  /* if save game did not open, return error */
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

  if ((type[0] != 'S') || 
      (type[1] != 'A') || 
      (type[2] != 'V') || 
      (type[3] != 'E'))
  {
    fclose(fp);
    return 1;
  }

  /* read save data */
  if (fread(G_save_game_data, 1, SAVE_GAME_NUM_WORLDS, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close save game */
  fclose(fp);

  /* validate save game */
  for (i = 0; i < SAVE_GAME_NUM_WORLDS; i++)
  {
    if (G_save_game_data[i] < 0)
      G_save_game_data[i] = 0;
    else if (G_save_game_data[i] > SAVE_GAME_ROOMS_PER_WORLD)
      G_save_game_data[i] = SAVE_GAME_ROOMS_PER_WORLD;
  }

  /* make sure that the episodes are unlocked in order */
  if ((G_save_game_data[0] < SAVE_GAME_ROOMS_PER_WORLD) || 
      (G_save_game_data[1] < SAVE_GAME_ROOMS_PER_WORLD) || 
      (G_save_game_data[2] < SAVE_GAME_ROOMS_PER_WORLD) || 
      (G_save_game_data[3] < SAVE_GAME_ROOMS_PER_WORLD))
  {
    for (i = 4; i < SAVE_GAME_NUM_WORLDS; i++)
      G_save_game_data[i] = 0;
  }
  else if ( (G_save_game_data[4] < SAVE_GAME_ROOMS_PER_WORLD) || 
            (G_save_game_data[5] < SAVE_GAME_ROOMS_PER_WORLD) || 
            (G_save_game_data[6] < SAVE_GAME_ROOMS_PER_WORLD) || 
            (G_save_game_data[7] < SAVE_GAME_ROOMS_PER_WORLD))
  {
    for (i = 8; i < SAVE_GAME_NUM_WORLDS; i++)
      G_save_game_data[i] = 0;
  }

  return 0;
}

/*******************************************************************************
** save_game_write_file()
*******************************************************************************/
short int save_game_write_file()
{
  FILE* fp;
  char  signature[4];
  char  type[4];

  /* determine save game to write */
  if (G_save_game_current_slot == 1)
    fp = fopen(G_path_save_game_1, "wb");
  else if (G_save_game_current_slot == 2)
    fp = fopen(G_path_save_game_2, "wb");
  else if (G_save_game_current_slot == 3)
    fp = fopen(G_path_save_game_3, "wb");
  else
    return 1;

  /* if save game did not open, return error */
  if (fp == NULL)
    return 1;

  /* write signature */
  signature[0] = 'C';
  signature[1] = 'A';
  signature[2] = 'R';
  signature[3] = 'R';

  if (fwrite(signature, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write type */
  type[0] = 'S';
  type[1] = 'A';
  type[2] = 'V';
  type[3] = 'E';

  if (fwrite(type, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  /* write save data */
  if (fwrite(G_save_game_data, 1, SAVE_GAME_NUM_WORLDS, fp) == 0)
  {
    fclose(fp);
    return 1;
  }

  /* close save game */
  fclose(fp);

  return 0;
}

/*******************************************************************************
** save_game_set_slot()
*******************************************************************************/
short int save_game_set_slot(int num)
{
  if ((num >= 1) && (num <= 3))
    G_save_game_current_slot = num;
  else
    G_save_game_current_slot = 1;

  save_game_load();

  return 0;
}

/*******************************************************************************
** save_game_load()
*******************************************************************************/
short int save_game_load()
{
  int i;

  /* make sure the slot is valid */
  if ((G_save_game_current_slot < 1) || (G_save_game_current_slot > 3))
    return 1;

  /* load this slot */
  if (save_game_read_file())
  {
    for (i = 0; i < SAVE_GAME_NUM_WORLDS; i++)
      G_save_game_data[i] = 0;

    G_save_game_percent = 0;
  }
  else
    save_game_compute_percent();

  return 0;
}

/*******************************************************************************
** save_game_save()
*******************************************************************************/
short int save_game_save()
{
  /* make sure the slot is valid */
  if ((G_save_game_current_slot < 1) || (G_save_game_current_slot > 3))
    return 1;

  /* save this slot */
  save_game_write_file();

  return 0;
}

/*******************************************************************************
** save_game_clear()
*******************************************************************************/
short int save_game_clear()
{
  int i;

  /* make sure the slot is valid */
  if ((G_save_game_current_slot < 1) || (G_save_game_current_slot > 3))
    return 1;

  /* clear this slot */
  for (i = 0; i < SAVE_GAME_NUM_WORLDS; i++)
    G_save_game_data[i] = 0;

  G_save_game_percent = 0;

  save_game_write_file();

  return 0;
}

/*******************************************************************************
** save_game_compute_percent()
*******************************************************************************/
short int save_game_compute_percent()
{
  int i;
  int rooms_completed;

  /* compute number of rooms completed */
  rooms_completed = 0;

  for (i = 0; i < SAVE_GAME_NUM_WORLDS; i++)
    rooms_completed += G_save_game_data[i];

  if (rooms_completed < 0)
    rooms_completed = 0;
  else if (rooms_completed > SAVE_GAME_NUM_ROOMS)
    rooms_completed = SAVE_GAME_NUM_ROOMS;

  /* compute percent of rooms completed (in tenths) */
  G_save_game_percent = (rooms_completed * 1000) / SAVE_GAME_NUM_ROOMS;

  return 0;
}

/*******************************************************************************
** save_game_mark_current_room_complete()
*******************************************************************************/
short int save_game_mark_current_room_complete()
{
  int world_index;
  int room_index;

  /* make sure this is a valid room index */
  if ((G_current_room < 0) || (G_current_room >= SAVE_GAME_NUM_ROOMS))
    return 1;

  /* determine world and room index */
  world_index = G_current_room / SAVE_GAME_ROOMS_PER_WORLD;
  room_index = G_current_room % SAVE_GAME_ROOMS_PER_WORLD;

  /* check if a new room was completed */
  if (G_save_game_data[world_index] == room_index)
  {
    G_save_game_data[world_index] = room_index + 1;
    save_game_compute_percent();
    save_game_save();

    /* set world complete flag if necessary */
    if (G_save_game_data[world_index] == SAVE_GAME_ROOMS_PER_WORLD)
    {
      if ((world_index >= 0) && (world_index < 4) && 
          (G_save_game_data[0] == SAVE_GAME_ROOMS_PER_WORLD) && 
          (G_save_game_data[1] == SAVE_GAME_ROOMS_PER_WORLD) && 
          (G_save_game_data[2] == SAVE_GAME_ROOMS_PER_WORLD) && 
          (G_save_game_data[3] == SAVE_GAME_ROOMS_PER_WORLD))
      {
        G_flag_world_complete = 2;
      }
      else if ( (world_index >= 4) && (world_index < 8) && 
                (G_save_game_data[4] == SAVE_GAME_ROOMS_PER_WORLD) && 
                (G_save_game_data[5] == SAVE_GAME_ROOMS_PER_WORLD) && 
                (G_save_game_data[6] == SAVE_GAME_ROOMS_PER_WORLD) && 
                (G_save_game_data[7] == SAVE_GAME_ROOMS_PER_WORLD))
      {
        G_flag_world_complete = 2;
      }
      else if ( (world_index >= 8) && (world_index < 12) && 
                (G_save_game_data[8]  == SAVE_GAME_ROOMS_PER_WORLD) && 
                (G_save_game_data[9]  == SAVE_GAME_ROOMS_PER_WORLD) && 
                (G_save_game_data[10] == SAVE_GAME_ROOMS_PER_WORLD) && 
                (G_save_game_data[11] == SAVE_GAME_ROOMS_PER_WORLD))
      {
        G_flag_world_complete = 2;
      }
      else
        G_flag_world_complete = 1;
    }
  }

  return 0;
}

