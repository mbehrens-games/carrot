/*******************************************************************************
** bunny.h (bunny update functions)
*******************************************************************************/

#ifndef BUNNY_H
#define BUNNY_H

/* function declarations */

/* moving */
short int bunny_player_input_d_pad_left_on();
short int bunny_player_input_d_pad_left_off();

short int bunny_player_input_d_pad_right_on();
short int bunny_player_input_d_pad_right_off();

/* climbing */
short int bunny_player_input_d_pad_up_on();
short int bunny_player_input_d_pad_up_off();

short int bunny_player_input_d_pad_down_on();
short int bunny_player_input_d_pad_down_off();

/* throwing, dropping, and kicking */
short int bunny_player_input_d_pad_up_on_and_button_b_pressed();
short int bunny_player_input_d_pad_down_on_and_button_b_pressed();
short int bunny_player_input_d_pad_left_on_and_button_b_pressed();
short int bunny_player_input_d_pad_right_on_and_button_b_pressed();
short int bunny_player_input_d_pad_neutral_and_button_b_pressed();

/* grabbing */
short int bunny_player_input_always_when_button_b_pressed();

/* jumping */
short int bunny_player_input_button_a_pressed();
short int bunny_player_input_button_a_released();

short int bunny_move_bunny();

#endif
