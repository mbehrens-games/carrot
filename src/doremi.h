/*******************************************************************************
** doremi.h (sound and music)
*******************************************************************************/

#ifndef DOREMI_H
#define DOREMI_H

#ifdef CARROT_MAC_OSX
  #include <SDL2_mixer/SDL_mixer.h>
#else
  #include <SDL2/SDL_mixer.h>
#endif

enum
{
  SFX_INDEX_BUBBLE_APPEAR = 0,
  SFX_INDEX_BUBBLE_HIT,
  SFX_INDEX_BUBBLE_POPPED,
  SFX_INDEX_BUNNY_DROP,
  SFX_INDEX_BUNNY_GRAB,
  SFX_INDEX_BUNNY_KICK,
  SFX_INDEX_BUNNY_LANDED,
  SFX_INDEX_BUNNY_THROW,
  SFX_INDEX_DIAMOND_BLOCK,
  SFX_INDEX_HEART_BLOCK,
  SFX_INDEX_KABOOM,
  SFX_INDEX_LEVEL_COMPLETE,
  SFX_INDEX_MAGI_BLOCK_FAST,
  SFX_INDEX_MAGI_BLOCK_NORMAL,
  SFX_INDEX_MARBLE_CHANGING_TO_METAL,
  SFX_INDEX_MARBLE_CHANGING_TO_WILD,
  SFX_INDEX_MARBLE_DROP,
  SFX_INDEX_MARBLE_FLASHING,
  SFX_INDEX_MENU_SELECT,
  SFX_INDEX_PAUSE,
  SFX_INDEX_PICKUP_BOOK_RIGHTSIDE_UP,
  SFX_INDEX_PICKUP_BOOK_UPSIDE_DOWN,
  SFX_INDEX_PICKUP_CARROT,
  SFX_INDEX_PICKUP_KEY,
  SFX_INDEX_PICKUP_MAGNET,
  SFX_INDEX_PICKUP_MUSHROOM,
  SFX_INDEX_PLATFORM_OFF,
  SFX_INDEX_PLATFORM_ON,
  SFX_INDEX_THROWN_THING_ARROWS,
  SFX_INDEX_THROWN_THING_BOUNCE,
  SFX_INDEX_THROWN_THING_LANDED,
  SFX_INDEX_TOGGLE_DOWN,
  SFX_INDEX_TOGGLE_UP,
  SFX_NUM_INDICES
};

enum
{
  MUSIC_TRACK_MENU = 0,
  MUSIC_TRACK_STORY,
  MUSIC_TRACK_WORLD_01,
  MUSIC_TRACK_WORLD_02,
  MUSIC_TRACK_WORLD_03,
  MUSIC_TRACK_WORLD_04,
  MUSIC_TRACK_WORLD_05,
  MUSIC_TRACK_WORLD_06,
  MUSIC_TRACK_WORLD_07,
  MUSIC_TRACK_WORLD_08,
  MUSIC_TRACK_WORLD_09,
  MUSIC_TRACK_WORLD_10,
  MUSIC_TRACK_WORLD_11,
  MUSIC_TRACK_WORLD_12,
  MUSIC_TRACK_NONE
};

extern Mix_Chunk  G_sfx_all[SFX_NUM_INDICES];
extern Mix_Music  *G_music;

extern int        G_sound_volume;

/* function declarations */
short int doremi_init_all_sfx();
short int doremi_deinit_all_sfx();
short int doremi_load_all_sfx_from_file(char* filename);
short int doremi_play_sfx(int type);

short int doremi_init_music();
short int doremi_deinit_music();
short int doremi_play_music(int track);
short int doremi_pause_music();
short int doremi_unpause_music();

short int doremi_set_volume(int steps);
short int doremi_increase_volume();
short int doremi_decrease_volume();

#endif
