/*******************************************************************************
** hola.c (startup functions)
*******************************************************************************/

#include <SDL2/SDL.h>

#ifdef CARROT_MAC_OSX
  #include <SDL2_mixer/SDL_mixer.h>
#else
  #include <SDL2/SDL_mixer.h>
#endif

#include <glad/glad.h>

#include <stdio.h>

#include "global.h"
#include "graphics.h"

/*******************************************************************************
** hola_window()
*******************************************************************************/
short int hola_window()
{
  /* set opengl attributes */
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  /* set opengl version to 3.3 */
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  /* create window */
  G_sdl_window = SDL_CreateWindow("The Year of the Carrot",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  854,
                                  480,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

  if (G_sdl_window == NULL)
  {
    printf("Failed to create window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  return 0;
}

/*******************************************************************************
** hola_mixer()
*******************************************************************************/
short int hola_mixer()
{
  int initialized_flags;

  /* initialize sdl mixer */
  initialized_flags = Mix_Init(MIX_INIT_MOD);

  if ((initialized_flags & MIX_INIT_MOD) != MIX_INIT_MOD)
  {
    printf("Failed to initialize SDL_mixer MOD: %s\n", Mix_GetError());
    return 1;
  }

  if (Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 512) != 0)
  {
    fprintf(stdout, "Failed to initialize SDL_mixer API: %s\n", Mix_GetError());
    return 1;
  }

  return 0;
}

/*******************************************************************************
** hola_opengl()
*******************************************************************************/
short int hola_opengl()
{
  SDL_GLContext context;

  /* create opengl context */
  context = SDL_GL_CreateContext(G_sdl_window);

  if (context == NULL)
  {
    fprintf(stdout, "Failed to create OpenGL context: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GL_MakeCurrent(G_sdl_window, context);

  /* obtain opengl function pointers with glad */
  if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
  {
    fprintf(stdout, "Failed to obtain OpenGL function pointers.\n");
    return 1;
  }

  /* setup vsync */
  if (SDL_GL_SetSwapInterval(1) == 0)
    G_flag_window_vsync = 1;
  else if (SDL_GL_SetSwapInterval(0) == 0)
    G_flag_window_vsync = 0;

  /* initialize 480p windowed mode */
  G_flag_window_fullscreen = 0;
  G_graphics_resolution = GRAPHICS_RESOLUTION_480P;

  if (graphics_set_window_size(GRAPHICS_RESOLUTION_480P) != 0)
  {
    fprintf(stdout, "Failed to set 480p window size.\n");
    return 1;
  }

  return 0;
}

