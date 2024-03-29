/*******************************************************************************
** fade.h (fade-out and fade-in functions)
*******************************************************************************/

#ifndef FADE_H
#define FADE_H

#include <glad/glad.h>

enum
{
  FADE_STATE_OFF = 0,
  FADE_STATE_FADEOUT,
  FADE_STATE_FADEIN
};

extern int G_fade_screen;
extern int G_fade_alternate;
extern int G_fade_page;
extern int G_fade_choice;

extern int G_fade_state;

extern GLfloat G_fade_amount;
extern GLfloat G_fade_panels;

/* function declarations */
short int fade_init_variables();

short int fade_start_fadein();

short int fade_start_transition(int screen, int alternate, int page, int choice);
short int fade_update_transition();

#endif
