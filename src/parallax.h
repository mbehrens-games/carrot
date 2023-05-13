/*******************************************************************************
** parallax.h (opengl parallax texture)
*******************************************************************************/

#ifndef PARALLAX_H
#define PARALLAX_H

#include <glad/glad.h>

#define PARALLAX_SIZE 256

#define PARALLAX_NUM_LAYERS 16

enum
{
  PARALLAX_SCROLLING_NONE,
  PARALLAX_SCROLLING_HALF,
  PARALLAX_SCROLLING_FULL,
  PARALLAX_SCROLLING_PER_SCANLINE
};

extern GLuint G_texture_id_parallax;

extern GLint G_parallax_hori_shift;
extern GLint G_parallax_vert_shift;

extern GLfloat G_parallax_coord_table[PARALLAX_NUM_LAYERS + 1];

/* function declarations */
short int parallax_generate_coord_tables();

short int parallax_init();
short int parallax_deinit();

short int parallax_create_opengl_texture();

short int parallax_set_hori_scrolling(int scrolling);
short int parallax_set_vert_scrolling(int scrolling);

short int parallax_increment_shift(int dx, int dy);

#endif
