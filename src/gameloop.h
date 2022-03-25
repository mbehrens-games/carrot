/*******************************************************************************
** gameloop.h (main game loop functions)
*******************************************************************************/

#ifndef GAME_LOOP_H
#define GAME_LOOP_H

/* function declarations */
short int game_loop_load_room(int index);
short int game_loop_reset_room();

short int game_loop_change_screen(int screen, int alternate, int page, int choice);

short int game_loop_advance_frame();

#endif
