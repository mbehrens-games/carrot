/*******************************************************************************
** fade.c (fade-out and fade-in functions)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "fade.h"
#include "screen.h"

#define FADE_STEP_LENGTH  2
#define FADE_NUM_STEPS    8

int G_fade_screen;
int G_fade_alternate;
int G_fade_page;
int G_fade_choice;

int G_fade_state;

GLfloat G_fade_amount;
GLfloat G_fade_panels;

static unsigned int S_fade_timer;
static int          S_fade_step;

static GLfloat S_fade_amount_table[FADE_NUM_STEPS];

/*******************************************************************************
** fade_init_variables()
*******************************************************************************/
short int fade_init_variables()
{
  int k;

  /* fade amount table */
  /* finding shift amounts, each the width of a pixel on the texture  */
  /* on a 256x256 texture, the pixel size is 1/256, so the shift      */
  /* amounts are multiples of 1/256 = 0.00390625                      */
  for (k = 0; k < FADE_NUM_STEPS; k++)
    S_fade_amount_table[k] = (k + 1) * 0.00390625f;

  G_fade_screen = GAME_SCREEN_TITLE;
  G_fade_alternate = 0;
  G_fade_page = 0;
  G_fade_choice = 0;

  G_fade_state = FADE_STATE_OFF;

  G_fade_amount = 0.0f;
  G_fade_panels = S_fade_amount_table[(FADE_NUM_STEPS / 2) - 1];

  S_fade_timer = 0;
  S_fade_step = 0;

  return 0;
}

/*******************************************************************************
** fade_start_fadein()
*******************************************************************************/
short int fade_start_fadein()
{
  G_fade_state = FADE_STATE_FADEIN;
  G_fade_amount = S_fade_amount_table[FADE_NUM_STEPS - 1];

  S_fade_timer = 0;
  S_fade_step = 0;

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

  G_fade_state = FADE_STATE_FADEOUT;
  G_fade_amount = S_fade_amount_table[0];

  S_fade_timer = 0;
  S_fade_step = 0;

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
  S_fade_timer += 1;

  /* determine frame */
  S_fade_step = S_fade_timer / FADE_STEP_LENGTH;

  /* determine fade amount based on current frame */
  if (G_fade_state == FADE_STATE_FADEOUT)
  {
    if ((S_fade_step >= 0) && (S_fade_step < FADE_NUM_STEPS))
      G_fade_amount = S_fade_amount_table[S_fade_step];
    else
    {
      G_fade_state = FADE_STATE_FADEIN;
      G_fade_amount = S_fade_amount_table[FADE_NUM_STEPS - 1];

      S_fade_timer = 0;
      S_fade_step = 0;
    }
  }
  else if (G_fade_state == FADE_STATE_FADEIN)
  {
    if ((S_fade_step >= 0) && (S_fade_step < FADE_NUM_STEPS))
      G_fade_amount = S_fade_amount_table[FADE_NUM_STEPS - 1 - S_fade_step];
    else
    {
      G_fade_state = FADE_STATE_OFF;
      G_fade_amount = 0.0f;

      S_fade_timer = 0;
      S_fade_step = 0;
    }
  }

  return 0;
}

