/*******************************************************************************
** texture.h (opengl graphics rom data texture)
*******************************************************************************/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#define TEXTURE_SIZE 512

extern GLuint G_texture_id_rom_data;

extern GLfloat G_texture_coord_table[65];

/* function declarations */
short int texture_generate_coord_tables();

short int texture_init();
short int texture_deinit();

short int texture_load_all_from_file(char* filename);

#endif
