/*******************************************************************************
** doremi.c (sound and music)
*******************************************************************************/

#ifdef CARROT_MAC_OSX
  #include <SDL2_mixer/SDL_mixer.h>
#else
  #include <SDL2/SDL_mixer.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doremi.h"
#include "path.h"

Mix_Chunk G_sfx_all[SFX_NUM_INDICES];
Mix_Music *G_music;

int       G_sound_volume;

static int S_current_music_track;

/*******************************************************************************
** doremi_init_all_sfx()
*******************************************************************************/
short int doremi_init_all_sfx()
{
  int         i;
  Mix_Chunk*  chunk;

  for (i = 0; i < SFX_NUM_INDICES; i++)
  {
    chunk = &G_sfx_all[i];

    chunk->allocated = 0;
    chunk->abuf = NULL;
    chunk->alen = 0;
    chunk->volume = 0;
  }

  return 0;
}

/*******************************************************************************
** doremi_deinit_all_sfx()
*******************************************************************************/
short int doremi_deinit_all_sfx()
{
  int         i;
  Mix_Chunk*  chunk;

  for (i = 0; i < SFX_NUM_INDICES; i++)
  {
    chunk = &G_sfx_all[i];

    if (chunk->allocated == 1)
    {
      if (chunk->abuf != NULL)
      {
        free(chunk->abuf);
        chunk->abuf = NULL;
      }

      chunk->alen = 0;
      chunk->volume = 0;
      chunk->allocated = 0;
    }
  }

  return 0;
}

/*******************************************************************************
** doremi_load_all_sfx_from_file()
*******************************************************************************/
short int doremi_load_all_sfx_from_file(char* filename)
{
  FILE*         fp;
  int           i;
  int           j;
  int           num_samples;
  short int     tmp_int;
  unsigned int  tmp_uint;
  unsigned char msb;
  unsigned char lsb;
  char          signature[4];
  char          type[4];
  Mix_Chunk*    chunk;

  if (filename == NULL)
    return 1;

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

  if ((type[0] != 'S') || 
      (type[1] != 'O') || 
      (type[2] != 'U') || 
      (type[3] != 'N'))
  {
    fclose(fp);
    return 1;
  }

  /* cycle through sfx */
  for (i = 0; i < SFX_NUM_INDICES; i++)
  {
    chunk = &G_sfx_all[i];

    /* read number of samples */
    if (fread(&tmp_uint, 4, 1, fp) == 0)
    {
      fclose(fp);
      return 1;
    }

    num_samples = (int) tmp_uint;

    /* initialize chunk */
    chunk->allocated = 1;
    chunk->abuf = malloc(sizeof(short int) * 2 * num_samples);
    chunk->alen = sizeof(short int) * 2 * num_samples;
    chunk->volume = 64;

    /* load mono sfx data to 16 bit stereo audio in sdl */
    for (j = 0; j < num_samples; j++)
    {
      if (fread(&tmp_int, 2, 1, fp) == 0)
      {
        fclose(fp);
        return 1;
      }

      tmp_int = (int) (tmp_int * 0.70710678118655);

      lsb = tmp_int % 256;
      msb = tmp_int / 256;

      chunk->abuf[(4 * j) + 0] = lsb;
      chunk->abuf[(4 * j) + 1] = msb;
      chunk->abuf[(4 * j) + 2] = lsb;
      chunk->abuf[(4 * j) + 3] = msb;
    }
  }

  /* close file */
  fclose(fp);

  /* set volume for all sfx */
  Mix_Volume(-1, G_sound_volume);

  return 0;
}

/*******************************************************************************
** doremi_play_sfx()
*******************************************************************************/
short int doremi_play_sfx(int type)
{
  int         i;
  int         num_channels;
  Mix_Chunk*  chunk;

  if ((type < 0) || (type >= SFX_NUM_INDICES))
    return 1;

  /* get number of currently allocated channels */
  num_channels = Mix_AllocateChannels(-1);

  /* set chunk pointer */
  chunk = &G_sfx_all[type];

  /* check if this sample is already playing */
  for (i = 0; i < num_channels; i++)
  {
    if ((Mix_Playing(i)) && (Mix_GetChunk(i) == chunk))
    {
      Mix_HaltChannel(i);
      Mix_PlayChannel(i, chunk, 0);
      return 0;
    }
  }

  /* otherwise, play sample on first available channel */
  Mix_PlayChannel(-1, chunk, 0);

  return 0;
}

/*******************************************************************************
** doremi_init_music()
*******************************************************************************/
short int doremi_init_music()
{
  G_music = NULL;

  S_current_music_track = MUSIC_TRACK_NONE;

  return 0;
}

/*******************************************************************************
** doremi_deinit_music()
*******************************************************************************/
short int doremi_deinit_music()
{
  if (G_music != NULL)
  {
    Mix_FreeMusic(G_music);
    G_music = NULL;
  }

  S_current_music_track = MUSIC_TRACK_NONE;

  return 0;
}

/*******************************************************************************
** doremi_play_music()
*******************************************************************************/
short int doremi_play_music(int track)
{
  char* path_music_track;

  /* check if we are already playing this track */
  if (track == S_current_music_track)
    return 0;

  /* stop currently playing music, if any */
  if (G_music != NULL)
  {
    Mix_FreeMusic(G_music);
    G_music = NULL;
  }

  /* allocate string for music filename */
  path_music_track = malloc(sizeof(char) * (strlen(G_path_ost) + 64 + 1));
  strcpy(path_music_track, G_path_ost);

  /* determine new music based on type */
  if (track == MUSIC_TRACK_MENU)
    strcpy(path_music_track + strlen(G_path_ost), "rein1.xm");
  else if (track == MUSIC_TRACK_STORY)
    strcpy(path_music_track + strlen(G_path_ost), "rein2.xm");
  else if (track == MUSIC_TRACK_WORLD_01)
    strcpy(path_music_track + strlen(G_path_ost), "cham1.xm");
  else if (track == MUSIC_TRACK_WORLD_02)
    strcpy(path_music_track + strlen(G_path_ost), "cleo1.xm");
  else if (track == MUSIC_TRACK_WORLD_03)
    strcpy(path_music_track + strlen(G_path_ost), "tris1.xm");
  else if (track == MUSIC_TRACK_WORLD_04)
    strcpy(path_music_track + strlen(G_path_ost), "amer.xm");
  else if (track == MUSIC_TRACK_WORLD_05)
    strcpy(path_music_track + strlen(G_path_ost), "bohe1.xm");
  else if (track == MUSIC_TRACK_WORLD_06)
    strcpy(path_music_track + strlen(G_path_ost), "cels.xm");
  else if (track == MUSIC_TRACK_WORLD_07)
    strcpy(path_music_track + strlen(G_path_ost), "ethi.xm");
  else if (track == MUSIC_TRACK_WORLD_08)
    strcpy(path_music_track + strlen(G_path_ost), "gale.xm");
  else if (track == MUSIC_TRACK_WORLD_09)
    strcpy(path_music_track + strlen(G_path_ost), "bohe2.xm");
  else if (track == MUSIC_TRACK_WORLD_10)
    strcpy(path_music_track + strlen(G_path_ost), "tris2.xm");
  else if (track == MUSIC_TRACK_WORLD_11)
    strcpy(path_music_track + strlen(G_path_ost), "cham2.xm");
  else if (track == MUSIC_TRACK_WORLD_12)
    strcpy(path_music_track + strlen(G_path_ost), "cleo2.xm");
  else
    return 0;

  /* load music track */
  G_music = Mix_LoadMUS(path_music_track);

  /* check that music track was loaded */
  if (G_music == NULL)
  {
    fprintf(stdout, "Mix_LoadMUS Error: %s\n", Mix_GetError());
    S_current_music_track = MUSIC_TRACK_NONE;
    return 1;
  }

  /* free music filename string */
  free(path_music_track);

  /* play music track */
  if (G_music != NULL)
  {
    if (Mix_PlayMusic(G_music, -1) == -1)
    {
      fprintf(stdout, "Mix_PlayMusic Error: %s\n", Mix_GetError());
      S_current_music_track = MUSIC_TRACK_NONE;
      return 1;
    }

    Mix_VolumeMusic(G_sound_volume);
  }
  else
  {
    fprintf(stdout, "Failed to load XM file.\n");
    S_current_music_track = MUSIC_TRACK_NONE;
    return 1;
  }

  /* save current music track */
  S_current_music_track = track;

  return 0;
}

/*******************************************************************************
** doremi_pause_music()
*******************************************************************************/
short int doremi_pause_music()
{
  if (G_music == NULL)
    return 0;

  Mix_PauseMusic();

  return 0;
}

/*******************************************************************************
** doremi_unpause_music()
*******************************************************************************/
short int doremi_unpause_music()
{
  if (G_music == NULL)
    return 0;

  Mix_ResumeMusic();

  return 0;
}

/*******************************************************************************
** doremi_apply_volume()
*******************************************************************************/
short int doremi_apply_volume()
{
  /* set sfx volume */
  Mix_Volume(-1, G_sound_volume);

  /* set music volume */
  if (G_music != NULL)
    Mix_VolumeMusic(G_sound_volume);

  return 0;
}

/*******************************************************************************
** doremi_set_volume()
*******************************************************************************/
short int doremi_set_volume(int steps)
{
  if ((steps >= 0) && (steps <= 8))
  {
    G_sound_volume = 16 * steps;
    doremi_apply_volume();
  }

  return 0;
}

/*******************************************************************************
** doremi_increase_volume()
*******************************************************************************/
short int doremi_increase_volume()
{
  if ((G_sound_volume >= 0) && (G_sound_volume <= 112))
  {
    G_sound_volume += 16;
    doremi_apply_volume();
    doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** doremi_decrease_volume()
*******************************************************************************/
short int doremi_decrease_volume()
{
  if ((G_sound_volume >= 16) && (G_sound_volume <= 128))
  {
    G_sound_volume -= 16;
    doremi_apply_volume();
    doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

