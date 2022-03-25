/*******************************************************************************
** The Year of the Carrot - Michael Behrens 2021
*******************************************************************************/

/*******************************************************************************
** main.c
*******************************************************************************/

#include <SDL2/SDL.h>

#ifdef CARROT_MAC_OSX
  #include <SDL2_mixer/SDL_mixer.h>
#else
  #include <SDL2/SDL_mixer.h>
#endif

#include <glad/glad.h>

#include <stdio.h>

#include "controls.h"
#include "doremi.h"
#include "fade.h"
#include "gameloop.h"
#include "global.h"
#include "graphics.h"
#include "grid.h"
#include "hola.h"
#include "level.h"
#include "options.h"
#include "path.h"
#include "render.h"
#include "savegame.h"
#include "screen.h"
#include "story.h"
#include "texture.h"
#include "video.h"
#include "world.h"

/*******************************************************************************
** main()
*******************************************************************************/
int main(int argc, char *argv[])
{
  SDL_Event event;
  Uint32    ticks_last_update;
  Uint32    ticks_current;

  /* initialize sdl */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
  {
    fprintf(stdout, "Failed to initialize SDL: %s\n", SDL_GetError());
    return 0;
  }

  /* load opengl library */
  if (SDL_GL_LoadLibrary(NULL))
  {
    fprintf(stdout, "Failed to load OpenGL Library: %s\n", SDL_GetError());
    goto cleanup_sdl;
  }

  /* initialize global variables */
  fade_init_variables();
  globals_init_variables();
  video_init_variables();

  /* initialize paths */
  path_init_paths();

  if (path_obtain_base_paths())
  {
    fprintf(stdout, "Failed to obtain Base paths. Exiting...\n");
    goto cleanup_gl_library;
  }

  if (path_obtain_preferences_path())
  {
    fprintf(stdout, "Failed to obtain Preferences path. Exiting...\n");
    goto cleanup_paths;
  }

  /* create window */
  if (hola_window())
  {
    fprintf(stdout, "Failed to create window. Exiting...\n");
    goto cleanup_paths;
  }

  /* initialize sdl mixer */
  if (hola_mixer())
  {
    fprintf(stdout, "Failed to initialize SDL_mixer. Exiting...\n");
    goto cleanup_window;
  }

  /* initialize opengl */
  if (hola_opengl())
  {
    fprintf(stdout, "Failed to initialize OpenGL. Exiting...\n");
    goto cleanup_mixer;
  }

  /* initialize opengl objects */
  if (graphics_create_opengl_objects())
  {
    fprintf(stdout, "Failed to initialize OpenGL objects. Exiting...\n");
    goto cleanup_opengl;
  }

  /* initialize opengl textures */
  if (textures_init())
  {
    fprintf(stdout, "Failed to initialize textures. Exiting...\n");
    goto cleanup_opengl_objects;
  }

  /* initialize world */
  if (world_reset())
  {
    fprintf(stdout, "Failed to initialize world. Exiting...\n");
    goto cleanup_textures;
  }

  /* initialize collision grid */
  if (grid_reset())
  {
    fprintf(stdout, "Failed to initialize collision grid. Exiting...\n");
    goto cleanup_world;
  }

  /* initialize controller input states */
  controls_setup();
  controls_plug_in_all_gamepads();

  /* generate texture coordinate tables */
  graphics_generate_tables();

  /* generate palette texture */
  texture_generate_palette();

  /* load all textures */
  if (texture_load_all_from_file(G_path_gfx_data))
  {
    fprintf(stdout, "Error loading gfx data. Exiting...\n");
    goto cleanup_gamepads;
  }

  /* initialize volume */
  doremi_set_volume(8);

  /* load all sfx data */
  doremi_init_all_sfx();

  if (doremi_load_all_sfx_from_file(G_path_sfx_data))
  {
    fprintf(stdout, "Error loading sfx data. Exiting...\n");
    goto cleanup_gamepads;
  }

  /* initialize music */
  doremi_init_music();

  /* load level data */
  level_init_arrays();

  if (level_load_all_from_file(G_path_level_data))
  {
    fprintf(stdout, "Error loading level data. Exiting...\n");
    goto cleanup_sfx_and_music;
  }

  /* load story scene text */
  story_init_arrays();

  if (story_load_all_from_file(G_path_story_data))
  {
    fprintf(stdout, "Error loading story data. Exiting...\n");
    goto cleanup_sfx_and_music;
  }

  /* initialize save game */
  save_game_set_slot(1);

  /* initialize options */
  options_load();

  /* initialize game screen */
  game_loop_change_screen(GAME_SCREEN_TITLE, 0, 0, 0);
  fade_start_fadein();

  /* initialize ticks */
  ticks_current = SDL_GetTicks();
  ticks_last_update = ticks_current;

  /* initialize minimization flag */
  G_flag_window_minimized = 0;

  /* main loop */
  while (1)
  {
    /* process sdl events */
    while (SDL_PollEvent(&event))
    {
      /* quit */
      if (event.type == SDL_QUIT)
      {
        goto cleanup_all;
      }

      /* window */
      if (event.type == SDL_WINDOWEVENT)
      {
        /* if window is closed, quit */
        if (event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
          goto cleanup_all;
        }

        /* if focus is lost, pause */
        if ((event.window.event == SDL_WINDOWEVENT_MINIMIZED) ||
            (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST))
        {
          doremi_pause_music();
          G_flag_window_minimized = 1;
        }

        /* if focus is gained, unpause */
        if ((event.window.event == SDL_WINDOWEVENT_RESTORED) ||
            (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED))
        {
          doremi_unpause_music();
          G_flag_window_minimized = 0;
          ticks_last_update = SDL_GetTicks();
        }
      }

      /* keyboard (key down) */
      if (event.type == SDL_KEYDOWN)
      {
        if ((event.key.state == SDL_PRESSED) && (event.key.repeat == 0))
          controls_keyboard_key_pressed(event.key.keysym.scancode);
      }

      /* keyboard (key up) */
      if (event.type == SDL_KEYUP)
      {
        if ((event.key.state == SDL_RELEASED) && (event.key.repeat == 0))
          controls_keyboard_key_released(event.key.keysym.scancode);
      }

      /* gamepad (plugged in) */
      if (event.type == SDL_CONTROLLERDEVICEADDED)
      {
        controls_unplug_all_gamepads();
        controls_plug_in_all_gamepads();
      }

      /* gamepad (unplugged) */
      if (event.type == SDL_CONTROLLERDEVICEREMOVED)
      {
        controls_unplug_all_gamepads();
        controls_plug_in_all_gamepads();
      }

      /* gamepad (remapped) */
      if (event.type == SDL_CONTROLLERDEVICEREMAPPED)
      {
        controls_reset_all_states();
      }

      /* gamepad (button down) */
      if (event.type == SDL_CONTROLLERBUTTONDOWN)
      {
        if (event.cbutton.state == SDL_PRESSED)
        {
          controls_gamepad_button_pressed(event.cbutton.which, 
                                          event.cbutton.button);
        }
      }

      /* gamepad (button up) */
      if (event.type == SDL_CONTROLLERBUTTONUP)
      {
        if (event.cbutton.state == SDL_RELEASED)
        {
          controls_gamepad_button_released( event.cbutton.which, 
                                            event.cbutton.button);
        }
      }

      /* gamepad (axis moved) */
      if (event.type == SDL_CONTROLLERAXISMOTION)
      {
        controls_gamepad_axis_moved(event.cbutton.which, 
                                    event.caxis.axis, 
                                    event.caxis.value);
      }
    }

    /* make sure the window is not minimized */
    if (G_flag_window_minimized == 1)
      continue;

    /* update ticks */
    ticks_current = SDL_GetTicks();

    /* check for tick wraparound (~49 days) */
    if (ticks_current < ticks_last_update)
      ticks_last_update = 0;

    /* check if a new frame has elapsed */
    if ((ticks_current - ticks_last_update) >= (1000 / 60))
    {
      /* advance frame */
      game_loop_advance_frame();

      /* quit */
      if (G_flag_quit_game == 1)
      {
        goto cleanup_all;
      }

      /* update window */
      SDL_GL_SwapWindow(G_sdl_window);

      /* store this update time */
      ticks_last_update = ticks_current;
    }
  }

  /* cleanup window and quit */
cleanup_all:
cleanup_sfx_and_music:
  doremi_deinit_all_sfx();
  doremi_deinit_music();
cleanup_gamepads:
  controls_unplug_all_gamepads();
cleanup_world:
cleanup_textures:
  textures_deinit();
cleanup_opengl_objects:
  graphics_destroy_opengl_objects();
cleanup_opengl:
  SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());
cleanup_mixer:
  Mix_CloseAudio();

  while (Mix_Init(0))
    Mix_Quit();
cleanup_window:
  SDL_DestroyWindow(G_sdl_window);
cleanup_paths:
  path_free_paths();
cleanup_gl_library:
  SDL_GL_UnloadLibrary();
cleanup_sdl:
  SDL_Quit();

  return 0;
}
