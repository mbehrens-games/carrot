/*******************************************************************************
** parallax.h (opengl parallax texture)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "parallax.h"

GLuint  G_texture_id_parallax;

GLint   G_parallax_hori_shift;
GLint   G_parallax_vert_shift;
GLfloat G_parallax_vert_weight;

GLfloat G_parallax_coord_table[16];

static GLfloat* S_parallax_data;

/*******************************************************************************
** parallax_generate_coord_tables()
*******************************************************************************/
short int parallax_generate_coord_tables()
{
  int k;

  /* parallax coordinate table */
  /* these are scaling factors on the reversal of the shift */
  /* that was applied to the sky layer by the mvp matrix.   */
  /* a scaling factor of 0.0 would mean that the sky layer  */
  /* scrolling matches the foreground exactly.              */
  /* a scaling factor of 1.0 would mean that the sky layer  */
  /* remains stationary as the foreground scrolls.          */
  for (k = 0; k < 16; k++)
    G_parallax_coord_table[k] = 1.0f - (k / 16.0f);

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
  G_parallax_vert_weight = 1.0f;

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
** parallax_update_shift()
*******************************************************************************/
short int parallax_update_shift(int x_shift, int y_shift)
{
  if (x_shift < 0)
    G_parallax_hori_shift = x_shift + 320;
  else
    G_parallax_hori_shift = x_shift;

  if (y_shift < 0)
    G_parallax_vert_shift = y_shift + 224;
  else
    G_parallax_vert_shift = y_shift;

  return 0;
}

