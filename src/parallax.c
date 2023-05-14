/*******************************************************************************
** parallax.h (opengl parallax texture)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graphics.h"
#include "parallax.h"
#include "subpixel.h"

#define PARALLAX_SCROLL_WIDTH   (GRAPHICS_OVERSCAN_WIDTH * PARALLAX_NUM_LAYERS * SUBPIXEL_MANTISSA_FULL)
#define PARALLAX_SCROLL_HEIGHT  (GRAPHICS_OVERSCAN_HEIGHT * SUBPIXEL_MANTISSA_FULL)

GLuint G_texture_id_parallax;

GLint G_parallax_hori_shift;
GLint G_parallax_vert_shift;

GLfloat G_parallax_coord_table[PARALLAX_NUM_LAYERS + 1];

static GLfloat* S_parallax_data;

static int S_parallax_hori_scrolling;
static int S_parallax_vert_scrolling;

static int S_parallax_hori_subpixels;
static int S_parallax_vert_subpixels;

/*******************************************************************************
** parallax_generate_coord_tables()
*******************************************************************************/
short int parallax_generate_coord_tables()
{
  int k;

  /* parallax coordinate table */
  /* these are scaling factors on the horizontal  */
  /* shift applied to the sky layer. a scaling    */
  /* factor of 1.0 means that the scanline        */
  /* matches the foreground scrolling exactly.    */
  G_parallax_coord_table[0] = 0.0f;

  for (k = 1; k < PARALLAX_NUM_LAYERS; k++)
    G_parallax_coord_table[k] = ((float) k) / PARALLAX_NUM_LAYERS;

  G_parallax_coord_table[PARALLAX_NUM_LAYERS] = 1.0f;

  return 0;
}

/*******************************************************************************
** parallax_init()
*******************************************************************************/
short int parallax_init()
{
  int k;

  G_texture_id_parallax = 0;

  /* allocate parallax data */
  S_parallax_data = malloc(sizeof(GLfloat) * 1 * PARALLAX_SIZE);

  /* initialize parallax data */
  for (k = 0; k < PARALLAX_SIZE; k++)
  {
    S_parallax_data[1 * k + 0] = 0.0f;
  }

  /* initialize variables (for the sky_parallax shader) */
  G_parallax_hori_shift = 0;
  G_parallax_vert_shift = 0;

  S_parallax_hori_scrolling = PARALLAX_SCROLLING_NONE;
  S_parallax_vert_scrolling = PARALLAX_SCROLLING_NONE;

  S_parallax_hori_subpixels = 0;
  S_parallax_vert_subpixels = 0;

  return 0;
}

/*******************************************************************************
** parallax_deinit()
*******************************************************************************/
short int parallax_deinit()
{
  /* clear parallax id from opengl */
  if (G_texture_id_parallax != 0)
  {
    glDeleteTextures(1, &(G_texture_id_parallax));
    G_texture_id_parallax = 0;
  }

  /* clear parallax data */
  if (S_parallax_data != NULL)
  {
    free(S_parallax_data);
    S_parallax_data = NULL;
  }

  return 0;
}

/*******************************************************************************
** parallax_create_opengl_texture()
*******************************************************************************/
short int parallax_create_opengl_texture()
{
  int k;

  /* initialize parallax data */
  for (k = 0; k < PARALLAX_SIZE; k++)
  {
    S_parallax_data[1 * k + 0] = G_parallax_coord_table[0];
  }

  /* assign gl texture id to the parallax, set it to the current gl texture */
  glGenTextures(1, &G_texture_id_parallax);
  glBindTexture(GL_TEXTURE_1D, G_texture_id_parallax);

  /* set up texture in opengl */
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

  /* load the texture to opengl from the parallax data */
  glTexImage1D( GL_TEXTURE_1D, 0, GL_R32F, PARALLAX_SIZE, 
                0, GL_RED, GL_FLOAT, S_parallax_data);

  return 0;
}

/*******************************************************************************
** parallax_set_hori_scrolling()
*******************************************************************************/
short int parallax_set_hori_scrolling(int scrolling)
{
  int k;

  /* horizontal scrolling */
  if ((scrolling == PARALLAX_SCROLLING_NONE) || 
      (scrolling == PARALLAX_SCROLLING_HALF) || 
      (scrolling == PARALLAX_SCROLLING_FULL))
  {
    S_parallax_hori_scrolling = scrolling;
  }
  else
    S_parallax_hori_scrolling = PARALLAX_SCROLLING_NONE;

  /* update parallax texture */
  for (k = 0; k < PARALLAX_SIZE; k++)
  {
    if (S_parallax_hori_scrolling == PARALLAX_SCROLLING_NONE)
      S_parallax_data[1 * k + 0] = G_parallax_coord_table[0];
    else if (S_parallax_hori_scrolling == PARALLAX_SCROLLING_HALF)
      S_parallax_data[1 * k + 0] = G_parallax_coord_table[PARALLAX_NUM_LAYERS / 2];
    else if (S_parallax_hori_scrolling == PARALLAX_SCROLLING_FULL)
      S_parallax_data[1 * k + 0] = G_parallax_coord_table[PARALLAX_NUM_LAYERS];
    else
      S_parallax_data[1 * k + 0] = G_parallax_coord_table[0];
  }

  glBindTexture(GL_TEXTURE_1D, G_texture_id_parallax);
  glTexSubImage1D(GL_TEXTURE_1D, 0, 0, PARALLAX_SIZE, 
                  GL_RED, GL_FLOAT, S_parallax_data);

  return 0;
}

/*******************************************************************************
** parallax_set_vert_scrolling()
*******************************************************************************/
short int parallax_set_vert_scrolling(int scrolling)
{
  /* vertical scrolling */
  if ((scrolling == PARALLAX_SCROLLING_NONE) || 
      (scrolling == PARALLAX_SCROLLING_HALF) || 
      (scrolling == PARALLAX_SCROLLING_FULL))
  {
    S_parallax_vert_scrolling = scrolling;
  }
  else
    S_parallax_vert_scrolling = PARALLAX_SCROLLING_NONE;

  return 0;
}

/*******************************************************************************
** parallax_increment_shift()
*******************************************************************************/
short int parallax_increment_shift(int dx, int dy)
{
  /* add increments to the current scroll position (in subpixels) */
  S_parallax_hori_subpixels += dx;

  if (S_parallax_vert_scrolling == PARALLAX_SCROLLING_NONE)
    S_parallax_vert_subpixels += 0;
  else if (S_parallax_vert_scrolling == PARALLAX_SCROLLING_HALF)
    S_parallax_vert_subpixels += dy / 2;
  else if (S_parallax_vert_scrolling == PARALLAX_SCROLLING_FULL)
    S_parallax_vert_subpixels += dy;
  else
    S_parallax_vert_subpixels += 0;

  /* wraparound */
  if (S_parallax_hori_subpixels < 0)
    S_parallax_hori_subpixels += PARALLAX_SCROLL_WIDTH;
  else
    S_parallax_hori_subpixels = S_parallax_hori_subpixels % PARALLAX_SCROLL_WIDTH;

  if (S_parallax_vert_subpixels < 0)
    S_parallax_vert_subpixels += PARALLAX_SCROLL_HEIGHT;
  else
    S_parallax_vert_subpixels = S_parallax_vert_subpixels % PARALLAX_SCROLL_HEIGHT;

  /* determine scroll position (in pixels) */
  G_parallax_hori_shift = (S_parallax_hori_subpixels + SUBPIXEL_MANTISSA_HALF) / SUBPIXEL_MANTISSA_FULL;
  G_parallax_vert_shift = (S_parallax_vert_subpixels + SUBPIXEL_MANTISSA_HALF) / SUBPIXEL_MANTISSA_FULL;

  return 0;
}

