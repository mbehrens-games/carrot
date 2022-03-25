/*******************************************************************************
** texture.h (opengl texture management)
*******************************************************************************/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#define PALETTE_SIZE    64
#define SUBPALETTE_SIZE 8

#define TEXTURE_SIZE    512

#define NUM_TEXTURES    1

extern GLuint G_palette_id;
extern GLuint G_texture_ids[];

extern GLint  G_subpalette_size;

/* function declarations */
short int textures_init();
short int textures_deinit();

short int texture_generate_palette();
short int texture_load_all_from_file(char* filename);

#endif
