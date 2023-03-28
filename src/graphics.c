/*******************************************************************************
** graphics.c (opengl objects)
*******************************************************************************/

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "doremi.h"
#include "global.h"
#include "graphics.h"
#include "path.h"

/* sdl window, various sizes, etc */
SDL_Window* G_sdl_window;

int G_graphics_resolution;

int G_viewport_w;
int G_viewport_h;

static int S_window_w;
static int S_window_h;

static int S_desktop_w;
static int S_desktop_h;

static int S_nearest_w;
static int S_nearest_h;

/* texture coordinate tables */
GLfloat G_texture_coord_table[65];
GLfloat G_palette_coord_table[64];

/* vbo size variables */
int G_num_tiles;
int G_num_sprites;
int G_num_saved_sprites[GRAPHICS_SAVED_SPRITE_LAYERS];

/* opengl vbo ids */
GLuint G_vertex_array_id;

GLuint G_vertex_buffer_id_tiles;
GLuint G_texture_coord_buffer_id_tiles;
GLuint G_palette_coord_buffer_id_tiles;
GLuint G_index_buffer_id_tiles;

GLuint G_vertex_buffer_id_sprites;
GLuint G_texture_coord_buffer_id_sprites;
GLuint G_palette_coord_buffer_id_sprites;
GLuint G_index_buffer_id_sprites;

/* postprocessing vbo ids */
GLuint G_vertex_buffer_id_postprocessing_overscan;
GLuint G_vertex_buffer_id_postprocessing_nearest_resize;
GLuint G_vertex_buffer_id_postprocessing_cubic_resize;
GLuint G_vertex_buffer_id_postprocessing_window;

GLuint G_texture_coord_buffer_id_postprocessing_overscan;
GLuint G_texture_coord_buffer_id_postprocessing_nearest_resize;
GLuint G_texture_coord_buffer_id_postprocessing_cubic_resize;

GLuint G_index_buffer_id_postprocessing_all;

/* opengl intermediate textures */
GLuint G_texture_id_intermediate_1;
GLuint G_texture_id_intermediate_2;

/* opengl framebuffer ids */
GLuint G_framebuffer_id_intermediate_1;
GLuint G_renderbuffer_id_intermediate_1;

GLuint G_framebuffer_id_intermediate_2;
GLuint G_renderbuffer_id_intermediate_2;

/* shader program and uniform ids */
GLuint G_program_id_A;
GLuint G_program_id_B;
GLuint G_program_id_C;
GLuint G_program_id_D;
GLuint G_program_id_E;
GLuint G_program_id_OV1;
GLuint G_program_id_OV2;

GLuint G_uniform_A_mvp_matrix_id;
GLuint G_uniform_A_texture_sampler_id;
GLuint G_uniform_A_rgb2yiq_matrix_id;
GLuint G_uniform_A_yiq2rgb_matrix_id;
GLuint G_uniform_A_black_level_id;
GLuint G_uniform_A_white_level_id;

GLuint G_uniform_B_mvp_matrix_id;
GLuint G_uniform_B_texture_sampler_id;

GLuint G_uniform_C_mvp_matrix_id;
GLuint G_uniform_C_texture_sampler_id;

GLuint G_uniform_D_mvp_matrix_id;
GLuint G_uniform_D_texture_sampler_id;
GLuint G_uniform_D_cubic_matrix_id;

GLuint G_uniform_E_mvp_matrix_id;
GLuint G_uniform_E_texture_sampler_id;

GLuint G_uniform_OV1_mvp_matrix_id;
GLuint G_uniform_OV1_texture_sampler_id;
GLuint G_uniform_OV1_palette_sampler_id;

GLuint G_uniform_OV2_mvp_matrix_id;
GLuint G_uniform_OV2_texture_sampler_id;
GLuint G_uniform_OV2_palette_sampler_id;
GLuint G_uniform_OV2_fade_amount_id;
GLuint G_uniform_OV2_subpalette_size_id;

/* vbos and matrices */
GLfloat*          G_vertex_buffer_tiles;
GLfloat*          G_texture_coord_buffer_tiles;
GLfloat*          G_palette_coord_buffer_tiles;
unsigned short*   G_index_buffer_tiles;

GLfloat*          G_vertex_buffer_sprites;
GLfloat*          G_texture_coord_buffer_sprites;
GLfloat*          G_palette_coord_buffer_sprites;
unsigned short*   G_index_buffer_sprites;

GLfloat           G_vertex_buffer_postprocessing_overscan[12];
GLfloat           G_vertex_buffer_postprocessing_nearest_resize[12];
GLfloat           G_vertex_buffer_postprocessing_cubic_resize[12];
GLfloat           G_vertex_buffer_postprocessing_window[12];

GLfloat           G_texture_coord_buffer_postprocessing_overscan[8];
GLfloat           G_texture_coord_buffer_postprocessing_nearest_resize[8];
GLfloat           G_texture_coord_buffer_postprocessing_cubic_resize[8];

unsigned short    G_index_buffer_postprocessing_all[6];

GLfloat           G_mvp_matrix_overscan[16];
GLfloat           G_mvp_matrix_intermediate[16];
GLfloat           G_mvp_matrix_window[16];

/*******************************************************************************
** graphics_generate_tables()
*******************************************************************************/
short int graphics_generate_tables()
{
  int i;

  /* texture coordinate table */
  /* finding coordinates at the borders of a 8x8 cell on the texture  */
  /* on a 512x512 texture, the pixel size is 1/512, so the borders    */
  /* are at multiples of 8/512 = 1/64 = 0.015625                      */
  for (i = 0; i < 64; i++)
    G_texture_coord_table[i] = i * 0.015625f;

  G_texture_coord_table[64] = 1.0f;

  /* palette coordinate table */
  /* finding coordinates at the center of a pixel on the texture  */
  /* on a 64x64 texture, the pixel size is 1/64, so the centers   */
  /* are at odd multiples of 1/128 = 0.0078125                    */
  for (i = 0; i < 64; i++)
    G_palette_coord_table[i] = (2 * i + 1) * 0.0078125f;

  return 0;
}

/*******************************************************************************
** graphics_compile_program()
*******************************************************************************/
short int graphics_compile_program( GLuint  program_id, 
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
** graphics_load_shaders()
*******************************************************************************/
short int graphics_load_shaders(char* filename)
{
  FILE*         fp;
  int           i;
  char          signature[8];

  GLchar*       vert_source_string[GRAPHICS_NUM_SHADER_PROGRAMS];
  GLint         vert_source_length[GRAPHICS_NUM_SHADER_PROGRAMS];

  GLchar*       frag_source_string[GRAPHICS_NUM_SHADER_PROGRAMS];
  GLint         frag_source_length[GRAPHICS_NUM_SHADER_PROGRAMS];

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
      (signature[6] != '1') || 
      (signature[7] != 'B'))
  {
    fclose(fp);
    return 1;
  }

  /* initialize source variables */
  for (i = 0; i < GRAPHICS_NUM_SHADER_PROGRAMS; i++)
  {
    vert_source_string[i] = NULL;
    vert_source_length[i] = 0;

    frag_source_string[i] = NULL;
    frag_source_length[i] = 0;
  }

  /* read shader sources */
  for (i = 0; i < GRAPHICS_NUM_SHADER_PROGRAMS; i++)
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

  /* apply brightness program */
  G_program_id_A = glCreateProgram();

  if (graphics_compile_program( G_program_id_A, 
                                vert_source_string[GRAPHICS_SHADER_PROGRAM_A],
                                vert_source_length[GRAPHICS_SHADER_PROGRAM_A],
                                frag_source_string[GRAPHICS_SHADER_PROGRAM_A],
                                frag_source_length[GRAPHICS_SHADER_PROGRAM_A]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (apply brightness).\n");
    glDeleteProgram(G_program_id_A);
    return 1;
  }

  /* nearest upscale program */
  G_program_id_B = glCreateProgram();

  if (graphics_compile_program( G_program_id_B, 
                                vert_source_string[GRAPHICS_SHADER_PROGRAM_B],
                                vert_source_length[GRAPHICS_SHADER_PROGRAM_B],
                                frag_source_string[GRAPHICS_SHADER_PROGRAM_B],
                                frag_source_length[GRAPHICS_SHADER_PROGRAM_B]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (nearest upscale).\n");
    glDeleteProgram(G_program_id_A);
    glDeleteProgram(G_program_id_B);
    return 1;
  }

  /* linear upscale program */
  G_program_id_C = glCreateProgram();

  if (graphics_compile_program( G_program_id_C, 
                                vert_source_string[GRAPHICS_SHADER_PROGRAM_C],
                                vert_source_length[GRAPHICS_SHADER_PROGRAM_C],
                                frag_source_string[GRAPHICS_SHADER_PROGRAM_C],
                                frag_source_length[GRAPHICS_SHADER_PROGRAM_C]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (linear upscale).\n");
    glDeleteProgram(G_program_id_A);
    glDeleteProgram(G_program_id_B);
    glDeleteProgram(G_program_id_C);
    return 1;
  }

  /* cubic horizontal resize program */
  G_program_id_D = glCreateProgram();

  if (graphics_compile_program( G_program_id_D, 
                                vert_source_string[GRAPHICS_SHADER_PROGRAM_D],
                                vert_source_length[GRAPHICS_SHADER_PROGRAM_D],
                                frag_source_string[GRAPHICS_SHADER_PROGRAM_D],
                                frag_source_length[GRAPHICS_SHADER_PROGRAM_D]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (horizontal cubic).\n");
    glDeleteProgram(G_program_id_A);
    glDeleteProgram(G_program_id_B);
    glDeleteProgram(G_program_id_C);
    glDeleteProgram(G_program_id_D);
    return 1;
  }

  /* scanline vertical resize program */
  G_program_id_E = glCreateProgram();

  if (graphics_compile_program( G_program_id_E, 
                                vert_source_string[GRAPHICS_SHADER_PROGRAM_E],
                                vert_source_length[GRAPHICS_SHADER_PROGRAM_E],
                                frag_source_string[GRAPHICS_SHADER_PROGRAM_E],
                                frag_source_length[GRAPHICS_SHADER_PROGRAM_E]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (vertical scanlines).\n");
    glDeleteProgram(G_program_id_A);
    glDeleteProgram(G_program_id_B);
    glDeleteProgram(G_program_id_C);
    glDeleteProgram(G_program_id_D);
    glDeleteProgram(G_program_id_E);
    return 1;
  }

  /* overscan (standard) program */
  G_program_id_OV1 = glCreateProgram();

  if (graphics_compile_program( G_program_id_OV1, 
                                vert_source_string[GRAPHICS_SHADER_PROGRAM_OV1],
                                vert_source_length[GRAPHICS_SHADER_PROGRAM_OV1],
                                frag_source_string[GRAPHICS_SHADER_PROGRAM_OV1],
                                frag_source_length[GRAPHICS_SHADER_PROGRAM_OV1]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (overscan standard).\n");
    glDeleteProgram(G_program_id_A);
    glDeleteProgram(G_program_id_B);
    glDeleteProgram(G_program_id_C);
    glDeleteProgram(G_program_id_D);
    glDeleteProgram(G_program_id_E);
    glDeleteProgram(G_program_id_OV1);
    return 1;
  }

  /* overscan (fade) program */
  G_program_id_OV2 = glCreateProgram();

  if (graphics_compile_program( G_program_id_OV2, 
                                vert_source_string[GRAPHICS_SHADER_PROGRAM_OV2],
                                vert_source_length[GRAPHICS_SHADER_PROGRAM_OV2],
                                frag_source_string[GRAPHICS_SHADER_PROGRAM_OV2],
                                frag_source_length[GRAPHICS_SHADER_PROGRAM_OV2]))
  {
    fprintf(stdout, "Failed to compile OpenGL program (overscan fade).\n");
    glDeleteProgram(G_program_id_A);
    glDeleteProgram(G_program_id_B);
    glDeleteProgram(G_program_id_C);
    glDeleteProgram(G_program_id_D);
    glDeleteProgram(G_program_id_E);
    glDeleteProgram(G_program_id_OV1);
    glDeleteProgram(G_program_id_OV2);
    return 1;
  }

  /* free shader source strings */
  for (i = 0; i < GRAPHICS_NUM_SHADER_PROGRAMS; i++)
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
** graphics_create_opengl_objects()
*******************************************************************************/
short int graphics_create_opengl_objects()
{
  int i;
  int j;

  /* create vertex array object */
  glGenVertexArrays(1, &G_vertex_array_id);
  glBindVertexArray(G_vertex_array_id);

  /* compile glsl shaders */
  if (graphics_load_shaders(G_path_shader_data))
  {
    fprintf(stdout, "Failed to compile GLSL shaders.\n");
    glDeleteVertexArrays(1, &G_vertex_array_id);
    return 1;
  }

  /* obtain ids for uniform variables in the shaders */
  G_uniform_A_mvp_matrix_id = glGetUniformLocation(G_program_id_A, "mvp_matrix");
  G_uniform_A_texture_sampler_id = glGetUniformLocation(G_program_id_A, "texture_sampler");
  G_uniform_A_rgb2yiq_matrix_id = glGetUniformLocation(G_program_id_A, "rgb2yiq_matrix");
  G_uniform_A_yiq2rgb_matrix_id = glGetUniformLocation(G_program_id_A, "yiq2rgb_matrix");
  G_uniform_A_black_level_id = glGetUniformLocation(G_program_id_A, "black_level");
  G_uniform_A_white_level_id = glGetUniformLocation(G_program_id_A, "white_level");

  G_uniform_B_mvp_matrix_id = glGetUniformLocation(G_program_id_B, "mvp_matrix");
  G_uniform_B_texture_sampler_id = glGetUniformLocation(G_program_id_B, "texture_sampler");

  G_uniform_C_mvp_matrix_id = glGetUniformLocation(G_program_id_C, "mvp_matrix");
  G_uniform_C_texture_sampler_id = glGetUniformLocation(G_program_id_C, "texture_sampler");

  G_uniform_D_mvp_matrix_id = glGetUniformLocation(G_program_id_D, "mvp_matrix");
  G_uniform_D_texture_sampler_id = glGetUniformLocation(G_program_id_D, "texture_sampler");
  G_uniform_D_cubic_matrix_id = glGetUniformLocation(G_program_id_D, "cubic_matrix");

  G_uniform_E_mvp_matrix_id = glGetUniformLocation(G_program_id_E, "mvp_matrix");
  G_uniform_E_texture_sampler_id = glGetUniformLocation(G_program_id_E, "texture_sampler");

  G_uniform_OV1_mvp_matrix_id = glGetUniformLocation(G_program_id_OV1, "mvp_matrix");
  G_uniform_OV1_texture_sampler_id = glGetUniformLocation(G_program_id_OV1, "texture_sampler");
  G_uniform_OV1_palette_sampler_id = glGetUniformLocation(G_program_id_OV1, "palette_sampler");

  G_uniform_OV2_mvp_matrix_id = glGetUniformLocation(G_program_id_OV2, "mvp_matrix");
  G_uniform_OV2_texture_sampler_id = glGetUniformLocation(G_program_id_OV2, "texture_sampler");
  G_uniform_OV2_palette_sampler_id = glGetUniformLocation(G_program_id_OV2, "palette_sampler");
  G_uniform_OV2_fade_amount_id = glGetUniformLocation(G_program_id_OV2, "fade_amount");
  G_uniform_OV2_subpalette_size_id = glGetUniformLocation(G_program_id_OV2, "subpalette_size");

  /* create intermediate texture 1 */
  glGenTextures(1, &G_texture_id_intermediate_1);
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_1);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, 
                GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

  /* create framebuffer object 1 */
  glGenFramebuffers(1, &G_framebuffer_id_intermediate_1);
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_1);

  glGenRenderbuffers(1, &G_renderbuffer_id_intermediate_1);
  glBindRenderbuffer(GL_RENDERBUFFER, G_renderbuffer_id_intermediate_1);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
                        GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                            GL_RENDERBUFFER, G_renderbuffer_id_intermediate_1);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, G_texture_id_intermediate_1, 0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    fprintf(stdout, "Error: Framebuffer Object 1 is not complete!\n");
    return 1;
  }

  /* create intermediate texture 2 */
  glGenTextures(1, &G_texture_id_intermediate_2);
  glBindTexture(GL_TEXTURE_2D, G_texture_id_intermediate_2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, 
                GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

  /* create framebuffer object 2 */
  glGenFramebuffers(1, &G_framebuffer_id_intermediate_2);
  glBindFramebuffer(GL_FRAMEBUFFER, G_framebuffer_id_intermediate_2);

  glGenRenderbuffers(1, &G_renderbuffer_id_intermediate_2);
  glBindRenderbuffer(GL_RENDERBUFFER, G_renderbuffer_id_intermediate_2);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
                        GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH, GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                            GL_RENDERBUFFER, G_renderbuffer_id_intermediate_2);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, G_texture_id_intermediate_2, 0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    fprintf(stdout, "Error: Framebuffer Object 1 is not complete!\n");
    return 1;
  }

  /* orthographic projection matrix (overscan) */
  G_mvp_matrix_overscan[0]  = 2.0f / GRAPHICS_OVERSCAN_OUTPUT_WIDTH;
  G_mvp_matrix_overscan[1]  = 0.0f;
  G_mvp_matrix_overscan[2]  = 0.0f;
  G_mvp_matrix_overscan[3]  = 0.0f;

  G_mvp_matrix_overscan[4]  = 0.0f;
  G_mvp_matrix_overscan[5]  = -2.0f / GRAPHICS_OVERSCAN_OUTPUT_HEIGHT;
  G_mvp_matrix_overscan[6]  = 0.0f;
  G_mvp_matrix_overscan[7]  = 0.0f;

  G_mvp_matrix_overscan[8]  = 0.0f;
  G_mvp_matrix_overscan[9]  = 0.0f;
  G_mvp_matrix_overscan[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_overscan[11] = 0.0f;

  G_mvp_matrix_overscan[12] = -1.0f;
  G_mvp_matrix_overscan[13] = 1.0f;
  G_mvp_matrix_overscan[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_overscan[15] = 1.0f;

  /* orthographic projection matrix (intermediate) */
  G_mvp_matrix_intermediate[0]  = 2.0f / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_mvp_matrix_intermediate[1]  = 0.0f;
  G_mvp_matrix_intermediate[2]  = 0.0f;
  G_mvp_matrix_intermediate[3]  = 0.0f;

  G_mvp_matrix_intermediate[4]  = 0.0f;
  G_mvp_matrix_intermediate[5]  = -2.0f / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;
  G_mvp_matrix_intermediate[6]  = 0.0f;
  G_mvp_matrix_intermediate[7]  = 0.0f;

  G_mvp_matrix_intermediate[8]  = 0.0f;
  G_mvp_matrix_intermediate[9]  = 0.0f;
  G_mvp_matrix_intermediate[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_intermediate[11] = 0.0f;

  G_mvp_matrix_intermediate[12] = -1.0f;
  G_mvp_matrix_intermediate[13] = 1.0f;
  G_mvp_matrix_intermediate[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_intermediate[15] = 1.0f;

  /* orthographic projection matrix (window) */
  G_mvp_matrix_window[0]  = 2.0f / S_window_w;
  G_mvp_matrix_window[1]  = 0.0f;
  G_mvp_matrix_window[2]  = 0.0f;
  G_mvp_matrix_window[3]  = 0.0f;

  G_mvp_matrix_window[4]  = 0.0f;
  G_mvp_matrix_window[5]  = -2.0f / S_window_h;
  G_mvp_matrix_window[6]  = 0.0f;
  G_mvp_matrix_window[7]  = 0.0f;

  G_mvp_matrix_window[8]  = 0.0f;
  G_mvp_matrix_window[9]  = 0.0f;
  G_mvp_matrix_window[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_window[11] = 0.0f;

  G_mvp_matrix_window[12] = -1.0f;
  G_mvp_matrix_window[13] = 1.0f;
  G_mvp_matrix_window[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_window[15] = 1.0f;

  /* create buffer objects */
  glGenBuffers(1, &G_vertex_buffer_id_tiles);
  glGenBuffers(1, &G_texture_coord_buffer_id_tiles);
  glGenBuffers(1, &G_palette_coord_buffer_id_tiles);
  glGenBuffers(1, &G_index_buffer_id_tiles);

  glGenBuffers(1, &G_vertex_buffer_id_sprites);
  glGenBuffers(1, &G_texture_coord_buffer_id_sprites);
  glGenBuffers(1, &G_palette_coord_buffer_id_sprites);
  glGenBuffers(1, &G_index_buffer_id_sprites);

  glGenBuffers(1, &G_vertex_buffer_id_postprocessing_overscan);
  glGenBuffers(1, &G_vertex_buffer_id_postprocessing_nearest_resize);
  glGenBuffers(1, &G_vertex_buffer_id_postprocessing_cubic_resize);
  glGenBuffers(1, &G_vertex_buffer_id_postprocessing_window);

  glGenBuffers(1, &G_texture_coord_buffer_id_postprocessing_overscan);
  glGenBuffers(1, &G_texture_coord_buffer_id_postprocessing_nearest_resize);
  glGenBuffers(1, &G_texture_coord_buffer_id_postprocessing_cubic_resize);

  glGenBuffers(1, &G_index_buffer_id_postprocessing_all);

  /* allocate buffers */
  G_vertex_buffer_tiles = malloc(sizeof(GLfloat) * 3 * 4 * GRAPHICS_MAX_TILES);
  G_texture_coord_buffer_tiles = malloc(sizeof(GLfloat) * 2 * 4 * GRAPHICS_MAX_TILES);
  G_palette_coord_buffer_tiles = malloc(sizeof(GLfloat) * 1 * 4 * GRAPHICS_MAX_TILES);
  G_index_buffer_tiles = malloc(sizeof(unsigned short) * 6 * GRAPHICS_MAX_TILES);

  G_vertex_buffer_sprites = malloc(sizeof(GLfloat) * 3 * 4 * GRAPHICS_MAX_SPRITES);
  G_texture_coord_buffer_sprites = malloc(sizeof(GLfloat) * 2 * 4 * GRAPHICS_MAX_SPRITES);
  G_palette_coord_buffer_sprites = malloc(sizeof(GLfloat) * 1 * 4 * GRAPHICS_MAX_SPRITES);
  G_index_buffer_sprites = malloc(sizeof(unsigned short) * 6 * GRAPHICS_MAX_SPRITES);

  /* initialize tile buffers */
  for (i = 0; i < GRAPHICS_MAX_TILES; i++)
  {
    for (j = 0; j < 12; j++)
      G_vertex_buffer_tiles[(i * 12) + j] = 0.0f;

    for (j = 0; j < 8; j++)
      G_texture_coord_buffer_tiles[(i * 8) + j] = 0.0f;

    for (j = 0; j < 4; j++)
      G_palette_coord_buffer_tiles[(i * 4) + j] = 0.0f;

    for (j = 0; j < 6; j++)
      G_index_buffer_tiles[(i * 6) + j] = 0;
  }

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_tiles);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_TILES * 12 * sizeof(GLfloat),
                G_vertex_buffer_tiles, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_tiles);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_TILES * 8 * sizeof(GLfloat),
                G_texture_coord_buffer_tiles, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, G_palette_coord_buffer_id_tiles);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_TILES * 4 * sizeof(GLfloat),
                G_palette_coord_buffer_tiles, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_tiles);
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, GRAPHICS_MAX_TILES * 6 * sizeof(unsigned short),
                G_index_buffer_tiles, GL_STATIC_DRAW);

  /* initialize sprite buffers */
  for (i = 0; i < GRAPHICS_MAX_SPRITES; i++)
  {
    for (j = 0; j < 12; j++)
      G_vertex_buffer_sprites[(i * 12) + j] = 0.0f;

    for (j = 0; j < 8; j++)
      G_texture_coord_buffer_sprites[(i * 8) + j] = 0.0f;

    for (j = 0; j < 4; j++)
      G_palette_coord_buffer_sprites[(i * 4) + j] = 0.0f;

    for (j = 0; j < 6; j++)
      G_index_buffer_sprites[(i * 6) + j] = 0;
  }

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_sprites);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_SPRITES * 12 * sizeof(GLfloat),
                G_vertex_buffer_sprites, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_sprites);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_SPRITES * 8 * sizeof(GLfloat),
                G_texture_coord_buffer_sprites, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, G_palette_coord_buffer_id_sprites);
  glBufferData( GL_ARRAY_BUFFER, GRAPHICS_MAX_SPRITES * 4 * sizeof(GLfloat),
                G_palette_coord_buffer_sprites, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_sprites);
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, GRAPHICS_MAX_SPRITES * 6 * sizeof(unsigned short),
                G_index_buffer_sprites, GL_DYNAMIC_DRAW);

  /* set up postprocessing overscan vertex & texture coordinate buffers */
  G_vertex_buffer_postprocessing_overscan[0] = 0.0f;
  G_vertex_buffer_postprocessing_overscan[1] = 0.0f;
  G_vertex_buffer_postprocessing_overscan[2] = 0.5f;

  G_vertex_buffer_postprocessing_overscan[3] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_WIDTH;
  G_vertex_buffer_postprocessing_overscan[4] = 0.0f;
  G_vertex_buffer_postprocessing_overscan[5] = 0.5f;

  G_vertex_buffer_postprocessing_overscan[6] = 0.0f;
  G_vertex_buffer_postprocessing_overscan[7] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT;
  G_vertex_buffer_postprocessing_overscan[8] = 0.5f;

  G_vertex_buffer_postprocessing_overscan[9] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_WIDTH;
  G_vertex_buffer_postprocessing_overscan[10] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT;
  G_vertex_buffer_postprocessing_overscan[11] = 0.5f;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_overscan);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_overscan, GL_STATIC_DRAW);

  G_texture_coord_buffer_postprocessing_overscan[0] = 0.0f;
  G_texture_coord_buffer_postprocessing_overscan[1] = 1.0f;

  G_texture_coord_buffer_postprocessing_overscan[2] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_WIDTH / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_overscan[3] = 1.0f;

  G_texture_coord_buffer_postprocessing_overscan[4] = 0.0f;
  G_texture_coord_buffer_postprocessing_overscan[5] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  G_texture_coord_buffer_postprocessing_overscan[6] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_WIDTH / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_overscan[7] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_overscan);
  glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), 
                G_texture_coord_buffer_postprocessing_overscan, GL_STATIC_DRAW);

  /* set up postprocessing nearest resize vertex & texture coordinate buffers */
  G_vertex_buffer_postprocessing_nearest_resize[0] = 0.0f;
  G_vertex_buffer_postprocessing_nearest_resize[1] = 0.0f;
  G_vertex_buffer_postprocessing_nearest_resize[2] = 0.5f;

  G_vertex_buffer_postprocessing_nearest_resize[3] = (GLfloat) S_nearest_w;
  G_vertex_buffer_postprocessing_nearest_resize[4] = 0.0f;
  G_vertex_buffer_postprocessing_nearest_resize[5] = 0.5f;

  G_vertex_buffer_postprocessing_nearest_resize[6] = 0.0f;
  G_vertex_buffer_postprocessing_nearest_resize[7] = (GLfloat) S_nearest_h;
  G_vertex_buffer_postprocessing_nearest_resize[8] = 0.5f;

  G_vertex_buffer_postprocessing_nearest_resize[9] = (GLfloat) S_nearest_w;
  G_vertex_buffer_postprocessing_nearest_resize[10] = (GLfloat) S_nearest_h;
  G_vertex_buffer_postprocessing_nearest_resize[11] = 0.5f;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_nearest_resize);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_nearest_resize, GL_STATIC_DRAW);

  G_texture_coord_buffer_postprocessing_nearest_resize[0] = 0.0f;
  G_texture_coord_buffer_postprocessing_nearest_resize[1] = 1.0f;

  G_texture_coord_buffer_postprocessing_nearest_resize[2] = (GLfloat) S_nearest_w / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_nearest_resize[3] = 1.0f;

  G_texture_coord_buffer_postprocessing_nearest_resize[4] = 0.0f;
  G_texture_coord_buffer_postprocessing_nearest_resize[5] = 1.0f - (GLfloat) S_nearest_h / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  G_texture_coord_buffer_postprocessing_nearest_resize[6] = (GLfloat) S_nearest_w / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_nearest_resize[7] = 1.0f - (GLfloat) S_nearest_h / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_nearest_resize);
  glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), 
                G_texture_coord_buffer_postprocessing_nearest_resize, GL_STATIC_DRAW);

  /* set up postprocessing cubic resize vertex & texture coordinate buffers */
  G_vertex_buffer_postprocessing_cubic_resize[0] = 0.0f;
  G_vertex_buffer_postprocessing_cubic_resize[1] = 0.0f;
  G_vertex_buffer_postprocessing_cubic_resize[2] = 0.5f;

  G_vertex_buffer_postprocessing_cubic_resize[3] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_cubic_resize[4] = 0.0f;
  G_vertex_buffer_postprocessing_cubic_resize[5] = 0.5f;

  G_vertex_buffer_postprocessing_cubic_resize[6] = 0.0f;
  G_vertex_buffer_postprocessing_cubic_resize[7] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT;
  G_vertex_buffer_postprocessing_cubic_resize[8] = 0.5f;

  G_vertex_buffer_postprocessing_cubic_resize[9] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_cubic_resize[10] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT;
  G_vertex_buffer_postprocessing_cubic_resize[11] = 0.5f;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_cubic_resize);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_cubic_resize, GL_STATIC_DRAW);

  G_texture_coord_buffer_postprocessing_cubic_resize[0] = 0.0f;
  G_texture_coord_buffer_postprocessing_cubic_resize[1] = 1.0f;

  G_texture_coord_buffer_postprocessing_cubic_resize[2] = (GLfloat) S_window_w / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_cubic_resize[3] = 1.0f;

  G_texture_coord_buffer_postprocessing_cubic_resize[4] = 0.0f;
  G_texture_coord_buffer_postprocessing_cubic_resize[5] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  G_texture_coord_buffer_postprocessing_cubic_resize[6] = (GLfloat) S_window_w / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_cubic_resize[7] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_cubic_resize);
  glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), 
                G_texture_coord_buffer_postprocessing_cubic_resize, GL_STATIC_DRAW);

  /* set up postprocessing window vertex buffer */
  G_vertex_buffer_postprocessing_window[0] = 0.0f;
  G_vertex_buffer_postprocessing_window[1] = 0.0f;
  G_vertex_buffer_postprocessing_window[2] = 0.5f;

  G_vertex_buffer_postprocessing_window[3] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_window[4] = 0.0f;
  G_vertex_buffer_postprocessing_window[5] = 0.5f;

  G_vertex_buffer_postprocessing_window[6] = 0.0f;
  G_vertex_buffer_postprocessing_window[7] = (GLfloat) S_window_h;
  G_vertex_buffer_postprocessing_window[8] = 0.5f;

  G_vertex_buffer_postprocessing_window[9] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_window[10] = (GLfloat) S_window_h;
  G_vertex_buffer_postprocessing_window[11] = 0.5f;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_window, GL_STATIC_DRAW);

  /* set up postprocessing index buffer object */
  G_index_buffer_postprocessing_all[0] = 0;
  G_index_buffer_postprocessing_all[1] = 2;
  G_index_buffer_postprocessing_all[2] = 1;

  G_index_buffer_postprocessing_all[3] = 1;
  G_index_buffer_postprocessing_all[4] = 2;
  G_index_buffer_postprocessing_all[5] = 3;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, G_index_buffer_id_postprocessing_all);
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), 
                G_index_buffer_postprocessing_all, GL_STATIC_DRAW);

  return 0;
}

/*******************************************************************************
** graphics_destroy_opengl_objects()
*******************************************************************************/
short int graphics_destroy_opengl_objects()
{
  /* delete buffer objects */
  glDeleteBuffers(1, &G_vertex_buffer_id_tiles);
  glDeleteBuffers(1, &G_texture_coord_buffer_id_tiles);
  glDeleteBuffers(1, &G_palette_coord_buffer_id_tiles);
  glDeleteBuffers(1, &G_index_buffer_id_tiles);

  glDeleteBuffers(1, &G_vertex_buffer_id_sprites);
  glDeleteBuffers(1, &G_texture_coord_buffer_id_sprites);
  glDeleteBuffers(1, &G_palette_coord_buffer_id_sprites);
  glDeleteBuffers(1, &G_index_buffer_id_sprites);

  glDeleteBuffers(1, &G_vertex_buffer_id_postprocessing_overscan);
  glDeleteBuffers(1, &G_vertex_buffer_id_postprocessing_nearest_resize);
  glDeleteBuffers(1, &G_vertex_buffer_id_postprocessing_cubic_resize);
  glDeleteBuffers(1, &G_vertex_buffer_id_postprocessing_window);

  glDeleteBuffers(1, &G_texture_coord_buffer_id_postprocessing_overscan);
  glDeleteBuffers(1, &G_texture_coord_buffer_id_postprocessing_nearest_resize);
  glDeleteBuffers(1, &G_texture_coord_buffer_id_postprocessing_cubic_resize);

  glDeleteBuffers(1, &G_index_buffer_id_postprocessing_all);

  /* delete framebuffer objects */
  glDeleteFramebuffers(1, &G_framebuffer_id_intermediate_1);
  glDeleteRenderbuffers(1, &G_renderbuffer_id_intermediate_1);

  glDeleteFramebuffers(1, &G_framebuffer_id_intermediate_2);
  glDeleteRenderbuffers(1, &G_renderbuffer_id_intermediate_2);

  /* delete intermediate textures */
  glDeleteTextures(1, &G_texture_id_intermediate_1);
  glDeleteTextures(1, &G_texture_id_intermediate_2);

  /* delete opengl shader programs */
  glDeleteProgram(G_program_id_A);
  glDeleteProgram(G_program_id_B);
  glDeleteProgram(G_program_id_C);
  glDeleteProgram(G_program_id_D);
  glDeleteProgram(G_program_id_E);
  glDeleteProgram(G_program_id_OV1);
  glDeleteProgram(G_program_id_OV2);

  /* delete vertex array object */
  glDeleteVertexArrays(1, &G_vertex_array_id);

  /* free tile buffers */
  if (G_vertex_buffer_tiles != NULL)
  {
    free(G_vertex_buffer_tiles);
    G_vertex_buffer_tiles = NULL;
  }

  if (G_texture_coord_buffer_tiles != NULL)
  {
    free(G_texture_coord_buffer_tiles);
    G_texture_coord_buffer_tiles = NULL;
  }

  if (G_palette_coord_buffer_tiles != NULL)
  {
    free(G_palette_coord_buffer_tiles);
    G_palette_coord_buffer_tiles = NULL;
  }

  if (G_index_buffer_tiles != NULL)
  {
    free(G_index_buffer_tiles);
    G_index_buffer_tiles = NULL;
  }

  /* free sprite buffers */
  if (G_vertex_buffer_sprites != NULL)
  {
    free(G_vertex_buffer_sprites);
    G_vertex_buffer_sprites = NULL;
  }

  if (G_texture_coord_buffer_sprites != NULL)
  {
    free(G_texture_coord_buffer_sprites);
    G_texture_coord_buffer_sprites = NULL;
  }

  if (G_palette_coord_buffer_sprites != NULL)
  {
    free(G_palette_coord_buffer_sprites);
    G_palette_coord_buffer_sprites = NULL;
  }

  if (G_index_buffer_sprites != NULL)
  {
    free(G_index_buffer_sprites);
    G_index_buffer_sprites = NULL;
  }

  return 0;
}

/*******************************************************************************
** graphics_setup_overscan_mvp_matrix()
*******************************************************************************/
short int graphics_setup_overscan_mvp_matrix(int t_x, int t_y)
{
  /* orthographic projection matrix (overscan) */
  G_mvp_matrix_overscan[0]  = 2.0f / GRAPHICS_OVERSCAN_OUTPUT_WIDTH;
  G_mvp_matrix_overscan[1]  = 0.0f;
  G_mvp_matrix_overscan[2]  = 0.0f;
  G_mvp_matrix_overscan[3]  = 0.0f;

  G_mvp_matrix_overscan[4]  = 0.0f;
  G_mvp_matrix_overscan[5]  = -2.0f / GRAPHICS_OVERSCAN_OUTPUT_HEIGHT;
  G_mvp_matrix_overscan[6]  = 0.0f;
  G_mvp_matrix_overscan[7]  = 0.0f;

  G_mvp_matrix_overscan[8]  = 0.0f;
  G_mvp_matrix_overscan[9]  = 0.0f;
  G_mvp_matrix_overscan[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_overscan[11] = 0.0f;

  G_mvp_matrix_overscan[12] = ((2.0f * t_x) / GRAPHICS_OVERSCAN_OUTPUT_WIDTH) - 1.0f;
  G_mvp_matrix_overscan[13] = ((-2.0f * t_y) / GRAPHICS_OVERSCAN_OUTPUT_HEIGHT) + 1.0f;
  G_mvp_matrix_overscan[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_overscan[15] = 1.0f;

  return 0;
}

/*******************************************************************************
** graphics_set_graphics_resolution()
*******************************************************************************/
short int graphics_set_graphics_resolution(int resolution)
{
  /* set resolution */
  if (resolution == GRAPHICS_RESOLUTION_480P)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_480P;

    S_window_w = 854;
    S_window_h = 480;

    S_nearest_w = 640;
    S_nearest_h = 448;
  }
  else if (resolution == GRAPHICS_RESOLUTION_720P)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_720P;

    S_window_w = 1280;
    S_window_h = 720;

    S_nearest_w = 960;
    S_nearest_h = 672;
  }
  else if (resolution == GRAPHICS_RESOLUTION_768P)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_768P;

    S_window_w = 1366;
    S_window_h = 768;

    S_nearest_w = 960;
    S_nearest_h = 672;
  }
  else if (resolution == GRAPHICS_RESOLUTION_1080P)
  {
    G_graphics_resolution = GRAPHICS_RESOLUTION_1080P;

    S_window_w = 1920;
    S_window_h = 1080;

    S_nearest_w = 1280;
    S_nearest_h = 896;
  }

  return 0;
}

/*******************************************************************************
** graphics_read_desktop_dimensions()
*******************************************************************************/
short int graphics_read_desktop_dimensions()
{
  int             index;
  SDL_DisplayMode mode;

  /* determine display resolution */
  index = SDL_GetWindowDisplayIndex(G_sdl_window);
  SDL_GetDesktopDisplayMode(index, &mode);

  /* set desktop dimensions */
  S_desktop_w = mode.w;
  S_desktop_h = mode.h;

  /* determine if the window will fit on-screen */
  if ((S_desktop_w < S_window_w) || (S_desktop_h < S_window_h))
    return 1;

  return 0;
}

/*******************************************************************************
** graphics_setup_viewport_windowed()
*******************************************************************************/
short int graphics_setup_viewport_windowed()
{
  /* orthographic projection matrix (window) */
  G_mvp_matrix_window[0]  = 2.0f / S_window_w;
  G_mvp_matrix_window[1]  = 0.0f;
  G_mvp_matrix_window[2]  = 0.0f;
  G_mvp_matrix_window[3]  = 0.0f;

  G_mvp_matrix_window[4]  = 0.0f;
  G_mvp_matrix_window[5]  = -2.0f / S_window_h;
  G_mvp_matrix_window[6]  = 0.0f;
  G_mvp_matrix_window[7]  = 0.0f;

  G_mvp_matrix_window[8]  = 0.0f;
  G_mvp_matrix_window[9]  = 0.0f;
  G_mvp_matrix_window[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_window[11] = 0.0f;

  G_mvp_matrix_window[12] = -1.0f;
  G_mvp_matrix_window[13] = 1.0f;
  G_mvp_matrix_window[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_window[15] = 1.0f;

  /* set up postprocessing window vertex buffer object */
  G_vertex_buffer_postprocessing_window[0] = 0.0f;
  G_vertex_buffer_postprocessing_window[1] = 0.0f;
  G_vertex_buffer_postprocessing_window[2] = 0.5f;

  G_vertex_buffer_postprocessing_window[3] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_window[4] = 0.0f;
  G_vertex_buffer_postprocessing_window[5] = 0.5f;

  G_vertex_buffer_postprocessing_window[6] = 0.0f;
  G_vertex_buffer_postprocessing_window[7] = (GLfloat) S_window_h;
  G_vertex_buffer_postprocessing_window[8] = 0.5f;

  G_vertex_buffer_postprocessing_window[9] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_window[10] = (GLfloat) S_window_h;
  G_vertex_buffer_postprocessing_window[11] = 0.5f;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_window, GL_STATIC_DRAW);

  /* set up viewport width and height */
  G_viewport_w = S_window_w;
  G_viewport_h = S_window_h;

  return 0;
}

/*******************************************************************************
** graphics_setup_viewport_fullscreen()
*******************************************************************************/
short int graphics_setup_viewport_fullscreen()
{
  /* orthographic projection matrix (window) */
  G_mvp_matrix_window[0]  = 2.0f / S_desktop_w;
  G_mvp_matrix_window[1]  = 0.0f;
  G_mvp_matrix_window[2]  = 0.0f;
  G_mvp_matrix_window[3]  = 0.0f;

  G_mvp_matrix_window[4]  = 0.0f;
  G_mvp_matrix_window[5]  = -2.0f / S_desktop_h;
  G_mvp_matrix_window[6]  = 0.0f;
  G_mvp_matrix_window[7]  = 0.0f;

  G_mvp_matrix_window[8]  = 0.0f;
  G_mvp_matrix_window[9]  = 0.0f;
  G_mvp_matrix_window[10] = 1.6f; /* 2.0f / (1.375f - 0.125f); */
  G_mvp_matrix_window[11] = 0.0f;

  G_mvp_matrix_window[12] = -1.0f;
  G_mvp_matrix_window[13] = 1.0f;
  G_mvp_matrix_window[14] = -1.2f; /* -((1.375f + 0.125f) / (1.375f - 0.125f)) */
  G_mvp_matrix_window[15] = 1.0f;

  /* set up postprocessing window vertex buffer object */
  G_vertex_buffer_postprocessing_window[0]   = (GLfloat) (S_desktop_w - S_window_w) / 2.0f;
  G_vertex_buffer_postprocessing_window[1]   = (GLfloat) (S_desktop_h - S_window_h) / 2.0f;
  G_vertex_buffer_postprocessing_window[2]   = 0.5f;

  G_vertex_buffer_postprocessing_window[3]   = (GLfloat) (S_desktop_w + S_window_w) / 2.0f;
  G_vertex_buffer_postprocessing_window[4]   = (GLfloat) (S_desktop_h - S_window_h) / 2.0f;
  G_vertex_buffer_postprocessing_window[5]   = 0.5f;

  G_vertex_buffer_postprocessing_window[6]   = (GLfloat) (S_desktop_w - S_window_w) / 2.0f;
  G_vertex_buffer_postprocessing_window[7]   = (GLfloat) (S_desktop_h + S_window_h) / 2.0f;
  G_vertex_buffer_postprocessing_window[8]   = 0.5f;

  G_vertex_buffer_postprocessing_window[9]   = (GLfloat) (S_desktop_w + S_window_w) / 2.0f;
  G_vertex_buffer_postprocessing_window[10]  = (GLfloat) (S_desktop_h + S_window_h) / 2.0f;
  G_vertex_buffer_postprocessing_window[11]  = 0.5f;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_window);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_window, GL_STATIC_DRAW);

  /* set up viewport width and height */
  G_viewport_w = S_desktop_w;
  G_viewport_h = S_desktop_h;

  return 0;
}

/*******************************************************************************
** graphics_setup_resized_buffers()
*******************************************************************************/
short int graphics_setup_resized_buffers()
{
  /* set up postprocessing nearest resize vertex & texture coordinate buffers */
  G_vertex_buffer_postprocessing_nearest_resize[0] = 0.0f;
  G_vertex_buffer_postprocessing_nearest_resize[1] = 0.0f;
  G_vertex_buffer_postprocessing_nearest_resize[2] = 0.5f;

  G_vertex_buffer_postprocessing_nearest_resize[3] = (GLfloat) S_nearest_w;
  G_vertex_buffer_postprocessing_nearest_resize[4] = 0.0f;
  G_vertex_buffer_postprocessing_nearest_resize[5] = 0.5f;

  G_vertex_buffer_postprocessing_nearest_resize[6] = 0.0f;
  G_vertex_buffer_postprocessing_nearest_resize[7] = (GLfloat) S_nearest_h;
  G_vertex_buffer_postprocessing_nearest_resize[8] = 0.5f;

  G_vertex_buffer_postprocessing_nearest_resize[9] = (GLfloat) S_nearest_w;
  G_vertex_buffer_postprocessing_nearest_resize[10] = (GLfloat) S_nearest_h;
  G_vertex_buffer_postprocessing_nearest_resize[11] = 0.5f;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_nearest_resize);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_nearest_resize, GL_STATIC_DRAW);

  G_texture_coord_buffer_postprocessing_nearest_resize[0] = 0.0f;
  G_texture_coord_buffer_postprocessing_nearest_resize[1] = 1.0f;

  G_texture_coord_buffer_postprocessing_nearest_resize[2] = (GLfloat) S_nearest_w / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_nearest_resize[3] = 1.0f;

  G_texture_coord_buffer_postprocessing_nearest_resize[4] = 0.0f;
  G_texture_coord_buffer_postprocessing_nearest_resize[5] = 1.0f - (GLfloat) S_nearest_h / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  G_texture_coord_buffer_postprocessing_nearest_resize[6] = (GLfloat) S_nearest_w / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_nearest_resize[7] = 1.0f - (GLfloat) S_nearest_h / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_nearest_resize);
  glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), 
                G_texture_coord_buffer_postprocessing_nearest_resize, GL_STATIC_DRAW);

  /* set up postprocessing cubic resize vertex & texture coordinate buffers */
  G_vertex_buffer_postprocessing_cubic_resize[0] = 0.0f;
  G_vertex_buffer_postprocessing_cubic_resize[1] = 0.0f;
  G_vertex_buffer_postprocessing_cubic_resize[2] = 0.5f;

  G_vertex_buffer_postprocessing_cubic_resize[3] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_cubic_resize[4] = 0.0f;
  G_vertex_buffer_postprocessing_cubic_resize[5] = 0.5f;

  G_vertex_buffer_postprocessing_cubic_resize[6] = 0.0f;
  G_vertex_buffer_postprocessing_cubic_resize[7] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT;
  G_vertex_buffer_postprocessing_cubic_resize[8] = 0.5f;

  G_vertex_buffer_postprocessing_cubic_resize[9] = (GLfloat) S_window_w;
  G_vertex_buffer_postprocessing_cubic_resize[10] = (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT;
  G_vertex_buffer_postprocessing_cubic_resize[11] = 0.5f;

  glBindBuffer(GL_ARRAY_BUFFER, G_vertex_buffer_id_postprocessing_cubic_resize);
  glBufferData( GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), 
                G_vertex_buffer_postprocessing_cubic_resize, GL_STATIC_DRAW);

  G_texture_coord_buffer_postprocessing_cubic_resize[0] = 0.0f;
  G_texture_coord_buffer_postprocessing_cubic_resize[1] = 1.0f;

  G_texture_coord_buffer_postprocessing_cubic_resize[2] = (GLfloat) S_window_w / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_cubic_resize[3] = 1.0f;

  G_texture_coord_buffer_postprocessing_cubic_resize[4] = 0.0f;
  G_texture_coord_buffer_postprocessing_cubic_resize[5] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  G_texture_coord_buffer_postprocessing_cubic_resize[6] = (GLfloat) S_window_w / GRAPHICS_INTERMEDIATE_TEXTURE_WIDTH;
  G_texture_coord_buffer_postprocessing_cubic_resize[7] = 1.0f - (GLfloat) GRAPHICS_OVERSCAN_OUTPUT_HEIGHT / GRAPHICS_INTERMEDIATE_TEXTURE_HEIGHT;

  glBindBuffer(GL_ARRAY_BUFFER, G_texture_coord_buffer_id_postprocessing_cubic_resize);
  glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), 
                G_texture_coord_buffer_postprocessing_cubic_resize, GL_STATIC_DRAW);

  return 0;
}

/*******************************************************************************
** graphics_setup_viewport_mvp_matrix_and_buffers()
*******************************************************************************/
short int graphics_setup_viewport_mvp_matrix_and_buffers()
{
  /* setup windowed or fullscreen mode */
  if (G_flag_window_fullscreen == 0)
    graphics_setup_viewport_windowed();
  else
    graphics_setup_viewport_fullscreen();

  /* set up postprocessing resized buffers */
  graphics_setup_resized_buffers();

  return 0;
}

/*******************************************************************************
** graphics_set_window_size()
*******************************************************************************/
short int graphics_set_window_size(int res)
{
  int old_res;

  /* make sure the resolution is valid */
  if ((res < 0) && (res >= GRAPHICS_NUM_RESOLUTIONS))
    return 0;

  /* note: we still want to run this function even if we are    */
  /* already in this resolution, because this function is also  */
  /* used at startup for initializing the default resolution    */

  /* store old resolution */
  old_res = G_graphics_resolution;

  /* try to set the new resolution */
  graphics_set_graphics_resolution(res);

  /* make sure the new window size will fit on the desktop */
  if (graphics_read_desktop_dimensions() != 0)
  {
    graphics_set_graphics_resolution(old_res);
    return 1;
  }

  /* resize the window if not in fullscreen */
  if (G_flag_window_fullscreen == 0)
  {
    SDL_SetWindowSize(G_sdl_window, S_window_w, S_window_h);
    SDL_SetWindowPosition(G_sdl_window, 
                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  }

  /* setup mvp matrix and buffers */
  graphics_setup_viewport_mvp_matrix_and_buffers();

  return 0;
}

/*******************************************************************************
** graphics_increase_window_size()
*******************************************************************************/
short int graphics_increase_window_size()
{
  if (G_graphics_resolution < GRAPHICS_NUM_RESOLUTIONS - 1)
  {
    if (graphics_set_window_size(G_graphics_resolution + 1) == 0)
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
  }

  return 0;
}

/*******************************************************************************
** graphics_decrease_window_size()
*******************************************************************************/
short int graphics_decrease_window_size()
{
  if (G_graphics_resolution > 0)
  {
    if (graphics_set_window_size(G_graphics_resolution - 1) == 0)
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
  }

  return 0;
}

/*******************************************************************************
** graphics_initialize_fullscreen()
*******************************************************************************/
short int graphics_initialize_fullscreen(int flag)
{
  /* this is called on startup when loading the options */

  /* switch to fullscreen */
  if ((flag == 1) && (G_flag_window_fullscreen == 0))
  {
    if (graphics_read_desktop_dimensions() == 0)
    {
      if (SDL_SetWindowFullscreen(G_sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP) == 0)
      {
        SDL_ShowCursor(SDL_DISABLE);
        G_flag_window_fullscreen = 1;
        graphics_setup_viewport_mvp_matrix_and_buffers();
      }
    }
  }
  /* switch to windowed (should not happen, since we start in windowed) */
  else if ((flag == 0) && (G_flag_window_fullscreen == 1))
  {
    if (SDL_SetWindowFullscreen(G_sdl_window, 0) == 0)
    {
      SDL_ShowCursor(SDL_ENABLE);
      G_flag_window_fullscreen = 0;
      graphics_read_desktop_dimensions();

      SDL_SetWindowSize(G_sdl_window, S_window_w, S_window_h);
      SDL_SetWindowPosition(G_sdl_window, 
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

      graphics_setup_viewport_mvp_matrix_and_buffers();
    }
  }

  return 0;
}

/*******************************************************************************
** graphics_toggle_fullscreen()
*******************************************************************************/
short int graphics_toggle_fullscreen()
{
  /* switch to fullscreen */
  if (G_flag_window_fullscreen == 0)
  {
    if (graphics_read_desktop_dimensions() == 0)
    {
      if (SDL_SetWindowFullscreen(G_sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP) == 0)
      {
        SDL_ShowCursor(SDL_DISABLE);
        G_flag_window_fullscreen = 1;
        graphics_setup_viewport_mvp_matrix_and_buffers();
        doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
      }
    }
  }
  /* switch to windowed */
  else if (G_flag_window_fullscreen == 1)
  {
    if (SDL_SetWindowFullscreen(G_sdl_window, 0) == 0)
    {
      SDL_ShowCursor(SDL_ENABLE);
      G_flag_window_fullscreen = 0;
      graphics_read_desktop_dimensions();

      SDL_SetWindowSize(G_sdl_window, S_window_w, S_window_h);
      SDL_SetWindowPosition(G_sdl_window, 
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

      graphics_setup_viewport_mvp_matrix_and_buffers();
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }

  return 0;
}

/*******************************************************************************
** graphics_initialize_vsync()
*******************************************************************************/
short int graphics_initialize_vsync(int flag)
{
  /* this is called on startup when loading the options */

  /* vsync off */
  if ((flag == 0) && (G_flag_window_vsync == 1))
  {
    if (SDL_GL_SetSwapInterval(0) == 0)
      G_flag_window_vsync = 0;
  }
  /* vsync on */
  else if ((flag == 1) && (G_flag_window_vsync == 0))
  {
    if (SDL_GL_SetSwapInterval(1) == 0)
      G_flag_window_vsync = 1;
  }

  return 0;
}

/*******************************************************************************
** graphics_toggle_vsync()
*******************************************************************************/
short int graphics_toggle_vsync()
{
  /* turn vsync on */
  if (G_flag_window_vsync == 0)
  {
    if (SDL_GL_SetSwapInterval(1) == 0)
    {
      G_flag_window_vsync = 1;
      doremi_play_sfx(SFX_INDEX_TOGGLE_DOWN);
    }
  }
  /* turn vsync off */
  else
  {
    if (SDL_GL_SetSwapInterval(0) == 0)
    {
      G_flag_window_vsync = 0;
      doremi_play_sfx(SFX_INDEX_TOGGLE_UP);
    }
  }

  return 0;
}
