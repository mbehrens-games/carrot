/*******************************************************************************
** fade.c (fade-out and fade-in functions)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "fade.h"
#include "screen.h"

#define FADE_FRAME_LENGTH   4
#define FADE_NUM_FRAMES     4

int           G_fade_screen;
int           G_fade_alternate;
int           G_fade_page;
int           G_fade_choice;

unsigned int  G_fade_timer;
int           G_fade_state;
int           G_fade_frame;
GLint         G_fade_amount;

static int    S_fade_amount_table[FADE_NUM_FRAMES];

/*******************************************************************************
** fade_init_variables()
*******************************************************************************/
short int fade_init_variables()
{
  G_fade_screen = GAME_SCREEN_TITLE;
  G_fade_alternate = 0;
  G_fade_page = 0;
  G_fade_choice = 0;

  G_fade_timer = 0;
  G_fade_state = FADE_STATE_OFF;
  G_fade_frame = 0;
  G_fade_amount = 0;

  S_fade_amount_table[0] = 1;
  S_fade_amount_table[1] = 2;
  S_fade_amount_table[2] = 3;
  S_fade_amount_table[3] = 4;

  return 0;
}

/*******************************************************************************
** fade_start_fadein()
*******************************************************************************/
short int fade_start_fadein()
{
  G_fade_timer = 0;
  G_fade_state = FADE_STATE_FADEIN;
  G_fade_frame = 0;
  G_fade_amount = S_fade_amount_table[FADE_NUM_FRAMES - 1];

  return 0;
}

/*******************************************************************************
** fade_start_transition()
*******************************************************************************/
short int fade_start_transition(int screen, int alternate, int page, int choice)
{
  if ((screen < 0) || (screen >= GAME_NUM_SCREENS))
    return 0;

  G_fade_screen = screen;
  G_fade_alternate = alternate;
  G_fade_page = page;
  G_fade_choice = choice;

  G_fade_timer = 0;
  G_fade_state = FADE_STATE_FADEOUT;
  G_fade_frame = 0;
  G_fade_amount = S_fade_amount_table[0];

  return 0;
}

/*******************************************************************************
** fade_update_transition()
*******************************************************************************/
short int fade_update_transition()
{
  if (G_fade_state == FADE_STATE_OFF)
    return 0;

  /* increment fade timer */
  G_fade_timer += 1;

  /* determine frame */
  G_fade_frame = G_fade_timer / FADE_FRAME_LENGTH;

  /* determine fade amount based on current frame */
  if (G_fade_state == FADE_STATE_FADEOUT)
  {
    if ((G_fade_frame >= 0) && (G_fade_frame < FADE_NUM_FRAMES))
      G_fade_amount = S_fade_amount_table[G_fade_frame];
    else
    {
      G_fade_timer = 0;
      G_fade_state = FADE_STATE_FADEIN;
      G_fade_frame = 0;
      G_fade_amount = S_fade_amount_table[FADE_NUM_FRAMES - 1];
    }
  }
  else if (G_fade_state == FADE_STATE_FADEIN)
  {
    if ((G_fade_frame >= 0) && (G_fade_frame < FADE_NUM_FRAMES))
      G_fade_amount = S_fade_amount_table[FADE_NUM_FRAMES - 1 - G_fade_frame];
    else
    {
      G_fade_timer = 0;
      G_fade_state = FADE_STATE_OFF;
      G_fade_frame = 0;
      G_fade_amount = 0;
    }
  }

  return 0;
}

