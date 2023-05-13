/*******************************************************************************
** shaders.c (opengl shaders)
*******************************************************************************/

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "path.h"
#include "shaders.h"

/* shader program and uniform ids */
GLuint G_program_id_OV_A;
GLuint G_program_id_OV_B;
GLuint G_program_id_OV_C;
GLuint G_program_id_OV_D;
GLuint G_program_id_OV_E;

GLuint G_program_id_UP_A;
GLuint G_program_id_UP_B;
GLuint G_program_id_UP_C;
GLuint G_program_id_UP_D;
GLuint G_program_id_UP_E;

GLuint G_uniform_OV_A_mvp_matrix_id;
GLuint G_uniform_OV_A_texture_sampler_id;

GLuint G_uniform_OV_B_mvp_matrix_id;
GLuint G_uniform_OV_B_texture_sampler_id;
GLuint G_uniform_OV_B_parallax_sampler_id;
GLuint G_uniform_OV_B_hori_shift_id;
GLuint G_uniform_OV_B_vert_shift_id;

GLuint G_uniform_OV_C_mvp_matrix_id;
GLuint G_uniform_OV_C_texture_sampler_id;

GLuint G_uniform_OV_D_mvp_matrix_id;
GLuint G_uniform_OV_D_texture_sampler_id;
GLuint G_uniform_OV_D_amount_id;

GLuint G_uniform_OV_E_mvp_matrix_id;
GLuint G_uniform_OV_E_texture_sampler_id;
GLuint G_uniform_OV_E_palette_sampler_id;
GLuint G_uniform_OV_E_levels_id;

GLuint G_uniform_UP_A_mvp_matrix_id;
GLuint G_uniform_UP_A_texture_sampler_id;
GLuint G_uniform_UP_A_rgb2yiq_matrix_id;
GLuint G_uniform_UP_A_yiq2rgb_matrix_id;
GLuint G_uniform_UP_A_black_level_id;
GLuint G_uniform_UP_A_white_level_id;

GLuint G_uniform_UP_B_mvp_matrix_id;
GLuint G_uniform_UP_B_texture_sampler_id;

GLuint G_uniform_UP_C_mvp_matrix_id;
GLuint G_uniform_UP_C_texture_sampler_id;

GLuint G_uniform_UP_D_mvp_matrix_id;
GLuint G_uniform_UP_D_texture_sampler_id;
GLuint G_uniform_UP_D_cubic_matrix_id;

GLuint G_uniform_UP_E_mvp_matrix_id;
GLuint G_uniform_UP_E_texture_sampler_id;

/*******************************************************************************
** shaders_compile_program()
*******************************************************************************/
short int shaders_compile_program(GLuint  program_id, 
                                  GLchar* vert_source_string, 
                                  GLint   vert_source_length, 
                                  GLchar* frag_source_string, 
                                  GLint   frag_source_length)
{
  GLuint  vertex_shader_id;
  GLuint  fragment_shader_id;

  const GLchar* string_array[1];
  GLint         length_array[1];

  GLint   compile_status;
  GLint   link_status;
  char    info_log[256];

  /* create and compile vertex shader */
  vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

  string_array[0] = vert_source_string;
  length_array[0] = vert_source_length;

  glShaderSource(vertex_shader_id, 1, string_array, length_array);
  glCompileShader(vertex_shader_id);

  /* make sure the shader compiled correctly */
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &compile_status);

  if (compile_status != GL_TRUE)
  {
    fprintf(stdout, "Failed to compile vertex shader.\n");

    glGetShaderInfoLog(vertex_shader_id, 256, NULL, info_log);
    fprintf(stdout, "Info Log: %s\n", info_log);

    glDeleteShader(vertex_shader_id);

    return 1;
  }

  /* create and compile fragment shader */
  fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  string_array[0] = frag_source_string;
  length_array[0] = frag_source_length;

  glShaderSource(fragment_shader_id, 1, string_array, length_array);
  glCompileShader(fragment_shader_id);

  /* make sure the shader compiled correctly */
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &compile_status);

  if (compile_status != GL_TRUE)
  {
    fprintf(stdout, "Failed to compile fragment shader.\n");

    glGetShaderInfoLog(fragment_shader_id, 256, NULL, info_log);
    fprintf(stdout, "Info Log: %s\n", info_log);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return 1;
  }

  /* link program */
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glLinkProgram(program_id);

  /* make sure the program was linked correctly */
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

  if (link_status != GL_TRUE)
  {
    fprintf(stdout, "Failed to link OpenGL program.\n");

    glGetProgramInfoLog(program_id, 256, NULL, info_log);
    fprintf(stdout, "Info Log: %s\n", info_log);

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    glDeleteProgram(program_id);
    return 1;
  }

  /* cleanup */
  glDetachShader(program_id, vertex_shader_id);
  glDetachShader(program_id, fragment_shader_id);

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  return 0;
}

/*******************************************************************************
** shaders_load_shaders()
*******************************************************************************/
short int shaders_load_shaders(char* filename)
{
  FILE*         fp;
  int           i;
  char          signature[8];

  GLchar*       vert_source_string[SHADERS_NUM_PROGRAMS];
  GLint         vert_source_length[SHADERS_NUM_PROGRAMS];

  GLchar*       frag_source_string[SHADERS_NUM_PROGRAMS];
  GLint         frag_source_length[SHADERS_NUM_PROGRAMS];

  if (filename == NULL)
    return 1;

  /* open file */
  fp = fopen(filename, "rb");

  /* if file did not open, return error */
  if (fp == NULL)
    return 1;

  /* read signature */
  if (fread(signature, 1, 8, fp) < 8)
  {
    fclose(fp);
    return 1;
  }

  if ((signature[0] != 'S') || 
      (signature[1] != 'H') || 
      (signature[2] != 'A') || 
      (signature[3] != 'D') || 
      (signature[4] != 'E') || 
      (signature[5] != 'R') || 
      (signature[6] != '2') || 
      (signature[7] != 'A'))
  {
    fclose(fp);
    return 1;
  }

  /* initialize source variables */
  for (i = 0; i < SHADERS_NUM_PROGRAMS; i++)
  {
    vert_source_string[i] = NULL;
    vert_source_length[i] = 0;

    frag_source_string[i] = NULL;
    frag_source_length[i] = 0;
  }

  /* read shader sources */
  for (i = 0; i < SHADERS_NUM_PROGRAMS; i++)
  {
    /* vertex shader */
    if (fread(&vert_source_length[i], 4, 1, fp) == 0)
    {
      fclose(fp);
      return 1;
    }

    vert_source_string[i] = malloc(sizeof(GLchar) * (unsigned int) vert_source_length[i]);

    if (fread(vert_source_string[i], 1, vert_source_length[i], fp) < (unsigned int) vert_source_length[i])
    {
      fclose(fp);
      return 1;
    }

    /* fragment shader */
    if (fread(&frag_source_length[i], 4, 1, fp) == 0)
    {
      fclose(fp);
      return 1;
    }

    frag_source_string[i] = malloc(sizeof(GLchar) * (unsigned int) frag_source_length[i]);

    if (fread(frag_source_string[i], 1, frag_source_length[i], fp) < (unsigned int) frag_source_length[i])
    {
      fclose(fp);
      return 1;
    }
  }

  /* close file */
  fclose(fp);

  /* create and compile programs */

  /* draw tiles program */
  G_program_id_OV_A = glCreateProgram();

  if (shaders_compile_program( G_program_id_OV_A, 
                                vert_source_string[SHADERS_PROGRAM_OV_A],
                                vert_source_length[SHADERS_PROGRAM_OV_A],
                                frag_source_string[SHADERS_PROGRAM_OV_A],
                                frag_source_length[SHADERS_PROGRAM_OV_A]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (draw tiles).\n");
    glDeleteProgram(G_program_id_OV_A);
    return 1;
  }

  /* sky parallax program */
  G_program_id_OV_B = glCreateProgram();

  if (shaders_compile_program( G_program_id_OV_B, 
                                vert_source_string[SHADERS_PROGRAM_OV_B],
                                vert_source_length[SHADERS_PROGRAM_OV_B],
                                frag_source_string[SHADERS_PROGRAM_OV_B],
                                frag_source_length[SHADERS_PROGRAM_OV_B]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (sky parallax).\n");
    glDeleteProgram(G_program_id_OV_A);
    glDeleteProgram(G_program_id_OV_B);
    return 1;
  }

  /* draw sprites program */
  G_program_id_OV_C = glCreateProgram();

  if (shaders_compile_program( G_program_id_OV_C, 
                                vert_source_string[SHADERS_PROGRAM_OV_C],
                                vert_source_length[SHADERS_PROGRAM_OV_C],
                                frag_source_string[SHADERS_PROGRAM_OV_C],
                                frag_source_length[SHADERS_PROGRAM_OV_C]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (draw sprites).\n");
    glDeleteProgram(G_program_id_OV_A);
    glDeleteProgram(G_program_id_OV_B);
    glDeleteProgram(G_program_id_OV_C);
    return 1;
  }

  /* fade program */
  G_program_id_OV_D = glCreateProgram();

  if (shaders_compile_program( G_program_id_OV_D, 
                                vert_source_string[SHADERS_PROGRAM_OV_D],
                                vert_source_length[SHADERS_PROGRAM_OV_D],
                                frag_source_string[SHADERS_PROGRAM_OV_D],
                                frag_source_length[SHADERS_PROGRAM_OV_D]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (fade).\n");
    glDeleteProgram(G_program_id_OV_A);
    glDeleteProgram(G_program_id_OV_B);
    glDeleteProgram(G_program_id_OV_C);
    glDeleteProgram(G_program_id_OV_D);
    return 1;
  }

  /* convert to rgb program */
  G_program_id_OV_E = glCreateProgram();

  if (shaders_compile_program( G_program_id_OV_E, 
                                vert_source_string[SHADERS_PROGRAM_OV_E],
                                vert_source_length[SHADERS_PROGRAM_OV_E],
                                frag_source_string[SHADERS_PROGRAM_OV_E],
                                frag_source_length[SHADERS_PROGRAM_OV_E]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (convert to rgb).\n");
    glDeleteProgram(G_program_id_OV_A);
    glDeleteProgram(G_program_id_OV_B);
    glDeleteProgram(G_program_id_OV_C);
    glDeleteProgram(G_program_id_OV_D);
    glDeleteProgram(G_program_id_OV_E);
    return 1;
  }

  /* apply brightness program */
  G_program_id_UP_A = glCreateProgram();

  if (shaders_compile_program( G_program_id_UP_A, 
                                vert_source_string[SHADERS_PROGRAM_UP_A],
                                vert_source_length[SHADERS_PROGRAM_UP_A],
                                frag_source_string[SHADERS_PROGRAM_UP_A],
                                frag_source_length[SHADERS_PROGRAM_UP_A]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (apply brightness).\n");
    glDeleteProgram(G_program_id_OV_A);
    glDeleteProgram(G_program_id_OV_B);
    glDeleteProgram(G_program_id_OV_C);
    glDeleteProgram(G_program_id_OV_D);
    glDeleteProgram(G_program_id_OV_E);
    glDeleteProgram(G_program_id_UP_A);
    return 1;
  }

  /* nearest upscale program */
  G_program_id_UP_B = glCreateProgram();

  if (shaders_compile_program( G_program_id_UP_B, 
                                vert_source_string[SHADERS_PROGRAM_UP_B],
                                vert_source_length[SHADERS_PROGRAM_UP_B],
                                frag_source_string[SHADERS_PROGRAM_UP_B],
                                frag_source_length[SHADERS_PROGRAM_UP_B]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (nearest upscale).\n");
    glDeleteProgram(G_program_id_OV_A);
    glDeleteProgram(G_program_id_OV_B);
    glDeleteProgram(G_program_id_OV_C);
    glDeleteProgram(G_program_id_OV_D);
    glDeleteProgram(G_program_id_OV_E);
    glDeleteProgram(G_program_id_UP_A);
    glDeleteProgram(G_program_id_UP_B);
    return 1;
  }

  /* linear upscale program */
  G_program_id_UP_C = glCreateProgram();

  if (shaders_compile_program( G_program_id_UP_C, 
                                vert_source_string[SHADERS_PROGRAM_UP_C],
                                vert_source_length[SHADERS_PROGRAM_UP_C],
                                frag_source_string[SHADERS_PROGRAM_UP_C],
                                frag_source_length[SHADERS_PROGRAM_UP_C]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (linear upscale).\n");
    glDeleteProgram(G_program_id_OV_A);
    glDeleteProgram(G_program_id_OV_B);
    glDeleteProgram(G_program_id_OV_C);
    glDeleteProgram(G_program_id_OV_D);
    glDeleteProgram(G_program_id_OV_E);
    glDeleteProgram(G_program_id_UP_A);
    glDeleteProgram(G_program_id_UP_B);
    glDeleteProgram(G_program_id_UP_C);
    return 1;
  }

  /* cubic horizontal resize program */
  G_program_id_UP_D = glCreateProgram();

  if (shaders_compile_program( G_program_id_UP_D, 
                                vert_source_string[SHADERS_PROGRAM_UP_D],
                                vert_source_length[SHADERS_PROGRAM_UP_D],
                                frag_source_string[SHADERS_PROGRAM_UP_D],
                                frag_source_length[SHADERS_PROGRAM_UP_D]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (horizontal cubic).\n");
    glDeleteProgram(G_program_id_OV_A);
    glDeleteProgram(G_program_id_OV_B);
    glDeleteProgram(G_program_id_OV_C);
    glDeleteProgram(G_program_id_OV_D);
    glDeleteProgram(G_program_id_OV_E);
    glDeleteProgram(G_program_id_UP_A);
    glDeleteProgram(G_program_id_UP_B);
    glDeleteProgram(G_program_id_UP_C);
    glDeleteProgram(G_program_id_UP_D);
    return 1;
  }

  /* scanline vertical resize program */
  G_program_id_UP_E = glCreateProgram();

  if (shaders_compile_program( G_program_id_UP_E, 
                                vert_source_string[SHADERS_PROGRAM_UP_E],
                                vert_source_length[SHADERS_PROGRAM_UP_E],
                                frag_source_string[SHADERS_PROGRAM_UP_E],
                                frag_source_length[SHADERS_PROGRAM_UP_E]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (vertical scanlines).\n");
    glDeleteProgram(G_program_id_OV_A);
    glDeleteProgram(G_program_id_OV_B);
    glDeleteProgram(G_program_id_OV_C);
    glDeleteProgram(G_program_id_OV_D);
    glDeleteProgram(G_program_id_OV_E);
    glDeleteProgram(G_program_id_UP_A);
    glDeleteProgram(G_program_id_UP_B);
    glDeleteProgram(G_program_id_UP_C);
    glDeleteProgram(G_program_id_UP_D);
    glDeleteProgram(G_program_id_UP_E);
    return 1;
  }

  /* free shader source strings */
  for (i = 0; i < SHADERS_NUM_PROGRAMS; i++)
  {
    if (vert_source_string[i] != NULL)
    {
      free(vert_source_string[i]);
      vert_source_string[i] = NULL;
    }

    if (frag_source_string[i] != NULL)
    {
      free(frag_source_string[i]);
      frag_source_string[i] = NULL;
    }
  }

  return 0;
}

/*******************************************************************************
** shaders_create_programs()
*******************************************************************************/
short int shaders_create_programs()
{
  /* compile glsl shaders */
  if (shaders_load_shaders(G_path_shader_data))
  {
    fprintf(stdout, "Failed to compile GLSL shaders.\n");
    return 1;
  }

  /* obtain ids for uniform variables in the shaders */
  G_uniform_OV_A_mvp_matrix_id = glGetUniformLocation(G_program_id_OV_A, "mvp_matrix");
  G_uniform_OV_A_texture_sampler_id = glGetUniformLocation(G_program_id_OV_A, "texture_sampler");

  G_uniform_OV_B_mvp_matrix_id = glGetUniformLocation(G_program_id_OV_B, "mvp_matrix");
  G_uniform_OV_B_texture_sampler_id = glGetUniformLocation(G_program_id_OV_B, "texture_sampler");
  G_uniform_OV_B_parallax_sampler_id = glGetUniformLocation(G_program_id_OV_B, "parallax_sampler");
  G_uniform_OV_B_hori_shift_id = glGetUniformLocation(G_program_id_OV_B, "hori_shift");
  G_uniform_OV_B_vert_shift_id = glGetUniformLocation(G_program_id_OV_B, "vert_shift");

  G_uniform_OV_C_mvp_matrix_id = glGetUniformLocation(G_program_id_OV_C, "mvp_matrix");
  G_uniform_OV_C_texture_sampler_id = glGetUniformLocation(G_program_id_OV_C, "texture_sampler");

  G_uniform_OV_D_mvp_matrix_id = glGetUniformLocation(G_program_id_OV_D, "mvp_matrix");
  G_uniform_OV_D_texture_sampler_id = glGetUniformLocation(G_program_id_OV_D, "texture_sampler");
  G_uniform_OV_D_amount_id = glGetUniformLocation(G_program_id_OV_D, "amount");

  G_uniform_OV_E_mvp_matrix_id = glGetUniformLocation(G_program_id_OV_E, "mvp_matrix");
  G_uniform_OV_E_texture_sampler_id = glGetUniformLocation(G_program_id_OV_E, "texture_sampler");
  G_uniform_OV_E_palette_sampler_id = glGetUniformLocation(G_program_id_OV_E, "palette_sampler");
  G_uniform_OV_E_levels_id = glGetUniformLocation(G_program_id_OV_E, "levels");

  G_uniform_UP_A_mvp_matrix_id = glGetUniformLocation(G_program_id_UP_A, "mvp_matrix");
  G_uniform_UP_A_texture_sampler_id = glGetUniformLocation(G_program_id_UP_A, "texture_sampler");
  G_uniform_UP_A_rgb2yiq_matrix_id = glGetUniformLocation(G_program_id_UP_A, "rgb2yiq_matrix");
  G_uniform_UP_A_yiq2rgb_matrix_id = glGetUniformLocation(G_program_id_UP_A, "yiq2rgb_matrix");
  G_uniform_UP_A_black_level_id = glGetUniformLocation(G_program_id_UP_A, "black_level");
  G_uniform_UP_A_white_level_id = glGetUniformLocation(G_program_id_UP_A, "white_level");

  G_uniform_UP_B_mvp_matrix_id = glGetUniformLocation(G_program_id_UP_B, "mvp_matrix");
  G_uniform_UP_B_texture_sampler_id = glGetUniformLocation(G_program_id_UP_B, "texture_sampler");

  G_uniform_UP_C_mvp_matrix_id = glGetUniformLocation(G_program_id_UP_C, "mvp_matrix");
  G_uniform_UP_C_texture_sampler_id = glGetUniformLocation(G_program_id_UP_C, "texture_sampler");

  G_uniform_UP_D_mvp_matrix_id = glGetUniformLocation(G_program_id_UP_D, "mvp_matrix");
  G_uniform_UP_D_texture_sampler_id = glGetUniformLocation(G_program_id_UP_D, "texture_sampler");
  G_uniform_UP_D_cubic_matrix_id = glGetUniformLocation(G_program_id_UP_D, "cubic_matrix");

  G_uniform_UP_E_mvp_matrix_id = glGetUniformLocation(G_program_id_UP_E, "mvp_matrix");
  G_uniform_UP_E_texture_sampler_id = glGetUniformLocation(G_program_id_UP_E, "texture_sampler");

  return 0;
}

/*******************************************************************************
** shaders_destroy_programs()
*******************************************************************************/
short int shaders_destroy_programs()
{
  /* delete opengl shader programs */
  glDeleteProgram(G_program_id_OV_A);
  glDeleteProgram(G_program_id_OV_B);
  glDeleteProgram(G_program_id_OV_C);
  glDeleteProgram(G_program_id_OV_D);
  glDeleteProgram(G_program_id_OV_E);
  glDeleteProgram(G_program_id_UP_A);
  glDeleteProgram(G_program_id_UP_B);
  glDeleteProgram(G_program_id_UP_C);
  glDeleteProgram(G_program_id_UP_D);
  glDeleteProgram(G_program_id_UP_E);

  return 0;
}

