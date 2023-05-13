/*******************************************************************************
** world.c (the world of right now)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "animate.h"
#include "color.h"
#include "global.h"
#include "grid.h"
#include "level.h"
#include "state.h"
#include "subpixel.h"
#include "thing.h"
#include "world.h"

thing G_world_all_things[WORLD_MAX_THINGS];
int   G_world_riders[WORLD_MAX_RIDERS];

/*******************************************************************************
** world_reset()
*******************************************************************************/
short int world_reset()
{
  int i;

  thing* t;

  /* reset all things */
  for (i = 0; i < WORLD_MAX_THINGS; i++)
  {
    t = &G_world_all_things[i];

    t->type   = THING_TYPE_NONE;
    t->color  = COLOR_NONE;
    t->state  = STATE_NONE;
    t->orient = THING_ORIENT_NORMAL;

    t->flag   = THING_COLLISION_FLAG_NONE;

    t->pos_x  = 0;
    t->pos_y  = 0;

    t->vel_x  = 0;
    t->vel_y  = 0;

    t->hx     = 0;
    t->hy     = 0;

    t->timer_offset = 0;
  }

  /* reset riders */
  for (i = 0; i < WORLD_MAX_RIDERS; i++)
    G_world_riders[i] = -1;

  return 0;
}

/*******************************************************************************
** world_init_riders()
*******************************************************************************/
short int world_init_riders()
{
  int i;

  /* reset riders */
  for (i = 0; i < WORLD_MAX_RIDERS; i++)
    G_world_riders[i] = -1;

  return 0;
}

/*******************************************************************************
** world_spawn_thing()
*******************************************************************************/
short int world_spawn_thing(int type, int color, int state, int orient, 
                            int pos_x, int pos_y, int vel_x, int vel_y, 
                            unsigned int timer_offset)
{
  int i;

  thing* t;

  int start_index;
  int end_index;
  int insert_index;

  /* determine start & end indices */
  if (type == THING_TYPE_BUNNY)
  {
    start_index = WORLD_BUNNY_INDEX;
    end_index = WORLD_BUNNY_INDEX + 1;
  }
  else if ( (type == THING_TYPE_HELD_MARBLE)            || 
            (type == THING_TYPE_HELD_CRITTER_WALKER_1)  || 
            (type == THING_TYPE_HELD_CRITTER_WALKER_2)  || 
            (type == THING_TYPE_HELD_CRITTER_FLYER_1)   || 
            (type == THING_TYPE_HELD_CRITTER_FLYER_2))
  {
    start_index = WORLD_HELD_THING_INDEX;
    end_index = WORLD_HELD_THING_INDEX + 1;
  }
  else if ( (type == THING_TYPE_BUBBLE_HORIZONTAL)    || 
            (type == THING_TYPE_BUBBLE_VERTICAL)      || 
            (type == THING_TYPE_BUBBLE_STRESSED)      || 
            (type == THING_TYPE_BUBBLE_POPPED))
  {
    start_index = WORLD_BUBBLES_START_INDEX;
    end_index = WORLD_BUBBLES_END_INDEX;
  }
  else if ( (type == THING_TYPE_BASS_KNOB)            || 
            (type == THING_TYPE_TREBLE_KNOB)          || 
            (type == THING_TYPE_ROCKET_BLOCK_RIGHT)   || 
            (type == THING_TYPE_ROCKET_BLOCK_LEFT)    || 
            (type == THING_TYPE_ELEVATOR_HORI_RIGHT)  || 
            (type == THING_TYPE_ELEVATOR_HORI_LEFT)   || 
            (type == THING_TYPE_ELEVATOR_VERT_UP)     || 
            (type == THING_TYPE_ELEVATOR_VERT_DOWN)   || 
            (type == THING_TYPE_DROP_BOLT_DOWN)       || 
            (type == THING_TYPE_DROP_BOLT_UP)         || 
            (type == THING_TYPE_ANTI_BOLT_UP)         || 
            (type == THING_TYPE_ANTI_BOLT_DOWN)       || 
            (type == THING_TYPE_PLATFORM))
  {
    start_index = WORLD_PLATFORMS_ETC_START_INDEX;
    end_index = WORLD_PLATFORMS_ETC_END_INDEX;
  }
  else if ( (type == THING_TYPE_CARROT)       || 
            (type == THING_TYPE_BOOK)         || 
            (type == THING_TYPE_MUSIC_NOTES)  || 
            (type == THING_TYPE_MAGNET_DOWN)  || 
            (type == THING_TYPE_MAGNET_UP)    || 
            (type == THING_TYPE_MAGNET_RIGHT) || 
            (type == THING_TYPE_MAGNET_LEFT)  || 
            (type == THING_TYPE_GEM)          || 
            (type == THING_TYPE_KEY)          || 
            (type == THING_TYPE_MUSHROOM))
  {
    start_index = WORLD_PICKUPS_START_INDEX;
    end_index = WORLD_PICKUPS_END_INDEX;
  }
  else if ( (type == THING_TYPE_CRITTER_WALKER_1)     || 
            (type == THING_TYPE_CRITTER_WALKER_2)     || 
            (type == THING_TYPE_CRITTER_FLYER_1)      || 
            (type == THING_TYPE_CRITTER_FLYER_2))
  {
    start_index = WORLD_CRITTERS_START_INDEX;
    end_index = WORLD_CRITTERS_END_INDEX;
  }
  else if ( (type == THING_TYPE_THROWN_MARBLE)            || 
            (type == THING_TYPE_THROWN_CRITTER_WALKER_1)  || 
            (type == THING_TYPE_THROWN_CRITTER_WALKER_2)  || 
            (type == THING_TYPE_THROWN_CRITTER_FLYER_1)   || 
            (type == THING_TYPE_THROWN_CRITTER_FLYER_2))
  {
    start_index = WORLD_THROWN_THINGS_START_INDEX;
    end_index = WORLD_THROWN_THINGS_END_INDEX;
  }
  else
    return 1;

  /* find open spot, if possible */
  insert_index = -1;

  for (i = start_index; i < end_index; i++)
  {
    if (G_world_all_things[i].type == THING_TYPE_NONE)
    {
      insert_index = i;
      break;
    }
  }

  /* insert this thing into the world */
  if (insert_index == -1)
    return 1;

  t = &G_world_all_things[insert_index];

  /* set type */
  t->type = type;

  /* set color */
  if ((type == THING_TYPE_MUSIC_NOTES)  || 
      (type == THING_TYPE_KEY)          || 
      (type == THING_TYPE_MUSHROOM))
  {
    if ((color == COLOR_ORANGE) || 
        (color == COLOR_PINK)   || 
        (color == COLOR_BLUE)   || 
        (color == COLOR_GREEN))
    {
      t->color = color;
    }
    else
      t->color = COLOR_ORANGE;
  }
  else if ( (type == THING_TYPE_GEM)          || 
            (type == THING_TYPE_HELD_MARBLE)  || 
            (type == THING_TYPE_THROWN_MARBLE))
  {
    if ((color == COLOR_METAL)  || 
        (color == COLOR_ORANGE) || 
        (color == COLOR_PINK)   || 
        (color == COLOR_BLUE)   || 
        (color == COLOR_GREEN)  || 
        (color == COLOR_WILD))
    {
      t->color = color;
    }
    else
      t->color = COLOR_ORANGE;
  }
  else
    t->color = COLOR_NONE;

  /* set state */
  /* note that the passed in argument is only used in the */
  /* case of marbles, critters, bolts, bass/treble knobs, */
  /* and rocket blocks. otherwise, it is ignored.         */
  if ((type == THING_TYPE_HELD_MARBLE) || 
      (type == THING_TYPE_THROWN_MARBLE))
  {
    if ((state == STATE_MARBLE_FLASHING) || 
        (state == STATE_MARBLE_CHANGING))
    {
      t->state = state;
    }
    else
      t->state = STATE_NONE;
  }
  else if ( (type == THING_TYPE_CRITTER_WALKER_1) || 
            (type == THING_TYPE_CRITTER_WALKER_2) || 
            (type == THING_TYPE_CRITTER_FLYER_1)  || 
            (type == THING_TYPE_CRITTER_FLYER_2))
  {
    if (state == STATE_CRITTER_STUNNED)
      t->state = state;
    else
      t->state = STATE_NONE;
  }
  else if ( (type == THING_TYPE_BASS_KNOB)          || 
            (type == THING_TYPE_TREBLE_KNOB)        || 
            (type == THING_TYPE_ROCKET_BLOCK_RIGHT) || 
            (type == THING_TYPE_ROCKET_BLOCK_LEFT)  || 
            (type == THING_TYPE_DROP_BOLT_DOWN)     || 
            (type == THING_TYPE_DROP_BOLT_UP)       || 
            (type == THING_TYPE_ANTI_BOLT_UP)       || 
            (type == THING_TYPE_ANTI_BOLT_DOWN))
  {
    if (state == STATE_RETRACTING_STOPPED)
      t->state = state;
    else
      t->state = STATE_NONE;
  }
  else
    t->state = STATE_NONE;

  /* set orientation */
  /* note that the passed in argument is  */
  /* only used in the case of critters.   */
  /* otherwise, it is ignored.            */
  if ((type == THING_TYPE_MAGNET_UP)     || 
      (type == THING_TYPE_DROP_BOLT_UP)  || 
      (type == THING_TYPE_ANTI_BOLT_DOWN))
  {
    t->orient = THING_ORIENT_FLIP_VERT;
  }
  else if (type == THING_TYPE_MAGNET_LEFT)
  {
    t->orient = THING_ORIENT_FLIP_HORI;
  }
  else if ( (type == THING_TYPE_CRITTER_WALKER_1)         || 
            (type == THING_TYPE_CRITTER_WALKER_2)         || 
            (type == THING_TYPE_CRITTER_FLYER_1)          || 
            (type == THING_TYPE_CRITTER_FLYER_2)          || 
            (type == THING_TYPE_THROWN_CRITTER_WALKER_1)  || 
            (type == THING_TYPE_THROWN_CRITTER_WALKER_2)  || 
            (type == THING_TYPE_THROWN_CRITTER_FLYER_1)   || 
            (type == THING_TYPE_THROWN_CRITTER_FLYER_2))
  {
    if ((orient == THING_ORIENT_NORMAL)     || 
        (orient == THING_ORIENT_FLIP_HORI)  || 
        (orient == THING_ORIENT_FLIP_VERT)  || 
        (orient == THING_ORIENT_ROTATE_180))
    {
      t->orient = orient;
    }
    else
      t->orient = THING_ORIENT_NORMAL;
  }
  else
    t->orient = THING_ORIENT_NORMAL;

  /* set hitbox size based on type */
  if (type == THING_TYPE_BUNNY)
  {
    t->hx = 5 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 8 * SUBPIXEL_MANTISSA_FULL;
  }
  else if (type == THING_TYPE_CARROT)
  {
    t->hx = 4 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 7 * SUBPIXEL_MANTISSA_FULL;
  }
  else if (type == THING_TYPE_BOOK)
  {
    t->hx = 6 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 7 * SUBPIXEL_MANTISSA_FULL;
  }
  else if (type == THING_TYPE_MUSIC_NOTES)
  {
    t->hx = 8 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 7 * SUBPIXEL_MANTISSA_FULL;
  }
  else if ( (type == THING_TYPE_MAGNET_DOWN)  || 
            (type == THING_TYPE_MAGNET_UP)    || 
            (type == THING_TYPE_MAGNET_RIGHT) || 
            (type == THING_TYPE_MAGNET_LEFT))
  {
    t->hx = 7 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 7 * SUBPIXEL_MANTISSA_FULL;
  }
  else if (type == THING_TYPE_GEM)
  {
    t->hx = 8 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 7 * SUBPIXEL_MANTISSA_FULL;
  }
  else if (type == THING_TYPE_KEY)
  {
    t->hx = 4 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 7 * SUBPIXEL_MANTISSA_FULL;
  }
  else if (type == THING_TYPE_MUSHROOM)
  {
    t->hx = 6 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 7 * SUBPIXEL_MANTISSA_FULL;
  }
  else if ( (type == THING_TYPE_CRITTER_WALKER_1)         || 
            (type == THING_TYPE_CRITTER_WALKER_2)         || 
            (type == THING_TYPE_CRITTER_FLYER_1)          || 
            (type == THING_TYPE_CRITTER_FLYER_2)          || 
            (type == THING_TYPE_THROWN_CRITTER_WALKER_1)  || 
            (type == THING_TYPE_THROWN_CRITTER_WALKER_2)  || 
            (type == THING_TYPE_THROWN_CRITTER_FLYER_1)   || 
            (type == THING_TYPE_THROWN_CRITTER_FLYER_2))
  {
    t->hx = 7 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 8 * SUBPIXEL_MANTISSA_FULL;
  }
  else if ( (type == THING_TYPE_BASS_KNOB)            || 
            (type == THING_TYPE_TREBLE_KNOB)          || 
            (type == THING_TYPE_ROCKET_BLOCK_RIGHT)   || 
            (type == THING_TYPE_ROCKET_BLOCK_LEFT)    || 
            (type == THING_TYPE_ELEVATOR_HORI_RIGHT)  || 
            (type == THING_TYPE_ELEVATOR_HORI_LEFT)   || 
            (type == THING_TYPE_ELEVATOR_VERT_UP)     || 
            (type == THING_TYPE_ELEVATOR_VERT_DOWN)   || 
            (type == THING_TYPE_DROP_BOLT_DOWN)       || 
            (type == THING_TYPE_DROP_BOLT_UP)         || 
            (type == THING_TYPE_ANTI_BOLT_UP)         || 
            (type == THING_TYPE_ANTI_BOLT_DOWN)       || 
            (type == THING_TYPE_PLATFORM))
  {
    t->hx = 8 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 8 * SUBPIXEL_MANTISSA_FULL;
  }
  else if (type == THING_TYPE_THROWN_MARBLE)
  {
    t->hx = 8 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 8 * SUBPIXEL_MANTISSA_FULL;
  }
  else if ( (type == THING_TYPE_BUBBLE_HORIZONTAL)  || 
            (type == THING_TYPE_BUBBLE_VERTICAL)    || 
            (type == THING_TYPE_BUBBLE_STRESSED))
  {
    t->hx = 7 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 7 * SUBPIXEL_MANTISSA_FULL;
  }
  else
  {
    t->hx = 8 * SUBPIXEL_MANTISSA_FULL;
    t->hy = 8 * SUBPIXEL_MANTISSA_FULL;
  }

  t->flag = THING_COLLISION_FLAG_NONE;

  t->pos_x = pos_x;
  t->pos_y = pos_y;
  t->vel_x = vel_x;
  t->vel_y = vel_y;

  t->timer_offset = timer_offset;

  /* add this thing to the grid */
  if ((type != THING_TYPE_HELD_MARBLE)            && 
      (type != THING_TYPE_HELD_CRITTER_WALKER_1)  && 
      (type != THING_TYPE_HELD_CRITTER_WALKER_2)  && 
      (type != THING_TYPE_HELD_CRITTER_FLYER_1)   && 
      (type != THING_TYPE_HELD_CRITTER_FLYER_2)   && 
      (type != THING_TYPE_BUBBLE_POPPED))
  {
    grid_add_thing(insert_index);
  }

  return 0;
}

/*******************************************************************************
** world_despawn_thing()
*******************************************************************************/
short int world_despawn_thing(int index)
{
  thing* t;

  if ((index < 0) || (index >= WORLD_MAX_THINGS))
    return 0;

  t = &G_world_all_things[index];

  if (t->type == THING_TYPE_NONE)
    return 0;

  /* remove this thing from the grid */
  if ((t->type != THING_TYPE_HELD_MARBLE)            && 
      (t->type != THING_TYPE_HELD_CRITTER_WALKER_1)  && 
      (t->type != THING_TYPE_HELD_CRITTER_WALKER_2)  && 
      (t->type != THING_TYPE_HELD_CRITTER_FLYER_1)   && 
      (t->type != THING_TYPE_HELD_CRITTER_FLYER_2)   && 
      (t->type != THING_TYPE_BUBBLE_POPPED))
  {
    grid_remove_thing(index);
  }

  /* reset this thing */
  t->type   = THING_TYPE_NONE;
  t->color  = COLOR_NONE;
  t->state  = STATE_NONE;
  t->orient = THING_ORIENT_NORMAL;

  t->flag   = THING_COLLISION_FLAG_NONE;

  t->pos_x  = 0;
  t->pos_y  = 0;

  t->vel_x  = 0;
  t->vel_y  = 0;

  t->hx     = 0;
  t->hy     = 0;

  t->timer_offset = 0;

  return 0;
}

/*******************************************************************************
** world_load_things_from_tilemap()
*******************************************************************************/
short int world_load_things_from_tilemap()
{
  int m;
  int n;

  int type;
  int color;
  int state;
  int orient;

  int pos_x;
  int pos_y;

  int vel_x;
  int vel_y;

  unsigned int timer_offset;

  unsigned char* room_tilemap;

  /* set room tilemap */
  if ((G_current_room >= 0) && (G_current_room < LEVEL_NUM_ROOMS))
    room_tilemap = &G_room_tilemaps[G_current_room][0];
  else
    room_tilemap = &G_room_tilemaps[0][0];

  /* reset world */
  world_reset();

  /* reset globals */
  G_carrots_remaining = 0;

  G_bunny_action = BUNNY_ACTION_STANDING;
  G_bunny_facing = BUNNY_FACING_RIGHT;
  G_bunny_gravity = BUNNY_GRAVITY_DOWN;

  G_marble_gravity = MARBLE_GRAVITY_DOWN;

  /* load room tilemap to world */
  for (n = 0; n < LEVEL_ROOM_HEIGHT; n++)
  {
    for (m = 0; m < LEVEL_ROOM_WIDTH; m++)
    {
      /* initialize type, color, and orientation */
      type = THING_TYPE_NONE;
      color = COLOR_NONE;
      state = STATE_NONE;
      orient = THING_ORIENT_NORMAL;

      /* determine position */
      pos_x = (LEVEL_ROOM_TILE_SIZE * m + LEVEL_ROOM_TILE_SIZE_HALF) * SUBPIXEL_MANTISSA_FULL;
      pos_y = (LEVEL_ROOM_TILE_SIZE * n + LEVEL_ROOM_TILE_SIZE_HALF) * SUBPIXEL_MANTISSA_FULL;

      /* initialize velocity */
      vel_x = 0;
      vel_y = 0;

      /* determine thing type and starting velocity */
      if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_PLAYER_START)
        type = THING_TYPE_BUNNY;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CARROT)
        type = THING_TYPE_CARROT;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BOOK)
        type = THING_TYPE_BOOK;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSIC_NOTES_ORANGE)
      {
        type = THING_TYPE_MUSIC_NOTES;
        color = COLOR_ORANGE;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSIC_NOTES_PINK)
      {
        type = THING_TYPE_MUSIC_NOTES;
        color = COLOR_PINK;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSIC_NOTES_BLUE)
      {
        type = THING_TYPE_MUSIC_NOTES;
        color = COLOR_BLUE;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSIC_NOTES_GREEN)
      {
        type = THING_TYPE_MUSIC_NOTES;
        color = COLOR_GREEN;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_METAL)
      {
        type = THING_TYPE_GEM;
        color = COLOR_METAL;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_ORANGE)
      {
        type = THING_TYPE_GEM;
        color = COLOR_ORANGE;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_PINK)
      {
        type = THING_TYPE_GEM;
        color = COLOR_PINK;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_BLUE)
      {
        type = THING_TYPE_GEM;
        color = COLOR_BLUE;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_GREEN)
      {
        type = THING_TYPE_GEM;
        color = COLOR_GREEN;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_WILD)
      {
        type = THING_TYPE_GEM;
        color = COLOR_WILD;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MAGNET_DOWN)
        type = THING_TYPE_MAGNET_DOWN;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MAGNET_RIGHT)
        type = THING_TYPE_MAGNET_RIGHT;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MAGNET_LEFT)
        type = THING_TYPE_MAGNET_LEFT;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CRITTER_WALKER_1_RIGHT)
      {
        type = THING_TYPE_CRITTER_WALKER_1;
        orient = THING_ORIENT_NORMAL;

        vel_x = THING_CRITTER_VEL;
        vel_y = 0;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CRITTER_WALKER_1_LEFT)
      {
        type = THING_TYPE_CRITTER_WALKER_1;
        orient = THING_ORIENT_FLIP_HORI;

        vel_x = -THING_CRITTER_VEL;
        vel_y = 0;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CRITTER_WALKER_2_RIGHT)
      {
        type = THING_TYPE_CRITTER_WALKER_2;
        orient = THING_ORIENT_NORMAL;

        vel_x = THING_CRITTER_VEL;
        vel_y = 0;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CRITTER_WALKER_2_LEFT)
      {
        type = THING_TYPE_CRITTER_WALKER_2;
        orient = THING_ORIENT_FLIP_HORI;

        vel_x = -THING_CRITTER_VEL;
        vel_y = 0;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CRITTER_FLYER_1_RIGHT)
      {
        type = THING_TYPE_CRITTER_FLYER_1;
        orient = THING_ORIENT_NORMAL;

        vel_x = THING_CRITTER_VEL;
        vel_y = 0;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CRITTER_FLYER_1_LEFT)
      {
        type = THING_TYPE_CRITTER_FLYER_1;
        orient = THING_ORIENT_FLIP_HORI;

        vel_x = -THING_CRITTER_VEL;
        vel_y = 0;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CRITTER_FLYER_2_UP)
      {
        type = THING_TYPE_CRITTER_FLYER_2;
        orient = THING_ORIENT_FLIP_HORI;

        vel_x = 0;
        vel_y = -THING_CRITTER_VEL;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CRITTER_FLYER_2_DOWN)
      {
        type = THING_TYPE_CRITTER_FLYER_2;
        orient = THING_ORIENT_NORMAL;

        vel_x = 0;
        vel_y = THING_CRITTER_VEL;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BASS_KNOB)
        type = THING_TYPE_BASS_KNOB;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_TREBLE_KNOB)
        type = THING_TYPE_TREBLE_KNOB;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ROCKET_BLOCK_RIGHT)
        type = THING_TYPE_ROCKET_BLOCK_RIGHT;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ROCKET_BLOCK_LEFT)
        type = THING_TYPE_ROCKET_BLOCK_LEFT;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ELEVATOR_HORI_RIGHT)
        type = THING_TYPE_ELEVATOR_HORI_RIGHT;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ELEVATOR_HORI_LEFT)
        type = THING_TYPE_ELEVATOR_HORI_LEFT;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ELEVATOR_VERT_UP)
        type = THING_TYPE_ELEVATOR_VERT_UP;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ELEVATOR_VERT_DOWN)
        type = THING_TYPE_ELEVATOR_VERT_DOWN;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_DROP_BOLT_DOWN)
        type = THING_TYPE_DROP_BOLT_DOWN;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_DROP_BOLT_UP)
        type = THING_TYPE_DROP_BOLT_UP;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ANTI_BOLT_UP)
        type = THING_TYPE_ANTI_BOLT_UP;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ANTI_BOLT_DOWN)
        type = THING_TYPE_ANTI_BOLT_DOWN;
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_KEY_ORANGE)
      {
        type = THING_TYPE_KEY;
        color = COLOR_ORANGE;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_KEY_PINK)
      {
        type = THING_TYPE_KEY;
        color = COLOR_PINK;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_KEY_BLUE)
      {
        type = THING_TYPE_KEY;
        color = COLOR_BLUE;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_KEY_GREEN)
      {
        type = THING_TYPE_KEY;
        color = COLOR_GREEN;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSHROOM_ORANGE)
      {
        type = THING_TYPE_MUSHROOM;
        color = COLOR_ORANGE;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSHROOM_PINK)
      {
        type = THING_TYPE_MUSHROOM;
        color = COLOR_PINK;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSHROOM_BLUE)
      {
        type = THING_TYPE_MUSHROOM;
        color = COLOR_BLUE;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSHROOM_GREEN)
      {
        type = THING_TYPE_MUSHROOM;
        color = COLOR_GREEN;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_PLATFORM_RIGHT)
      {
        type = THING_TYPE_PLATFORM;
        vel_x = THING_PLATFORM_VEL;
        vel_y = 0;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_PLATFORM_LEFT)
      {
        type = THING_TYPE_PLATFORM;
        vel_x = -THING_PLATFORM_VEL;
        vel_y = 0;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_PLATFORM_UP)
      {
        type = THING_TYPE_PLATFORM;
        vel_x = 0;
        vel_y = -THING_PLATFORM_VEL;
      }
      else if (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_PLATFORM_DOWN)
      {
        type = THING_TYPE_PLATFORM;
        vel_x = 0;
        vel_y = THING_PLATFORM_VEL;
      }
      else
        continue;

      /* set states for platforms etc */
      if ((room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BASS_KNOB)           || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_TREBLE_KNOB)         || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ROCKET_BLOCK_RIGHT)  || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ROCKET_BLOCK_LEFT)   || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_DROP_BOLT_DOWN)      || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_DROP_BOLT_UP)        || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ANTI_BOLT_UP)        || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_ANTI_BOLT_DOWN))
      {
        state = STATE_RETRACTING_STOPPED;
      }

      /* set timer offsets */
      if ((room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_CARROT)              || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_BOOK)                || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSIC_NOTES_ORANGE)  || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSIC_NOTES_PINK)    || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSIC_NOTES_BLUE)    || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSIC_NOTES_GREEN)   || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_METAL)           || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_ORANGE)          || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_PINK)            || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_BLUE)            || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_GREEN)           || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_GEM_WILD)            || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MAGNET_DOWN)         || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MAGNET_RIGHT)        || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MAGNET_LEFT)         || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_KEY_ORANGE)          || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_KEY_PINK)            || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_KEY_BLUE)            || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_KEY_GREEN)           || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSHROOM_ORANGE)     || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSHROOM_PINK)       || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSHROOM_BLUE)       || 
          (room_tilemap[(n * LEVEL_ROOM_WIDTH) + m] == LEVEL_ROOM_TILE_MUSHROOM_GREEN))
      {
        timer_offset = 240 - (((n + m) % ANIM_PICKUP_NUM_FRAMES) * ANIM_PICKUP_FRAME_LENGTH);
      }
      else
      {
        timer_offset = 0;
      }

      /* spawn thing */
      world_spawn_thing(type, color, state, orient, 
                        pos_x, pos_y, vel_x, vel_y, timer_offset);

      /* if this is a carrot, increment carrots remaining count */
      if (type == THING_TYPE_CARROT)
        G_carrots_remaining += 1;
    }
  }

  return 0;
}

