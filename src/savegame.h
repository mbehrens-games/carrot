/*******************************************************************************
** savegame.h (save games)
*******************************************************************************/

#ifndef SAVE_GAME_H
#define SAVE_GAME_H

#define SAVE_GAME_NUM_WORLDS  12
#define SAVE_GAME_NUM_ROOMS   96

#define SAVE_GAME_ROOMS_PER_WORLD (SAVE_GAME_NUM_ROOMS / SAVE_GAME_NUM_WORLDS)

extern int G_save_game_current_slot;
extern int G_save_game_percent;

extern unsigned char G_save_game_data[SAVE_GAME_NUM_WORLDS];

/* function declarations */
short int save_game_set_slot(int num);

short int save_game_load();
short int save_game_save();
short int save_game_clear();

short int save_game_compute_percent();
short int save_game_mark_current_room_complete();

#endif
