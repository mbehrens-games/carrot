/*******************************************************************************
** texture.c (opengl texture management)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "texture.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

GLuint  G_palette_id;
GLuint  G_texture_ids[NUM_TEXTURES];

GLint   G_subpalette_size;

/* the luma is the average of the low and high voltages */
/* for the 1st half of each table, the low value is 0   */
/* for the 2nd half of each table, the high value is 1  */
/* the saturation is half of the peak-to-peak voltage   */

float S_approx_nes_lum[4] = {0.20f, 0.35f, 0.65f, 0.85f};
float S_approx_nes_sat[4] = {0.20f, 0.35f, 0.35f, 0.15f};

/*******************************************************************************
** textures_init()
*******************************************************************************/
short int textures_init()
{
  int i;

  G_palette_id = 0;

  for (i = 0; i < NUM_TEXTURES; i++)
    G_texture_ids[i] = 0;

  return 0;
}

/*******************************************************************************
** textures_deinit()
*******************************************************************************/
short int textures_deinit()
{
  int i;

  /* clear palette id from opengl */
  if (G_palette_id != 0)
  {
    glDeleteTextures(1, &(G_palette_id));
    G_palette_id = 0;
  }

  /* clear all texture ids from opengl */
  for (i = 0; i < NUM_TEXTURES; i++)
  {
    if (G_texture_ids[i] != 0)
    {
      glDeleteTextures(1, &(G_texture_ids[i]));
      G_texture_ids[i] = 0;
    }
  }

  return 0;
}

/*******************************************************************************
** texture_generate_palette()
*******************************************************************************/
short int texture_generate_palette()
{
  int       m;
  int       n;
  int       k;

  GLubyte   gradients[13][4][3];

  float     y;
  float     i;
  float     q;

  int       r;
  int       g;
  int       b;

  GLubyte*  palette_data;

  /* generate greys */
  for (n = 0; n < 4; n++)
  {
    gradients[0][n][0] = (int) ((S_approx_nes_lum[n] * 255) + 0.5f);
    gradients[0][n][1] = (int) ((S_approx_nes_lum[n] * 255) + 0.5f);
    gradients[0][n][2] = (int) ((S_approx_nes_lum[n] * 255) + 0.5f);
  }

  /* generate each hue */
  for (m = 0; m < 12; m++)
  {
    for (n = 0; n < 4; n++)
    {
      y = S_approx_nes_lum[n];
      i = S_approx_nes_sat[n] * cos(TWO_PI * (m * 30) / 360.0f);
      q = S_approx_nes_sat[n] * sin(TWO_PI * (m * 30) / 360.0f);

      r = (int) (((y + (i * 0.956f) + (q * 0.619f)) * 255) + 0.5f);
      g = (int) (((y - (i * 0.272f) - (q * 0.647f)) * 255) + 0.5f);
      b = (int) (((y - (i * 1.106f) + (q * 1.703f)) * 255) + 0.5f);

      /* bound rgb values */
      if (r < 0)
        r = 0;
      else if (r > 255)
        r = 255;

      if (g < 0)
        g = 0;
      else if (g > 255)
        g = 255;

      if (b < 0)
        b = 0;
      else if (b > 255)
        b = 255;

      gradients[m + 1][n][0] = r;
      gradients[m + 1][n][1] = g;
      gradients[m + 1][n][2] = b;
    }
  }

  /* allocate palette data */
  palette_data = malloc(sizeof(GLubyte) * 4 * PALETTE_SIZE * PALETTE_SIZE);

  /* initialize palette data */
  for (n = 0; n < PALETTE_SIZE * PALETTE_SIZE; n++)
  {
    palette_data[4 * n + 0] = 0;
    palette_data[4 * n + 1] = 0;
    palette_data[4 * n + 2] = 0;
    palette_data[4 * n + 3] = 0;
  }

  /* generate palette 0 */

  /* transparency color */
  palette_data[4 * (4 * 64 + 0) + 0] = 0;
  palette_data[4 * (4 * 64 + 0) + 1] = 0;
  palette_data[4 * (4 * 64 + 0) + 2] = 0;
  palette_data[4 * (4 * 64 + 0) + 3] = 0;

  /* black */
  palette_data[4 * (4 * 64 + 1) + 0] = 0;
  palette_data[4 * (4 * 64 + 1) + 1] = 0;
  palette_data[4 * (4 * 64 + 1) + 2] = 0;
  palette_data[4 * (4 * 64 + 1) + 3] = 255;

  /* greys */
  for (n = 0; n < 4; n++)
  {
    palette_data[4 * (4 * 64 + n + 2) + 0] = gradients[0][n][0];
    palette_data[4 * (4 * 64 + n + 2) + 1] = gradients[0][n][1];
    palette_data[4 * (4 * 64 + n + 2) + 2] = gradients[0][n][2];
    palette_data[4 * (4 * 64 + n + 2) + 3] = 255;
  }

  /* white */
  palette_data[4 * (4 * 64 + 6) + 0] = 255;
  palette_data[4 * (4 * 64 + 6) + 1] = 255;
  palette_data[4 * (4 * 64 + 6) + 2] = 255;
  palette_data[4 * (4 * 64 + 6) + 3] = 255;

  /* hues */
  for (m = 0; m < 12; m++)
  {
    for (n = 0; n < 4; n++)
    {
      palette_data[4 * (4 * 64 + 7 + 4 * m + n) + 0] = gradients[m + 1][n][0];
      palette_data[4 * (4 * 64 + 7 + 4 * m + n) + 1] = gradients[m + 1][n][1];
      palette_data[4 * (4 * 64 + 7 + 4 * m + n) + 2] = gradients[m + 1][n][2];
      palette_data[4 * (4 * 64 + 7 + 4 * m + n) + 3] = 255;
    }
  }

  /* generate lighting levels for palette 0 */
  for (k = 0; k < 8; k++)
  {
    if (k == 4)
      continue;

    /* copy transparency color */
    memcpy(&palette_data[4 * (k * 64 + 0)], &palette_data[4 * (4 * 64 + 0)], 4);

    /* shadows */
    if (k < 4)
    {
      /* greys */
      for (m = 0; m < 4 - k + 1; m++)
        memcpy(&palette_data[4 * (k * 64 + m + 1)], &palette_data[4 * (4 * 64 + 1)], 4);

      memcpy(&palette_data[4 * (k * 64 + 4 - k + 2)], &palette_data[4 * (4 * 64 + 2)], 4 * (k + 1));

      /* hues */
      for (m = 0; m < 12; m++)
      {
        for (n = 0; n < 4 - k; n++)
          memcpy(&palette_data[4 * (k * 64 + 4 * m + 7 + n)], &palette_data[4 * (4 * 64 + 1)], 4);

        if (k != 0)
          memcpy(&palette_data[4 * (k * 64 + 4 * m + 7 + 4 - k)], &palette_data[4 * (4 * 64 + 4 * m + 7)], 4 * k);
      }
    }
    /* highlights */
    else if (k > 4)
    {
      /* greys */
      for (m = 0; m < k - 4 + 1; m++)
        memcpy(&palette_data[4 * (k * 64 + (6 - m))], &palette_data[4 * (4 * 64 + 6)], 4);

      memcpy(&palette_data[4 * (k * 64 + 1)], &palette_data[4 * (4 * 64 + (k - 4) + 1)], 4 * ((8 - k) + 1));

      /* hues */
      for (m = 0; m < 12; m++)
      {
        for (n = 0; n < k - 4; n++)
          memcpy(&palette_data[4 * (k * 64 + 4 * m + 7 + (3 - n))], &palette_data[4 * (4 * 64 + 6)], 4);

        memcpy(&palette_data[4 * (k * 64 + 4 * m + 7)], &palette_data[4 * (4 * 64 + 4 * m + 7 + (k - 4))], 4 * (8 - k));
      }
    }
  }

  /* generate palettes 1 - 5 (shift by 2 each time) */
  for (m = 1; m < 6; m++)
  {
    for (n = 0; n < 8; n++)
    {
      /* transparency & greys */
      memcpy(&palette_data[4 * ((8 * m + n) * 64 + 0)], &palette_data[4 * ((8 * (m - 1) + n) * 64 + 0)], 4 * 7);

      /* shifted back colors */
      memcpy(&palette_data[4 * ((8 * m + n) * 64 + 7)], &palette_data[4 * ((8 * (m - 1) + n) * 64 + 15)], 4 * 4 * 10);

      /* cycled around colors */
      memcpy(&palette_data[4 * ((8 * m + n) * 64 + 47)], &palette_data[4 * ((8 * (m - 1) + n) * 64 + 7)], 4 * 4 * 2);
    }
  }

  /* generate palette 6 (greyscale) */
  for (m = 0; m < 8; m++)
  {
    memcpy(&palette_data[4 * ((48 + m) * 64 + 0)], &palette_data[4 * (m * 64 + 0)], 4 * 7);

    for (n = 0; n < 12; n++)
      memcpy(&palette_data[4 * ((48 + m) * 64 + 4 * n + 7)], &palette_data[4 * (m * 64 + 2)], 4 * 4);
  }

  /* generate palette 7 (inverted greyscale) */
  for (m = 0; m < 8; m++)
  {
    memcpy(&palette_data[4 * ((56 + m) * 64 + 0)], &palette_data[4 * (m * 64 + 0)], 4);

    for (n = 1; n < 7; n++)
    {
      palette_data[4 * ((56 + m) * 64 + n) + 0] = palette_data[4 * (m * 64 + (7 - n)) + 0];
      palette_data[4 * ((56 + m) * 64 + n) + 1] = palette_data[4 * (m * 64 + (7 - n)) + 1];
      palette_data[4 * ((56 + m) * 64 + n) + 2] = palette_data[4 * (m * 64 + (7 - n)) + 2];
      palette_data[4 * ((56 + m) * 64 + n) + 3] = palette_data[4 * (m * 64 + (7 - n)) + 3];
    }

    for (n = 0; n < 12; n++)
      memcpy(&palette_data[4 * ((56 + m) * 64 + 4 * n + 7)], &palette_data[4 * ((56 + m) * 64 + 2)], 4 * 4);
  }

  /* assign gl texture id to the palette, set it to the current gl texture */
  glGenTextures(1, &G_palette_id);
  glBindTexture(GL_TEXTURE_2D, G_palette_id);

  /* set up texture in opengl */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  /* load the texture to opengl from the palette data */
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, PALETTE_SIZE, PALETTE_SIZE, 
                0, GL_RGBA, GL_UNSIGNED_BYTE, palette_data);

  /* clear palette data */
  if (palette_data != NULL)
  {
    free(palette_data);
    palette_data = NULL;
  }

  /* set subpalette size */
  G_subpalette_size = SUBPALETTE_SIZE;

  return 0;
}

/*******************************************************************************
** texture_load_all_from_file()
*******************************************************************************/
short int texture_load_all_from_file(char* filename)
{
  FILE* fp;

  int   i;

  int   m;
  int   n;

  char  signature[4];
  char  type[4];

  int   buffer[4];

  unsigned char*  image_data;
  GLfloat*        texture_data;

  if (filename == NULL)
    return 1;

  /* initialize variables */
  texture_data = NULL;

  /* open file */
  fp = fopen(filename, "rb");

  /* if file did not open, return error */
  if (fp == NULL)
  {
    fclose(fp);
    return 1;
  }

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

  if ((type[0] != 'G') || 
      (type[1] != 'R') || 
      (type[2] != 'P') || 
      (type[3] != 'H'))
  {
    fclose(fp);
    return 1;
  }

  /* allocate image data */
  image_data = malloc(sizeof(unsigned char) * (TEXTURE_SIZE * TEXTURE_SIZE / 4) * 3);

  /* allocate texture data */
  texture_data = malloc(sizeof(GLfloat) * TEXTURE_SIZE * TEXTURE_SIZE);

  /* cycle through textures */
  for (i = 0; i < NUM_TEXTURES; i++)
  {
    /* read image data from file */
    if (fread(image_data, 1, (TEXTURE_SIZE * TEXTURE_SIZE / 4) * 3, fp) == 0)
    {
      free(image_data);
      fclose(fp);
      return 1;
    }

    /* convert to floating point texture data */
    for (m = 0; m < TEXTURE_SIZE * TEXTURE_SIZE / 4; m++)
    {
      /* read 4 pixels from each set of 3 bytes         */
      /* (the pixels in the image data are 6 bits each) */
      buffer[0] = (image_data[3 * m + 0] >> 2) & 0x3F;
      buffer[1] = ((image_data[3 * m + 0] << 4) & 0x30) | ((image_data[3 * m + 1] >> 4) & 0x0F);
      buffer[2] = ((image_data[3 * m + 1] << 2) & 0x3C) | ((image_data[3 * m + 2] >> 6) & 0x03);
      buffer[3] = image_data[3 * m + 2] & 0x3F;

      /* finding coordinates at the center of a pixel on the palette texture  */
      /* on a 64x64 texture, the pixel size is 1/64, so the centers           */
      /* are at odd multiples of 1/128 = 0.0078125                            */
      for (n = 0; n < 4; n++)
        texture_data[4 * m + n] = (2 * buffer[n] + 1) * 0.0078125f;
    }

    /* assign gl texture id to this texture, set it to the current gl texture */
    glGenTextures(1, &(G_texture_ids[i]));
    glBindTexture(GL_TEXTURE_2D, G_texture_ids[i]);

    /* set up texture in opengl */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* load the texture to opengl from the texture data */
    glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F, TEXTURE_SIZE, TEXTURE_SIZE, 
                  0, GL_RED, GL_FLOAT, texture_data);
  }

  /* close file */
  fclose(fp);

  /* clear image data */
  if (image_data != NULL)
  {
    free(image_data);
    image_data = NULL;
  }

  /* clear texture data */
  if (texture_data != NULL)
  {
    free(texture_data);
    texture_data = NULL;
  }

  return 0;
}
