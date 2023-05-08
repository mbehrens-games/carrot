/*******************************************************************************
** parallax.h (opengl parallax texture)
*******************************************************************************/

#ifndef PARALLAX_H
#define PARALLAX_H

#include <glad/glad.h>

#define PARALLAX_SIZE 256

extern GLuint   G_texture_id_parallax;

extern GLint    G_parallax_hori_shift;
extern GLint    G_parallax_vert_shift;
extern GLfloat  G_parallax_vert_weight;

extern GLfloat  G_parallax_coord_table[16];

/* function declarations */
short int parallax_generate_coord_tables();

short int parallax_init();
short int parallax_deinit();

short int parallax_create_opengl_texture();

short int parallax_update_shift(int x_shift, int y_shift);

#endif
