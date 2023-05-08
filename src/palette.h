/*******************************************************************************
** palette.h (opengl palette texture)
*******************************************************************************/

#ifndef PALETTE_H
#define PALETTE_H

#include <glad/glad.h>

#define PALETTE_SIZE 256

extern GLuint G_texture_id_palette;

extern GLint G_palette_levels;

extern GLfloat G_lighting_coord_table[16];
extern GLfloat G_palette_coord_table[16];

/* function declarations */
short int palette_generate_coord_tables();

short int palette_init();
short int palette_deinit();

short int palette_create_opengl_texture();

#endif
