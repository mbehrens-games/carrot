/*******************************************************************************
** logic.c (grid and world update functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "animate.h"
#include "bunny.h"
#include "color.h"
#include "doremi.h"
#include "global.h"
#include "grid.h"
#include "lifetime.h"
#include "state.h"
#include "thing.h"
#include "world.h"

/*******************************************************************************
** logic_marbles_apply_gravity()
*******************************************************************************/
short int logic_marbles_apply_gravity()
{
  int m;
  int n;

  int outer_bound;
  int inner_bound;

  /* the size of this array is the larger of GRID_WIDTH and GRID_HEIGHT */
  grid_box* b[GRID_WIDTH];

  int start_index;
  int marble_color;

  int indices[2];

  /* set bounds based on gravity */
  if ((G_marble_gravity == MARBLE_GRAVITY_DOWN) || 
      (G_marble_gravity == MARBLE_GRAVITY_UP))
  {
    outer_bound = GRID_WIDTH;
    inner_bound = GRID_HEIGHT;
  }
  else if ( (G_marble_gravity == MARBLE_GRAVITY_RIGHT) || 
            (G_marble_gravity == MARBLE_GRAVITY_LEFT))
  {
    outer_bound = GRID_HEIGHT;
    inner_bound = GRID_WIDTH;
  }
  else
    return 0;

  /* if the gravity is vertical, we check columns left to right */
  /* if the gravity is horizontal, we check rows top to bottom  */
  for (m = 0; m < outer_bound; m++)
  {
    /* populate grid box pointers based on gravity */
    if (G_marble_gravity == MARBLE_GRAVITY_DOWN)
    {
      for (n = 0; n < inner_bound; n++)
        b[inner_bound - 1 - n] = &G_collision_grid[(n * GRID_WIDTH) + m];
    }
    else if (G_marble_gravity == MARBLE_GRAVITY_UP)
    {
      for (n = 0; n < inner_bound; n++)
        b[n] = &G_collision_grid[(n * GRID_WIDTH) + m];
    }
    else if (G_marble_gravity == MARBLE_GRAVITY_RIGHT)
    {
      for (n = 0; n < inner_bound; n++)
        b[inner_bound - 1 - n] = &G_collision_grid[(m * GRID_WIDTH) + n];
    }
    else if (G_marble_gravity == MARBLE_GRAVITY_LEFT)
    {
      for (n = 0; n < inner_bound; n++)
        b[n] = &G_collision_grid[(m * GRID_WIDTH) + n];
    }
    else
      return 0;

    /* determine start index */
    start_index = -1;

    for (n = 0; n < inner_bound; n++)
    {
      if ((b[n]->front_object != GRID_OBJECT_MARBLE) || 
          (b[n]->front_state != STATE_NONE))
      {
        start_index = n;
        break;
      }
    }

    /* if no start index found, this entire   */
    /* column (or row) is filled with marbles */
    /* so, just shift all marbles by one      */
    if (start_index == -1)
    {
      marble_color = b[0]->front_color;

      for (n = 0; n < inner_bound - 1; n++)
        b[n]->front_color = b[n + 1]->front_color;

      b[inner_bound - 1]->front_color = marble_color;

      /* play marble drop sound */
      doremi_play_sfx(SFX_INDEX_MARBLE_DROP);
    }
    /* otherwise, begin at the start index  */
    /* if a marble can drop to the next     */
    /* space, drop it                       */
    else
    {
      for (n = 0; n < inner_bound - 1; n++)
      {
        indices[0] = (start_index + n) % inner_bound;
        indices[1] = (indices[0] + 1) % inner_bound;

        if ((b[indices[0]]->front_object == GRID_OBJECT_NONE) && 
            (b[indices[0]]->num_things == 0))
        {
          if ((b[indices[1]]->front_object == GRID_OBJECT_MARBLE) && 
              (b[indices[1]]->front_state == STATE_NONE))
          {
            b[indices[0]]->front_object = b[indices[1]]->front_object;
            b[indices[0]]->front_color = b[indices[1]]->front_color;
            b[indices[0]]->front_state = b[indices[1]]->front_state;
            b[indices[0]]->front_timer_offset = b[indices[1]]->front_timer_offset;

            b[indices[1]]->front_object = GRID_OBJECT_NONE;
            b[indices[1]]->front_color = COLOR_NONE;
            b[indices[1]]->front_state = STATE_NONE;
            b[indices[1]]->front_timer_offset = 0;

            /* play marble drop sound */
            doremi_play_sfx(SFX_INDEX_MARBLE_DROP);
          }
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_marbles_clear_completed_flashes_and_changes()
*******************************************************************************/
short int logic_marbles_clear_completed_flashes_and_changes()
{
  int m;
  int n;

  grid_box* b;

  int k;

  thing* t;

  unsigned int adjusted_timer_count;

  /* check for completed flashes and changes in the grid */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b = &G_collision_grid[(n * GRID_WIDTH) + m];

      if (b->front_object == GRID_OBJECT_MARBLE)
      {
        /* determine adjusted timer count */
        adjusted_timer_count = (G_timer_count + b->front_timer_offset) % 240;

        /* clear completed flash or change if necessary */
        if ((b->front_state == STATE_MARBLE_FLASHING) && 
            (adjusted_timer_count >= LIFETIME_MARBLE_FLASHING))
        {
          b->front_object = GRID_OBJECT_NONE;
          b->front_color = COLOR_NONE;
          b->front_state = STATE_NONE;
          b->front_timer_offset = 0;
        }
        else if ( (b->front_state == STATE_MARBLE_CHANGING) && 
                  (adjusted_timer_count >= LIFETIME_MARBLE_CHANGING))
        {
          if (b->front_color == COLOR_METAL)
            b->front_color = COLOR_WILD;
          else
            b->front_color = COLOR_METAL;

          b->front_state = STATE_NONE;
          b->front_timer_offset = 0;
        }
      }
    }
  }

  /* check for completed flash or change with the held thing */
  t = &G_world_all_things[WORLD_HELD_THING_INDEX];

  if (t->type == THING_TYPE_HELD_MARBLE)
  {
    /* determine adjusted timer count */
    adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

    /* clear completed flash or change if necessary */
    if ((t->state == STATE_MARBLE_FLASHING) && 
        (adjusted_timer_count >= LIFETIME_MARBLE_FLASHING))
    {
      world_despawn_thing(WORLD_HELD_THING_INDEX);
    }
    else if ( (t->state == STATE_MARBLE_CHANGING) && 
              (adjusted_timer_count >= LIFETIME_MARBLE_CHANGING))
    {
      if (t->color == COLOR_METAL)
        t->color = COLOR_WILD;
      else
        t->color = COLOR_METAL;

      t->state = STATE_NONE;
      t->timer_offset = 0;
    }
  }

  /* check for completed flash or change with thrown things */
  for ( k = WORLD_THROWN_THINGS_START_INDEX; 
        k < WORLD_THROWN_THINGS_END_INDEX; 
        k++)
  {
    t = &G_world_all_things[k];

    if (t->type == THING_TYPE_THROWN_MARBLE)
    {
      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

      /* clear completed flash or change if necessary */
      if ((t->state == STATE_MARBLE_FLASHING) && 
          (adjusted_timer_count >= LIFETIME_MARBLE_FLASHING))
      {
        world_despawn_thing(k);
      }
      else if ( (t->state == STATE_MARBLE_CHANGING) && 
                (adjusted_timer_count >= LIFETIME_MARBLE_CHANGING))
      {
        if (t->color == COLOR_METAL)
          t->color = COLOR_WILD;
        else
          t->color = COLOR_METAL;

        t->state = STATE_NONE;
        t->timer_offset = 0;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_marbles_check_for_matches()
*******************************************************************************/
short int logic_marbles_check_for_matches()
{
  int m;
  int n;

  grid_box* b[3];

  int k;

  int adj_x[5];
  int adj_y[5];

  int indices[8][2];

  /* check for marble matches */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b[0] = &G_collision_grid[(n * GRID_WIDTH) + m];

      if (b[0]->front_object != GRID_OBJECT_MARBLE)
        continue;

      if (b[0]->front_state != STATE_NONE)
        continue;

      /* compute adjacent grid coordinates      */
      /* note that index 2 is the center (m, n) */
      for (k = 0; k < 5; k++)
      {
        adj_x[k] = (m - 2 + k + GRID_WIDTH) % GRID_WIDTH;
        adj_y[k] = (n - 2 + k + GRID_HEIGHT) % GRID_HEIGHT;
      }

      /* compute indices */
      /* direction: right */
      indices[0][0] = (adj_y[2] * GRID_WIDTH) + adj_x[3];
      indices[0][1] = (adj_y[2] * GRID_WIDTH) + adj_x[4];

      /* direction: up right */
      indices[1][0] = (adj_y[1] * GRID_WIDTH) + adj_x[3];
      indices[1][1] = (adj_y[0] * GRID_WIDTH) + adj_x[4];

      /* direction: up */
      indices[2][0] = (adj_y[1] * GRID_WIDTH) + adj_x[2];
      indices[2][1] = (adj_y[0] * GRID_WIDTH) + adj_x[2];

      /* direction: up left */
      indices[3][0] = (adj_y[1] * GRID_WIDTH) + adj_x[1];
      indices[3][1] = (adj_y[0] * GRID_WIDTH) + adj_x[0];

      /* direction: left */
      indices[4][0] = (adj_y[2] * GRID_WIDTH) + adj_x[1];
      indices[4][1] = (adj_y[2] * GRID_WIDTH) + adj_x[0];

      /* direction: down left */
      indices[5][0] = (adj_y[3] * GRID_WIDTH) + adj_x[1];
      indices[5][1] = (adj_y[4] * GRID_WIDTH) + adj_x[0];

      /* direction: down */
      indices[6][0] = (adj_y[3] * GRID_WIDTH) + adj_x[2];
      indices[6][1] = (adj_y[4] * GRID_WIDTH) + adj_x[2];

      /* direction: down right */
      indices[7][0] = (adj_y[3] * GRID_WIDTH) + adj_x[3];
      indices[7][1] = (adj_y[4] * GRID_WIDTH) + adj_x[4];

      /* check for match in each direction */
      for (k = 0; k < 8; k++)
      {
        /* set up grid box pointers */
        b[1] = &G_collision_grid[indices[k][0]];
        b[2] = &G_collision_grid[indices[k][1]];

        /* make sure these are all marbles */
        if ((b[0]->front_object != GRID_OBJECT_MARBLE) || 
            (b[1]->front_object != GRID_OBJECT_MARBLE) || 
            (b[2]->front_object != GRID_OBJECT_MARBLE))
        {
          continue;
        }

        /* make sure all marbles have a cleared */
        /* state (not flashing, changing, etc)  */
        if ((b[0]->front_state != STATE_NONE) && 
            (b[0]->front_state != STATE_MARBLE_MATCHED))
        {
          continue;
        }

        if ((b[1]->front_state != STATE_NONE) && 
            (b[1]->front_state != STATE_MARBLE_MATCHED))
        {
          continue;
        }

        if ((b[2]->front_state != STATE_NONE) && 
            (b[2]->front_state != STATE_MARBLE_MATCHED))
        {
          continue;
        }

        /* all wildcard match check */
        if ((b[0]->front_color == COLOR_WILD) && 
            (b[1]->front_color == COLOR_WILD) && 
            (b[2]->front_color == COLOR_WILD))
        {
          b[0]->front_state = STATE_MARBLE_MATCHED;
          b[1]->front_state = STATE_MARBLE_MATCHED;
          b[2]->front_state = STATE_MARBLE_MATCHED;

          /* play marble flashing sound */
          doremi_play_sfx(SFX_INDEX_MARBLE_FLASHING);
        }
        /* orange match check */
        else if ( ((b[0]->front_color == COLOR_ORANGE) || (b[0]->front_color == COLOR_WILD)) && 
                  ((b[1]->front_color == COLOR_ORANGE) || (b[1]->front_color == COLOR_WILD)) && 
                  ((b[2]->front_color == COLOR_ORANGE) || (b[2]->front_color == COLOR_WILD)))
        {
          b[0]->front_state = STATE_MARBLE_MATCHED;
          b[1]->front_state = STATE_MARBLE_MATCHED;
          b[2]->front_state = STATE_MARBLE_MATCHED;

          /* play marble flashing sound */
          doremi_play_sfx(SFX_INDEX_MARBLE_FLASHING);
        }
        /* pink match check */
        else if ( ((b[0]->front_color == COLOR_PINK) || (b[0]->front_color == COLOR_WILD)) && 
                  ((b[1]->front_color == COLOR_PINK) || (b[1]->front_color == COLOR_WILD)) && 
                  ((b[2]->front_color == COLOR_PINK) || (b[2]->front_color == COLOR_WILD)))
        {
          b[0]->front_state = STATE_MARBLE_MATCHED;
          b[1]->front_state = STATE_MARBLE_MATCHED;
          b[2]->front_state = STATE_MARBLE_MATCHED;

          /* play marble flashing sound */
          doremi_play_sfx(SFX_INDEX_MARBLE_FLASHING);
        }
        /* blue match check */
        else if ( ((b[0]->front_color == COLOR_BLUE) || (b[0]->front_color == COLOR_WILD)) && 
                  ((b[1]->front_color == COLOR_BLUE) || (b[1]->front_color == COLOR_WILD)) && 
                  ((b[2]->front_color == COLOR_BLUE) || (b[2]->front_color == COLOR_WILD)))
        {
          b[0]->front_state = STATE_MARBLE_MATCHED;
          b[1]->front_state = STATE_MARBLE_MATCHED;
          b[2]->front_state = STATE_MARBLE_MATCHED;

          /* play marble flashing sound */
          doremi_play_sfx(SFX_INDEX_MARBLE_FLASHING);
        }
        /* green match check */
        else if ( ((b[0]->front_color == COLOR_GREEN) || (b[0]->front_color == COLOR_WILD)) && 
                  ((b[1]->front_color == COLOR_GREEN) || (b[1]->front_color == COLOR_WILD)) && 
                  ((b[2]->front_color == COLOR_GREEN) || (b[2]->front_color == COLOR_WILD)))
        {
          b[0]->front_state = STATE_MARBLE_MATCHED;
          b[1]->front_state = STATE_MARBLE_MATCHED;
          b[2]->front_state = STATE_MARBLE_MATCHED;

          /* play marble flashing sound */
          doremi_play_sfx(SFX_INDEX_MARBLE_FLASHING);
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_marbles_activate_flashes_and_changes()
*******************************************************************************/
short int logic_marbles_activate_flashes_and_changes()
{
  int m;
  int n;

  grid_box* b;

  int k;

  thing* t;

  /* check for matched or ready marbles in the grid */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b = &G_collision_grid[(n * GRID_WIDTH) + m];

      if (b->front_object == GRID_OBJECT_MARBLE)
      {
        if (b->front_state == STATE_MARBLE_MATCHED)
        {
          b->front_state = STATE_MARBLE_FLASHING;
          b->front_timer_offset = 240 - G_timer_count;
        }
        else if (b->front_state == STATE_MARBLE_READY_TO_CHANGE)
        {
          b->front_state = STATE_MARBLE_CHANGING;
          b->front_timer_offset = 240 - G_timer_count;
        }
      }
    }
  }

  /* check for matched or ready marbles with the held thing */
  t = &G_world_all_things[WORLD_HELD_THING_INDEX];

  if (t->type == THING_TYPE_HELD_MARBLE)
  {
    if (t->state == STATE_MARBLE_MATCHED)
    {
      t->state = STATE_MARBLE_FLASHING;
      t->timer_offset = 240 - G_timer_count;
    }
    else if (t->state == STATE_MARBLE_READY_TO_CHANGE)
    {
      t->state = STATE_MARBLE_CHANGING;
      t->timer_offset = 240 - G_timer_count;
    }
  }

  /* check for matched or ready marbles with thrown things */
  for ( k = WORLD_THROWN_THINGS_START_INDEX; 
        k < WORLD_THROWN_THINGS_END_INDEX; 
        k++)
  {
    t = &G_world_all_things[k];

    if (t->type == THING_TYPE_THROWN_MARBLE)
    {
      if (t->state == STATE_MARBLE_MATCHED)
      {
        t->state = STATE_MARBLE_FLASHING;
        t->timer_offset = 240 - G_timer_count;
      }
      else if (t->state == STATE_MARBLE_READY_TO_CHANGE)
      {
        t->state = STATE_MARBLE_CHANGING;
        t->timer_offset = 240 - G_timer_count;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_complete_grid_objects_animations()
*******************************************************************************/
short int logic_complete_grid_objects_animations()
{
  int m;
  int n;

  grid_box* b;

  unsigned int adjusted_timer_count;

  /* check for completed animations */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b = &G_collision_grid[(n * GRID_WIDTH) + m];

      if ((b->back_object == GRID_OBJECT_DOOR_BLOCK) && 
          (b->back_state == STATE_DOOR_BLOCK_OPENING))
      {
        /* determine adjusted timer count */
        adjusted_timer_count = (G_timer_count + b->back_timer_offset) % 240;

        /* clear opened door if necessary */
        if (adjusted_timer_count >= LIFETIME_DOOR_OPENING)
        {
          b->back_object = GRID_OBJECT_NONE;
          b->back_color = COLOR_NONE;
          b->back_state = STATE_NONE;
          b->back_timer_offset = 0;
        }
      }
      else if ( (b->front_object == GRID_OBJECT_VISIBLE_BLOCK) && 
                (b->front_state == STATE_VISIBLE_BLOCK_APPEARING))
      {
        /* determine adjusted timer count */
        adjusted_timer_count = (G_timer_count + b->front_timer_offset) % 240;

        /* make appearing block remain visible if necessary */
        if (adjusted_timer_count >= LIFETIME_INVIS_APPEARING)
        {
          b->front_state = STATE_NONE;
          b->front_timer_offset = 0;
        }
      }
      else if (b->front_object == GRID_OBJECT_HEART_BLOCK)
      {
        if ((b->front_state == STATE_HEART_BLOCK_FLASHING_TOP) || 
            (b->front_state == STATE_HEART_BLOCK_FLASHING_BOTTOM))
        {
          /* determine adjusted timer count */
          adjusted_timer_count = (G_timer_count + b->front_timer_offset) % 240;

          /* make heart block stop flashing if necessary */
          if (adjusted_timer_count >= LIFETIME_HEART_BLOCK_FLASHING)
          {
            b->front_state = STATE_NONE;
            b->front_timer_offset = 0;
          }
        }
      }
      else if (b->front_object == GRID_OBJECT_DIAMOND_BLOCK)
      {
        if ((b->front_state == STATE_DIAMOND_BLOCK_FLASHING_TOP) || 
            (b->front_state == STATE_DIAMOND_BLOCK_FLASHING_BOTTOM))
        {
          /* determine adjusted timer count */
          adjusted_timer_count = (G_timer_count + b->front_timer_offset) % 240;

          /* make diamond block stop flashing if necessary */
          if (adjusted_timer_count >= LIFETIME_DIAMOND_BLOCK_FLASHING)
          {
            b->front_state = STATE_NONE;
            b->front_timer_offset = 0;
          }
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_doors_and_invisible_blocks_activate()
*******************************************************************************/
short int logic_doors_and_invisible_blocks_activate()
{
  int m;
  int n;

  grid_box* b;

  /* check for ready doors or invisible blocks */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b = &G_collision_grid[(n * GRID_WIDTH) + m];

      if ((b->front_object == GRID_OBJECT_DOOR_BLOCK) && 
          (b->front_state == STATE_DOOR_BLOCK_READY_TO_OPEN))
      {
        b->back_object = GRID_OBJECT_DOOR_BLOCK;
        b->back_color = b->front_color;
        b->back_state = STATE_DOOR_BLOCK_OPENING;
        b->back_timer_offset = 240 - G_timer_count;

        b->front_object = GRID_OBJECT_NONE;
        b->front_color = COLOR_NONE;
        b->front_state = STATE_NONE;
        b->front_timer_offset = 0;
      }
      else if ( (b->back_object == GRID_OBJECT_INVISIBLE_BLOCK)           && 
                (b->back_state == STATE_INVISIBLE_BLOCK_READY_TO_APPEAR)  && 
                (b->front_object == GRID_OBJECT_NONE)                     && 
                (b->num_things == 0))
      {
        b->front_object = GRID_OBJECT_VISIBLE_BLOCK;
        b->front_color = b->back_color;
        b->front_state = STATE_VISIBLE_BLOCK_APPEARING;
        b->front_timer_offset = 240 - G_timer_count;

        b->back_object = GRID_OBJECT_NONE;
        b->back_color = COLOR_NONE;
        b->back_state = STATE_NONE;
        b->back_timer_offset = 0;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_pickup_spawners_update()
*******************************************************************************/
short int logic_pickup_spawners_update()
{
  int m;
  int n;

  grid_box* b;

  int pos_x;
  int pos_y;

  unsigned int adjusted_timer_count;
  unsigned int pickup_timer_offset;

  /* update pickup spawners */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b = &G_collision_grid[(n * GRID_WIDTH) + m];

      if (b->back_object == GRID_OBJECT_NONE)
        continue;

      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + b->back_timer_offset) % 240;

      /* check pickup spawners */
      if ((b->back_object == GRID_OBJECT_BOOK_SPAWNER)  && 
          (b->front_object == GRID_OBJECT_NONE)         && 
          (b->num_things == 0)                          && 
          (adjusted_timer_count >= 210))
      {
        pos_x = (GRID_BOX_SIZE * m + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
        pos_y = (GRID_BOX_SIZE * n + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

        pickup_timer_offset = 240 - (((n + m) % ANIM_PICKUP_NUM_FRAMES) * ANIM_PICKUP_FRAME_LENGTH);

        world_spawn_thing(THING_TYPE_BOOK, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                          pos_x, pos_y, 0, 0, pickup_timer_offset);
      }
      else if ( (b->back_object == GRID_OBJECT_MAGNET_DOWN_SPAWNER) && 
                (b->front_object == GRID_OBJECT_NONE)               && 
                (b->num_things == 0)                                && 
                (adjusted_timer_count >= 210))
      {
        pos_x = (GRID_BOX_SIZE * m + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
        pos_y = (GRID_BOX_SIZE * n + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

        pickup_timer_offset = 240 - (((n + m) % ANIM_PICKUP_NUM_FRAMES) * ANIM_PICKUP_FRAME_LENGTH);

        if (G_marble_gravity == MARBLE_GRAVITY_UP)
        {
          world_spawn_thing(THING_TYPE_MAGNET_UP, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                            pos_x, pos_y, 0, 0, pickup_timer_offset);
        }
        else
        {
          world_spawn_thing(THING_TYPE_MAGNET_DOWN, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                            pos_x, pos_y, 0, 0, pickup_timer_offset);
        }
      }
      else if ( (b->back_object == GRID_OBJECT_MAGNET_RIGHT_SPAWNER)  && 
                (b->front_object == GRID_OBJECT_NONE)                 && 
                (b->num_things == 0)                                  && 
                (adjusted_timer_count >= 210))
      {
        pos_x = (GRID_BOX_SIZE * m + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
        pos_y = (GRID_BOX_SIZE * n + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

        pickup_timer_offset = 240 - (((n + m) % ANIM_PICKUP_NUM_FRAMES) * ANIM_PICKUP_FRAME_LENGTH);

        if (G_marble_gravity == MARBLE_GRAVITY_LEFT)
        {
          world_spawn_thing(THING_TYPE_MAGNET_LEFT, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                            pos_x, pos_y, 0, 0, pickup_timer_offset);
        }
        else
        {
          world_spawn_thing(THING_TYPE_MAGNET_RIGHT, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                            pos_x, pos_y, 0, 0, pickup_timer_offset);
        }
      }
      else if ( (b->back_object == GRID_OBJECT_MAGNET_LEFT_SPAWNER) && 
                (b->front_object == GRID_OBJECT_NONE)               && 
                (b->num_things == 0)                                && 
                (adjusted_timer_count >= 210))
      {
        pos_x = (GRID_BOX_SIZE * m + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
        pos_y = (GRID_BOX_SIZE * n + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

        pickup_timer_offset = 240 - (((n + m) % ANIM_PICKUP_NUM_FRAMES) * ANIM_PICKUP_FRAME_LENGTH);

        if (G_marble_gravity == MARBLE_GRAVITY_RIGHT)
        {
          world_spawn_thing(THING_TYPE_MAGNET_RIGHT, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                            pos_x, pos_y, 0, 0, pickup_timer_offset);
        }
        else
        {
          world_spawn_thing(THING_TYPE_MAGNET_LEFT, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                            pos_x, pos_y, 0, 0, pickup_timer_offset);
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_bubble_pots_update()
*******************************************************************************/
short int logic_bubble_pots_update()
{
  int m;
  int n;

  grid_box* b[2];

  int spawn_box_x;
  int spawn_box_y;

  int pos_x;
  int pos_y;

  unsigned int adjusted_timer_count;

  /* update bubble pots */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b[0] = &G_collision_grid[(n * GRID_WIDTH) + m];

      if (b[0]->front_object == GRID_OBJECT_NONE)
        continue;

      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + b[0]->front_timer_offset) % 240;

      /* check bubble pots */
      if ((b[0]->front_object == GRID_OBJECT_BUBBLE_POT_UP) && 
          (adjusted_timer_count % 120 == 0))
      {
        spawn_box_x = m;
        spawn_box_y = ((n - 1) + GRID_HEIGHT) % GRID_HEIGHT;

        b[1] = &G_collision_grid[(spawn_box_y * GRID_WIDTH) + spawn_box_x];

        if ((b[1]->front_object != GRID_OBJECT_NONE) || 
            (b[1]->num_things != 0))
        {
          continue;
        }

        pos_x = (GRID_BOX_SIZE * spawn_box_x + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
        pos_y = (GRID_BOX_SIZE * spawn_box_y + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

        world_spawn_thing(THING_TYPE_BUBBLE_VERTICAL, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                          pos_x, pos_y, 0, -THING_BUBBLE_VEL, 0);

        /* play bubble appear sound */
        doremi_play_sfx(SFX_INDEX_BUBBLE_APPEAR);
      }
      else if ( (b[0]->front_object == GRID_OBJECT_BUBBLE_POT_RIGHT) && 
                (adjusted_timer_count % 120 == 0))
      {
        spawn_box_x = ((m + 1) + GRID_WIDTH) % GRID_WIDTH;
        spawn_box_y = n;

        b[1] = &G_collision_grid[(spawn_box_y * GRID_WIDTH) + spawn_box_x];

        if ((b[1]->front_object != GRID_OBJECT_NONE) || 
            (b[1]->num_things != 0))
        {
          continue;
        }

        pos_x = (GRID_BOX_SIZE * spawn_box_x + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
        pos_y = (GRID_BOX_SIZE * spawn_box_y + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

        world_spawn_thing(THING_TYPE_BUBBLE_HORIZONTAL, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                          pos_x, pos_y, THING_BUBBLE_VEL, 0, 0);

        /* play bubble appear sound */
        doremi_play_sfx(SFX_INDEX_BUBBLE_APPEAR);
      }
      else if ( (b[0]->front_object == GRID_OBJECT_BUBBLE_POT_DOWN) && 
                (adjusted_timer_count % 120 == 0))
      {
        spawn_box_x = m;
        spawn_box_y = ((n + 1) + GRID_HEIGHT) % GRID_HEIGHT;

        b[1] = &G_collision_grid[(spawn_box_y * GRID_WIDTH) + spawn_box_x];

        if ((b[1]->front_object != GRID_OBJECT_NONE) || 
            (b[1]->num_things != 0))
        {
          continue;
        }

        pos_x = (GRID_BOX_SIZE * spawn_box_x + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
        pos_y = (GRID_BOX_SIZE * spawn_box_y + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

        world_spawn_thing(THING_TYPE_BUBBLE_VERTICAL, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                          pos_x, pos_y, 0, THING_BUBBLE_VEL, 0);

        /* play bubble appear sound */
        doremi_play_sfx(SFX_INDEX_BUBBLE_APPEAR);
      }
      else if ( (b[0]->front_object == GRID_OBJECT_BUBBLE_POT_LEFT) && 
                (adjusted_timer_count % 120 == 0))
      {
        spawn_box_x = ((m - 1) + GRID_WIDTH) % GRID_WIDTH;
        spawn_box_y = n;

        b[1] = &G_collision_grid[(spawn_box_y * GRID_WIDTH) + spawn_box_x];

        if ((b[1]->front_object != GRID_OBJECT_NONE) || 
            (b[1]->num_things != 0))
        {
          continue;
        }

        pos_x = (GRID_BOX_SIZE * spawn_box_x + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
        pos_y = (GRID_BOX_SIZE * spawn_box_y + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

        world_spawn_thing(THING_TYPE_BUBBLE_HORIZONTAL, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                          pos_x, pos_y, -THING_BUBBLE_VEL, 0, 0);

        /* play bubble appear sound */
        doremi_play_sfx(SFX_INDEX_BUBBLE_APPEAR);
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_magi_blocks_update()
*******************************************************************************/
short int logic_magi_blocks_update()
{
  int m;
  int n;

  grid_box* b;

  unsigned int adjusted_timer_count;

  /* update magi blocks */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b = &G_collision_grid[(n * GRID_WIDTH) + m];

      if (b->back_object == GRID_OBJECT_NONE)
        continue;

      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + b->back_timer_offset) % 240;

      /* magi block spawners */
      if ((adjusted_timer_count % 240) / 60 == 0)
      {
        if (b->back_object == GRID_OBJECT_MAGI_SPAWNER_1_1)
        {
          if ((b->front_object == GRID_OBJECT_NONE) &&
              (b->num_things == 0))
          {
            b->front_object = GRID_OBJECT_MAGI_BLOCK_2;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = b->back_timer_offset;

            /* play magi block sound */
            if ((G_timer_count % 240) % 60 == 0)
              doremi_play_sfx(SFX_INDEX_MAGI_BLOCK_NORMAL);
          }
        }
        else if ( (b->back_object == GRID_OBJECT_MAGI_SPAWNER_1_3) || 
                  (b->back_object == GRID_OBJECT_MAGI_SPAWNER_2_2) || 
                  (b->back_object == GRID_OBJECT_MAGI_SPAWNER_3_1))
        {
          if ((b->front_object == GRID_OBJECT_NONE) &&
              (b->num_things == 0))
          {
            b->front_object = GRID_OBJECT_MAGI_BLOCK_1;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = b->back_timer_offset;

            /* play magi block sound */
            if ((G_timer_count % 240) % 60 == 0)
              doremi_play_sfx(SFX_INDEX_MAGI_BLOCK_NORMAL);
          }
        }
      }
      else if ((adjusted_timer_count % 240) / 60 == 1)
      {
        if ((b->back_object == GRID_OBJECT_MAGI_SPAWNER_1_1) || 
            (b->back_object == GRID_OBJECT_MAGI_SPAWNER_1_3))
        {
          if ((b->front_object == GRID_OBJECT_MAGI_BLOCK_1) || 
              (b->front_object == GRID_OBJECT_MAGI_BLOCK_2))
          {
            b->front_object = GRID_OBJECT_NONE;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = 0;
          }
        }
      }
      else if ((adjusted_timer_count % 240) / 60 == 2)
      {
        if (b->back_object == GRID_OBJECT_MAGI_SPAWNER_1_1)
        {
          if ((b->front_object == GRID_OBJECT_NONE) &&
              (b->num_things == 0))
          {
            b->front_object = GRID_OBJECT_MAGI_BLOCK_2;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = b->back_timer_offset;

            /* play magi block sound */
            if ((G_timer_count % 240) % 60 == 0)
              doremi_play_sfx(SFX_INDEX_MAGI_BLOCK_NORMAL);
          }
        }
        else if (b->back_object == GRID_OBJECT_MAGI_SPAWNER_2_2)
        {
          if (b->front_object == GRID_OBJECT_MAGI_BLOCK_1)
          {
            b->front_object = GRID_OBJECT_NONE;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = 0;
          }
        }
      }
      else if ((adjusted_timer_count % 240) / 60 == 3)
      {
        if ((b->back_object == GRID_OBJECT_MAGI_SPAWNER_1_1) || 
            (b->back_object == GRID_OBJECT_MAGI_SPAWNER_3_1))
        {
          if ((b->front_object == GRID_OBJECT_MAGI_BLOCK_1) || 
              (b->front_object == GRID_OBJECT_MAGI_BLOCK_2))
          {
            b->front_object = GRID_OBJECT_NONE;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = 0;
          }
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_fast_magi_blocks_update()
*******************************************************************************/
short int logic_fast_magi_blocks_update()
{
  int m;
  int n;

  grid_box* b;

  unsigned int adjusted_timer_count;

  /* update fast magi blocks */
  for (n = 0; n < GRID_HEIGHT; n++)
  {
    for (m = 0; m < GRID_WIDTH; m++)
    {
      b = &G_collision_grid[(n * GRID_WIDTH) + m];

      if (b->back_object == GRID_OBJECT_NONE)
        continue;

      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + b->back_timer_offset) % 240;

      /* fast magi block spawners */
      if ((adjusted_timer_count % 120) / 30 == 0)
      {
        if (b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_1_1)
        {
          if ((b->front_object == GRID_OBJECT_NONE) &&
              (b->num_things == 0))
          {
            b->front_object = GRID_OBJECT_FAST_MAGI_BLOCK_2;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = b->back_timer_offset;

            /* play fast magi block sound */
            if ((G_timer_count % 120) % 30 == 0)
              doremi_play_sfx(SFX_INDEX_MAGI_BLOCK_FAST);
          }
        }
        else if ( (b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_1_3)  || 
                  (b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_2_2)  || 
                  (b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_3_1))
        {
          if ((b->front_object == GRID_OBJECT_NONE) &&
              (b->num_things == 0))
          {
            b->front_object = GRID_OBJECT_FAST_MAGI_BLOCK_1;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = b->back_timer_offset;

            /* play fast magi block sound */
            if ((G_timer_count % 120) % 30 == 0)
              doremi_play_sfx(SFX_INDEX_MAGI_BLOCK_FAST);
          }
        }
      }
      else if ((adjusted_timer_count % 120) / 30 == 1)
      {
        if ((b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_1_1)  || 
            (b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_1_3))
        {
          if ((b->front_object == GRID_OBJECT_FAST_MAGI_BLOCK_1)  || 
              (b->front_object == GRID_OBJECT_FAST_MAGI_BLOCK_2))
          {
            b->front_object = GRID_OBJECT_NONE;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = 0;
          }
        }
      }
      else if ((adjusted_timer_count % 120) / 30 == 2)
      {
        if (b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_1_1)
        {
          if ((b->front_object == GRID_OBJECT_NONE) &&
              (b->num_things == 0))
          {
            b->front_object = GRID_OBJECT_FAST_MAGI_BLOCK_2;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = b->back_timer_offset;

            /* play fast magi block sound */
            if ((G_timer_count % 120) % 30 == 0)
              doremi_play_sfx(SFX_INDEX_MAGI_BLOCK_FAST);
          }
        }
        else if (b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_2_2)
        {
          if (b->front_object == GRID_OBJECT_FAST_MAGI_BLOCK_1)
          {
            b->front_object = GRID_OBJECT_NONE;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = 0;
          }
        }
      }
      else if ((adjusted_timer_count % 120) / 30 == 3)
      {
        if ((b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_1_1)  || 
            (b->back_object == GRID_OBJECT_FAST_MAGI_SPAWNER_3_1))
        {
          if ((b->front_object == GRID_OBJECT_FAST_MAGI_BLOCK_1)  || 
              (b->front_object == GRID_OBJECT_FAST_MAGI_BLOCK_2))
          {
            b->front_object = GRID_OBJECT_NONE;
            b->front_color = COLOR_NONE;
            b->front_state = STATE_NONE;
            b->front_timer_offset = 0;
          }
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_collect_pickups()
*******************************************************************************/
short int logic_collect_pickups()
{
  grid_box* b;

  thing*    t1;
  thing*    t2;

  int       m;
  int       n;

  int       k;
  int       i;

  int       box_x;
  int       box_y;

  int       magnet_collected;

  /* initialize magnet collected flag */
  magnet_collected = 0;

  /* cycle through all pickups */
  for ( k = WORLD_PICKUPS_START_INDEX; 
        k < WORLD_PICKUPS_END_INDEX; 
        k++)
  {
    t1 = &G_world_all_things[k];

    if (t1->type == THING_TYPE_NONE)
      continue;

    /* check if this pickup was collided with by the player */
    if (t1->flag == THING_COLLISION_FLAG_PICKUP_COLLECTED)
    {
      if (t1->type == THING_TYPE_CARROT)
      {
        G_carrots_remaining -= 1;

        /* play carrot pickup sound */
        if (G_carrots_remaining > 0)
          doremi_play_sfx(SFX_INDEX_PICKUP_CARROT);
      }
      else if (t1->type == THING_TYPE_BOOK)
      {
        if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
          G_bunny_gravity = BUNNY_GRAVITY_UP;
        else
          G_bunny_gravity = BUNNY_GRAVITY_DOWN;

        /* switch jumping to falling if necessary */
        if (G_bunny_action == BUNNY_ACTION_HOPPING)
          G_bunny_action = BUNNY_ACTION_FALLING;
        else if (G_bunny_action == BUNNY_ACTION_JUMPING)
          G_bunny_action = BUNNY_ACTION_FALLING;
        else if (G_bunny_action == BUNNY_ACTION_FALLING)
          G_bunny_action = BUNNY_ACTION_JUMPING;
        else if (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE)
          G_bunny_action = BUNNY_ACTION_FALLING_DOUBLE;
        else if (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE)
          G_bunny_action = BUNNY_ACTION_JUMPING_DOUBLE;

        /* determine grid box that contains this pickup */
        box_x = (t1->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) % GRID_WIDTH;
        box_y = (t1->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) % GRID_HEIGHT;

        b = &G_collision_grid[(box_y * GRID_WIDTH) + box_x];

        /* set spawner timer */
        b->back_timer_offset = 240 - G_timer_count;

        /* play book pickup sound */
        if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
          doremi_play_sfx(SFX_INDEX_PICKUP_BOOK_RIGHTSIDE_UP);
        else
          doremi_play_sfx(SFX_INDEX_PICKUP_BOOK_UPSIDE_DOWN);
      }
      else if (t1->type == THING_TYPE_MUSIC_NOTES)
      {
        /* mark all marbles of this color in the grid as matched */
        for (n = 0; n < GRID_HEIGHT; n++)
        {
          for (m = 0; m < GRID_WIDTH; m++)
          {
            b = &G_collision_grid[(n * GRID_WIDTH) + m];

            if (b->front_object == GRID_OBJECT_MARBLE)
            {
              if (t1->color == b->front_color)
                b->front_state = STATE_MARBLE_MATCHED;
            }
          }
        }

        /* mark held marble of this color as matched */
        t2 = &G_world_all_things[WORLD_HELD_THING_INDEX];

        if ((t2->type == THING_TYPE_HELD_MARBLE) && (t2->color == t1->color))
          t2->state = STATE_MARBLE_MATCHED;

        /* mark thrown marbles of this color as matched */
        for ( i = WORLD_THROWN_THINGS_START_INDEX; 
              i < WORLD_THROWN_THINGS_END_INDEX; 
              i++)
        {
          t2 = &G_world_all_things[i];

          if ((t2->type == THING_TYPE_THROWN_MARBLE) && (t2->color == t1->color))
            t2->state = STATE_MARBLE_MATCHED;
        }

        /* play marble flashing sound */
        doremi_play_sfx(SFX_INDEX_MARBLE_FLASHING);
      }
      else if ( (t1->type == THING_TYPE_MAGNET_DOWN)   || 
                (t1->type == THING_TYPE_MAGNET_UP)     || 
                (t1->type == THING_TYPE_MAGNET_RIGHT)  || 
                (t1->type == THING_TYPE_MAGNET_LEFT))
      {
        if (t1->type == THING_TYPE_MAGNET_DOWN)
          G_marble_gravity = MARBLE_GRAVITY_UP;
        else if (t1->type == THING_TYPE_MAGNET_UP)
          G_marble_gravity = MARBLE_GRAVITY_DOWN;
        else if (t1->type == THING_TYPE_MAGNET_RIGHT)
          G_marble_gravity = MARBLE_GRAVITY_LEFT;
        else if (t1->type == THING_TYPE_MAGNET_LEFT)
          G_marble_gravity = MARBLE_GRAVITY_RIGHT;

        /* determine grid box that contains this pickup */
        box_x = (t1->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) % GRID_WIDTH;
        box_y = (t1->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) % GRID_HEIGHT;

        b = &G_collision_grid[(box_y * GRID_WIDTH) + box_x];

        /* set spawner timer */
        b->back_timer_offset = 240 - G_timer_count;

        /* adjust currently falling marbles based on marble gravity */
        for ( i = WORLD_THROWN_THINGS_START_INDEX; 
              i < WORLD_THROWN_THINGS_END_INDEX; 
              i++)
        {
          t2 = &G_world_all_things[i];

          if (t2->type == THING_TYPE_THROWN_MARBLE)
          {
            if ((t2->vel_x == -THING_THROWN_FALLING_VEL)  || 
                (t2->vel_x == THING_THROWN_FALLING_VEL)   || 
                (t2->vel_y == -THING_THROWN_FALLING_VEL)  || 
                (t2->vel_y == THING_THROWN_FALLING_VEL))
            {
              if (G_marble_gravity == MARBLE_GRAVITY_DOWN)
              {
                t2->vel_x = 0;
                t2->vel_y = THING_THROWN_FALLING_VEL;
              }
              else if (G_marble_gravity == MARBLE_GRAVITY_UP)
              {
                t2->vel_x = 0;
                t2->vel_y = -THING_THROWN_FALLING_VEL;
              }
              else if (G_marble_gravity == MARBLE_GRAVITY_RIGHT)
              {
                t2->vel_x = THING_THROWN_FALLING_VEL;
                t2->vel_y = 0;
              }
              else if (G_marble_gravity == MARBLE_GRAVITY_LEFT)
              {
                t2->vel_x = -THING_THROWN_FALLING_VEL;
                t2->vel_y = 0;
              }
              else
              {
                t2->vel_x = 0;
                t2->vel_y = THING_THROWN_FALLING_VEL;
              }
            }
          }
        }

        /* set magnet collected flag */
        magnet_collected = 1;

        /* play magnet pickup sound */
        doremi_play_sfx(SFX_INDEX_PICKUP_MAGNET);
      }
      else if (t1->type == THING_TYPE_GEM)
      {
        /* mark all marbles of this color as ready to change */
        for (n = 0; n < GRID_HEIGHT; n++)
        {
          for (m = 0; m < GRID_WIDTH; m++)
          {
            b = &G_collision_grid[(n * GRID_WIDTH) + m];

            if (b->front_object == GRID_OBJECT_MARBLE)
            {
              if (t1->color == b->front_color)
                b->front_state = STATE_MARBLE_READY_TO_CHANGE;
            }
          }
        }

        /* mark held marble of this color as ready to change */
        t2 = &G_world_all_things[WORLD_HELD_THING_INDEX];

        if ((t2->type == THING_TYPE_HELD_MARBLE) && (t2->color == t1->color))
          t2->state = STATE_MARBLE_READY_TO_CHANGE;

        /* mark thrown marbles of this color as ready to change */
        for ( i = WORLD_THROWN_THINGS_START_INDEX; 
              i < WORLD_THROWN_THINGS_END_INDEX; 
              i++)
        {
          t2 = &G_world_all_things[i];

          if ((t2->type == THING_TYPE_THROWN_MARBLE) && (t2->color == t1->color))
            t2->state = STATE_MARBLE_READY_TO_CHANGE;
        }

        /* play marble changing sound */
        if (t1->color == COLOR_METAL)
          doremi_play_sfx(SFX_INDEX_MARBLE_CHANGING_TO_WILD);
        else
          doremi_play_sfx(SFX_INDEX_MARBLE_CHANGING_TO_METAL);
      }
      else if (t1->type == THING_TYPE_KEY)
      {
        /* mark all door blocks of this color as ready to open */
        for (n = 0; n < GRID_HEIGHT; n++)
        {
          for (m = 0; m < GRID_WIDTH; m++)
          {
            b = &G_collision_grid[(n * GRID_WIDTH) + m];

            if (b->front_object == GRID_OBJECT_DOOR_BLOCK)
            {
              if (t1->color == b->front_color)
                b->front_state = STATE_DOOR_BLOCK_READY_TO_OPEN;
            }
          }
        }

        /* play key pickup sound */
        doremi_play_sfx(SFX_INDEX_PICKUP_KEY);
      }
      else if (t1->type == THING_TYPE_MUSHROOM)
      {
        /* mark all invisible blocks of this color as ready to appear */
        for (n = 0; n < GRID_HEIGHT; n++)
        {
          for (m = 0; m < GRID_WIDTH; m++)
          {
            b = &G_collision_grid[(n * GRID_WIDTH) + m];

            if (b->back_object == GRID_OBJECT_INVISIBLE_BLOCK)
            {
              if (t1->color == b->back_color)
                b->back_state = STATE_INVISIBLE_BLOCK_READY_TO_APPEAR;
            }
          }
        }

        /* play mushroom pickup sound */
        doremi_play_sfx(SFX_INDEX_PICKUP_MUSHROOM);
      }

      /* despawn this pickup */
      world_despawn_thing(k);
    }
  }

  /* check magnets, switch to non-redundant types if necessary */
  if (magnet_collected == 1)
  {
    for ( k = WORLD_PICKUPS_START_INDEX; 
          k < WORLD_PICKUPS_END_INDEX; 
          k++)
    {
      t1 = &G_world_all_things[k];

      if ((t1->type != THING_TYPE_MAGNET_DOWN)  && 
          (t1->type != THING_TYPE_MAGNET_UP)    && 
          (t1->type != THING_TYPE_MAGNET_RIGHT) && 
          (t1->type != THING_TYPE_MAGNET_LEFT))
      {
        continue;
      }

      /* determine grid box that contains this pickup */
      box_x = (t1->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) % GRID_WIDTH;
      box_y = (t1->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) % GRID_HEIGHT;

      b = &G_collision_grid[(box_y * GRID_WIDTH) + box_x];

      /* change magnet type based on pickup spawner */
      if (b->back_object == GRID_OBJECT_MAGNET_DOWN_SPAWNER)
      {
        if (G_marble_gravity == MARBLE_GRAVITY_UP)
        {
          t1->type = THING_TYPE_MAGNET_UP;
          t1->orient = THING_ORIENT_FLIP_VERT;
        }
        else
        {
          t1->type = THING_TYPE_MAGNET_DOWN;
          t1->orient = THING_ORIENT_NORMAL;
        }
      }
      else if (b->back_object == GRID_OBJECT_MAGNET_RIGHT_SPAWNER)
      {
        if (G_marble_gravity == MARBLE_GRAVITY_LEFT)
        {
          t1->type = THING_TYPE_MAGNET_LEFT;
          t1->orient = THING_ORIENT_FLIP_HORI;
        }
        else
        {
          t1->type = THING_TYPE_MAGNET_RIGHT;
          t1->orient = THING_ORIENT_NORMAL;
        }
      }
      else if (b->back_object == GRID_OBJECT_MAGNET_LEFT_SPAWNER)
      {
        if (G_marble_gravity == MARBLE_GRAVITY_RIGHT)
        {
          t1->type = THING_TYPE_MAGNET_RIGHT;
          t1->orient = THING_ORIENT_NORMAL;
        }
        else
        {
          t1->type = THING_TYPE_MAGNET_LEFT;
          t1->orient = THING_ORIENT_FLIP_HORI;
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_move_critters()
*******************************************************************************/
short int logic_move_critters()
{
  int i;

  thing* t;

  int rider_indices[2];

  int k;

  grid_box* b[2];

  int box_x[2];
  int box_y;

  thing*  p;
  int     turn_flag;

  unsigned int adjusted_timer_count;

  /* move critters */
  for ( i = WORLD_CRITTERS_START_INDEX; 
        i < WORLD_CRITTERS_END_INDEX; 
        i++)
  {
    t = &G_world_all_things[i];

    if (t->type == THING_TYPE_NONE)
      continue;

    /* update vertical velocity based on gravity */
    if ((t->type == THING_TYPE_CRITTER_WALKER_1) || 
        (t->type == THING_TYPE_CRITTER_WALKER_2))
    {
      t->vel_y += THING_GRAVITATIONAL_ACC;

      if (t->vel_y > THING_TERMINAL_VEL)
        t->vel_y = THING_TERMINAL_VEL;
    }

    /* check if this critter is stunned */
    if (t->state == STATE_CRITTER_STUNNED)
    {
      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

      if (adjusted_timer_count >= THING_CRITTER_STUNNED_TIME)
      {
        t->state = STATE_NONE;
        t->timer_offset = 0;
      }
      else
      {
        if ((t->type == THING_TYPE_CRITTER_WALKER_1)  || 
            (t->type == THING_TYPE_CRITTER_WALKER_2))
        {
          grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

          if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
            t->vel_y = 0;
        }

        continue;
      }
    }

    /* try to turn if necessary */
    if (t->state == STATE_CRITTER_TURNING)
    {
      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

      if (adjusted_timer_count >= THING_CRITTER_TURN_TIME)
      {
        if ((t->type == THING_TYPE_CRITTER_WALKER_1)  || 
            (t->type == THING_TYPE_CRITTER_WALKER_2)  || 
            (t->type == THING_TYPE_CRITTER_FLYER_1))
        {
          if (t->orient == THING_ORIENT_NORMAL)
          {
            t->orient = THING_ORIENT_FLIP_HORI;
            t->vel_x = -THING_CRITTER_VEL;
          }
          else if (t->orient == THING_ORIENT_FLIP_HORI)
          {
            t->orient = THING_ORIENT_NORMAL;
            t->vel_x = THING_CRITTER_VEL;
          }
        }
        else if (t->type == THING_TYPE_CRITTER_FLYER_2)
        {
          if (t->orient == THING_ORIENT_NORMAL)
          {
            t->orient = THING_ORIENT_FLIP_HORI;
            t->vel_y = -THING_CRITTER_VEL;
          }
          else if (t->orient == THING_ORIENT_FLIP_HORI)
          {
            t->orient = THING_ORIENT_NORMAL;
            t->vel_y = THING_CRITTER_VEL;
          }
        }

        t->state = STATE_NONE;
        t->timer_offset = 0;
      }
      else
      {
        if ((t->type == THING_TYPE_CRITTER_WALKER_1)  || 
            (t->type == THING_TYPE_CRITTER_WALKER_2))
        {
          grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

          if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
            t->vel_y = 0;
        }

        continue;
      }
    }

    /* determine rider indices    */
    /* note that the bunny is the */
    /* only possible rider here   */
    rider_indices[0] = -1;
    rider_indices[1] = G_world_riders[i - WORLD_CRITTERS_START_INDEX + WORLD_RIDERS_ON_CRITTER_START_INDEX];

    if ((rider_indices[1] == WORLD_BUNNY_INDEX) && (G_bunny_gravity == BUNNY_GRAVITY_UP))
    {
      rider_indices[0] = WORLD_BUNNY_INDEX;
      rider_indices[1] = -1;
    }

    /* move critter */
    if ((t->type == THING_TYPE_CRITTER_WALKER_1) || 
        (t->type == THING_TYPE_CRITTER_WALKER_2))
    {
      /* horizontal move */
      grid_move_thing(i, GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);

      /* if there was a collision, change direction */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        t->vel_x = 0;
        t->state = STATE_CRITTER_TURNING;
        t->timer_offset = 240 - G_timer_count;
      }
      /* if there was no collision, move the riders */
      else
      {
        if (rider_indices[0] != -1)
          grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);

        if (rider_indices[1] != -1)
          grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);
      }

      /* vertical move */
      grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        t->vel_y = 0;

        /* check if this critter is approaching an edge */
        if (t->type == THING_TYPE_CRITTER_WALKER_2)
        {
          /* determine grid boxes beneath critter's front & back edges  */
          /* note that the y coordinate is calculated as one more so    */
          /* we get the position just beneath the critter               */
          if (t->vel_x >= 0)
          {
            box_x[0] = (t->pos_x - t->hx) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
            box_x[1] = (t->pos_x + t->hx - 1) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
          }
          else
          {
            box_x[0] = (t->pos_x + t->hx - 1) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
            box_x[1] = (t->pos_x - t->hx) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
          }

          box_y = (t->pos_y + t->hy) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

          /* wraparound box indices */
          box_x[0] = (box_x[0] + GRID_WIDTH) % GRID_WIDTH;
          box_x[1] = (box_x[1] + GRID_WIDTH) % GRID_WIDTH;

          box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

          b[0] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[0]];
          b[1] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[1]];

          /* check if this critter should change direction */

          /* if this critter's front edge is over a block, keep moving */
          if (b[1]->front_object != GRID_OBJECT_NONE)
            turn_flag = 0;
          /* otherwise, if this critter's back edge is over a   */
          /* block, check if there is a platform to walk onto.  */
          /* if there is, keep moving. otherwise, turn around.  */
          else if (b[0]->front_object != GRID_OBJECT_NONE)
          {
            turn_flag = 1;

            for (k = 0; k < b[1]->num_things; k++)
            {
              /* don't compare this critter with itself */
              if (b[1]->thing_indices[k] == i)
                continue;

              /* check if this thing is a platform */
              if ((b[1]->thing_indices[k] >=  WORLD_PLATFORMS_ETC_START_INDEX) && 
                  (b[1]->thing_indices[k] <   WORLD_PLATFORMS_ETC_END_INDEX))
              {
                turn_flag = 0;
              }
            }
          }
          /* otherwise, check if this critter is on top of  */
          /* a platform. if the critter's front edge is     */
          /* over the edge of the platform, turn around.    */
          else
          {
            turn_flag = 0;

            for (k = 0; k < b[0]->num_things; k++)
            {
              /* don't compare this critter with itself */
              if (b[0]->thing_indices[k] == i)
                continue;

              /* check if this thing is a platform */
              if ((b[0]->thing_indices[k] >=  WORLD_PLATFORMS_ETC_START_INDEX) && 
                  (b[0]->thing_indices[k] <   WORLD_PLATFORMS_ETC_END_INDEX))
              {
                p = &G_world_all_things[b[0]->thing_indices[k]];

                /* check if this critter is aligned with the  */
                /* platform. then, if its front edge is past  */
                /* the edge of the platform, turn around.     */
                if (t->pos_y + t->hy == p->pos_y - p->hy)
                {
                  if ((t->vel_x > 0) && (t->pos_x + t->hx - 1 > p->pos_x + p->hx - 1))
                    turn_flag = 1;
                  else if ((t->vel_x < 0) && (t->pos_x - t->hx < p->pos_x - p->hx))
                    turn_flag = 1;
                }
              }
            }
          }

          if (turn_flag == 1)
          {
            t->vel_x = 0;
            t->state = STATE_CRITTER_TURNING;
            t->timer_offset = 240 - G_timer_count;
          }
        }
      }
    }
    else if (t->type == THING_TYPE_CRITTER_FLYER_1)
    {
      /* horizontal move */
      grid_move_thing(i, GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);

      /* if there was a collision, change direction */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        t->vel_x = 0;
        t->state = STATE_CRITTER_TURNING;
        t->timer_offset = 240 - G_timer_count;
      }
      /* if there was no collision, move the riders */
      else
      {
        for (k = 0; k < 2; k++)
        {
          if (rider_indices[k] != -1)
            grid_move_thing(rider_indices[k], GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);
        }
      }
    }
    else if (t->type == THING_TYPE_CRITTER_FLYER_2)
    {
      /* if we are moving up, we want to move the top   */
      /* rider, then the critter, then the bottom rider */
      if (t->vel_y < 0)
      {
        /* move top rider */
        if (rider_indices[1] != -1)
          grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

        /* vertical move */
        grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

        /* if there was a collision, change direction */
        if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
        {
          if (t->state == STATE_CRITTER_NEGATE_TURN)
            t->state = STATE_NONE;
          else
          {
            t->vel_y = 0;
            t->state = STATE_CRITTER_TURNING;
            t->timer_offset = 240 - G_timer_count;
          }
        }

        /* move bottom rider */
        if (rider_indices[0] != -1)
          grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);
      }
      /* if we are moving down, we want to move the bottom  */
      /* rider, then the critter, then the top rider        */
      else
      {
        /* move bottom rider */
        if (rider_indices[0] != -1)
          grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

        /* vertical move */
        grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

        /* if there was a collision, change direction */
        if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
        {
          if (t->state == STATE_CRITTER_NEGATE_TURN)
            t->state = STATE_NONE;
          else
          {
            t->vel_y = 0;
            t->state = STATE_CRITTER_TURNING;
            t->timer_offset = 240 - G_timer_count;
          }
        }

        /* move top rider */
        if (rider_indices[1] != -1)
          grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_move_platforms_etc()
*******************************************************************************/
short int logic_move_platforms_etc()
{
  int     i;

  thing*  t;

  int     rider_indices[3];

  int     old_pos_y;

  int     play_sfx_flag;

  unsigned int adjusted_timer_count;

  /* move platforms, elevators, etc */
  for ( i = WORLD_PLATFORMS_ETC_START_INDEX; 
        i < WORLD_PLATFORMS_ETC_END_INDEX; 
        i++)
  {
    t = &G_world_all_things[i];

    if (t->type == THING_TYPE_NONE)
      continue;

    /* if this platform is trying to snap to the grid, skip it for now */
    if (t->state == STATE_AWAITING_SNAP_TO_GRID)
      continue;

    /* try to turn if necessary */
    if (t->state == STATE_ELEVATOR_TURNING)
    {
      /* determine adjusted timer count */
      adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

      if (adjusted_timer_count >= THING_ELEVATOR_TURN_TIME)
      {
        if (t->type == THING_TYPE_ELEVATOR_HORI_RIGHT)
          t->type = THING_TYPE_ELEVATOR_HORI_LEFT;
        else if (t->type == THING_TYPE_ELEVATOR_HORI_LEFT)
          t->type = THING_TYPE_ELEVATOR_HORI_RIGHT;
        else if (t->type == THING_TYPE_ELEVATOR_VERT_UP)
          t->type = THING_TYPE_ELEVATOR_VERT_DOWN;
        else if (t->type == THING_TYPE_ELEVATOR_VERT_DOWN)
          t->type = THING_TYPE_ELEVATOR_VERT_UP;

        t->state = STATE_NONE;
        t->timer_offset = 0;
      }
      else
        continue;
    }

    /* determine rider indices */
    rider_indices[0] = G_world_riders[i - WORLD_PLATFORMS_ETC_START_INDEX + WORLD_RIDERS_UNDER_PLATFORM_START_INDEX];
    rider_indices[1] = G_world_riders[i - WORLD_PLATFORMS_ETC_START_INDEX + WORLD_RIDERS_ON_PLATFORM_START_INDEX];

    if ((rider_indices[1] != -1) && (rider_indices[1] != WORLD_BUNNY_INDEX))
      rider_indices[2] = G_world_riders[rider_indices[1] - WORLD_CRITTERS_START_INDEX + WORLD_RIDERS_ON_CRITTER_START_INDEX];
    else
      rider_indices[2] = -1;

    /* initialize play sfx flag */
    play_sfx_flag = 0;

    /* activate elevators, bolts, rocket blocks, etc based on riders */
    if (t->type == THING_TYPE_BASS_KNOB)
    {
      if ((rider_indices[0] != -1) || (rider_indices[1] != -1))
      {
        if (t->state == STATE_RETRACTING_STOPPED)
          t->timer_offset = 240 - G_timer_count;

        if (t->vel_y != THING_RETRACT_VEL)
          play_sfx_flag = 1;

        t->vel_y = THING_RETRACT_VEL;
        t->state = STATE_NONE;
      }
      else if (t->state != STATE_RETRACTING_STOPPED)
      {
        if (t->vel_y != -THING_PLATFORM_VEL)
          play_sfx_flag = 1;

        t->vel_y = -THING_PLATFORM_VEL;
      }
    }
    else if (t->type == THING_TYPE_TREBLE_KNOB)
    {
      if ((rider_indices[0] != -1) || (rider_indices[1] != -1))
      {
        if (t->state == STATE_RETRACTING_STOPPED)
          t->timer_offset = 240 - G_timer_count;

        if (t->vel_y != -THING_RETRACT_VEL)
          play_sfx_flag = 1;

        t->vel_y = -THING_RETRACT_VEL;
        t->state = STATE_NONE;
      }
      else if (t->state != STATE_RETRACTING_STOPPED)
      {
        if (t->vel_y != THING_PLATFORM_VEL)
          play_sfx_flag = 1;

        t->vel_y = THING_PLATFORM_VEL;
      }
    }
    else if (t->type == THING_TYPE_ROCKET_BLOCK_RIGHT)
    {
      if ((rider_indices[0] != -1) || (rider_indices[1] != -1))
      {
        if (t->state == STATE_RETRACTING_STOPPED)
          t->timer_offset = 240 - G_timer_count;

        if (t->vel_x != THING_PLATFORM_VEL)
          play_sfx_flag = 1;

        t->vel_x = THING_PLATFORM_VEL;
        t->state = STATE_NONE;
      }
      else if (t->state != STATE_RETRACTING_STOPPED)
      {
        if (t->vel_x != -THING_RETRACT_VEL)
          play_sfx_flag = 1;

        t->vel_x = -THING_RETRACT_VEL;
      }
    }
    else if (t->type == THING_TYPE_ROCKET_BLOCK_LEFT)
    {
      if ((rider_indices[0] != -1) || (rider_indices[1] != -1))
      {
        if (t->state == STATE_RETRACTING_STOPPED)
          t->timer_offset = 240 - G_timer_count;

        if (t->vel_x != -THING_PLATFORM_VEL)
          play_sfx_flag = 1;

        t->vel_x = -THING_PLATFORM_VEL;
        t->state = STATE_NONE;
      }
      else if (t->state != STATE_RETRACTING_STOPPED)
      {
        if (t->vel_x != THING_RETRACT_VEL)
          play_sfx_flag = 1;

        t->vel_x = THING_RETRACT_VEL;
      }
    }
    else if (t->type == THING_TYPE_ELEVATOR_HORI_RIGHT)
    {
      if ((rider_indices[0] != -1) || (rider_indices[1] != -1))
      {
        if (t->vel_x == 0)
          play_sfx_flag = 1;

        t->vel_x = THING_PLATFORM_VEL;
      }
      else
      {
        if (t->vel_x != 0)
          play_sfx_flag = 1;

        t->vel_x = 0;
      }
    }
    else if (t->type == THING_TYPE_ELEVATOR_HORI_LEFT)
    {
      if ((rider_indices[0] != -1) || (rider_indices[1] != -1))
      {
        if (t->vel_x == 0)
          play_sfx_flag = 1;

        t->vel_x = -THING_PLATFORM_VEL;
      }
      else
      {
        if (t->vel_x != 0)
          play_sfx_flag = 1;

        t->vel_x = 0;
      }
    }
    else if (t->type == THING_TYPE_ELEVATOR_VERT_UP)
    {
      if ((rider_indices[0] != -1) || (rider_indices[1] != -1))
      {
        if (t->vel_y == 0)
          play_sfx_flag = 1;

        t->vel_y = -THING_PLATFORM_VEL;
      }
      else
      {
        if (t->vel_y != 0)
          play_sfx_flag = 1;

        t->vel_y = 0;
      }
    }
    else if (t->type == THING_TYPE_ELEVATOR_VERT_DOWN)
    {
      if ((rider_indices[0] != -1) || (rider_indices[1] != -1))
      {
        if (t->vel_y == 0)
          play_sfx_flag = 1;

        t->vel_y = THING_PLATFORM_VEL;
      }
      else
      {
        if (t->vel_y != 0)
          play_sfx_flag = 1;

        t->vel_y = 0;
      }
    }
    else if (t->type == THING_TYPE_DROP_BOLT_DOWN)
    {
      if (rider_indices[1] != -1)
      {
        if (t->state == STATE_RETRACTING_STOPPED)
          t->timer_offset = 240 - G_timer_count;

        if (t->vel_y != THING_PLATFORM_VEL)
          play_sfx_flag = 1;

        t->vel_y = THING_PLATFORM_VEL;
        t->state = STATE_NONE;
      }
      else if (t->state != STATE_RETRACTING_STOPPED)
      {
        if (t->vel_y != -THING_RETRACT_VEL)
          play_sfx_flag = 1;

        t->vel_y = -THING_RETRACT_VEL;
      }
    }
    else if (t->type == THING_TYPE_DROP_BOLT_UP)
    {
      if (rider_indices[0] != -1)
      {
        if (t->state == STATE_RETRACTING_STOPPED)
          t->timer_offset = 240 - G_timer_count;

        if (t->vel_y != -THING_PLATFORM_VEL)
          play_sfx_flag = 1;

        t->vel_y = -THING_PLATFORM_VEL;
        t->state = STATE_NONE;
      }
      else if (t->state != STATE_RETRACTING_STOPPED)
      {
        if (t->vel_y != THING_RETRACT_VEL)
          play_sfx_flag = 1;

        t->vel_y = THING_RETRACT_VEL;
      }
    }
    else if (t->type == THING_TYPE_ANTI_BOLT_UP)
    {
      if (rider_indices[1] != -1)
      {
        if (t->state == STATE_RETRACTING_STOPPED)
          t->timer_offset = 240 - G_timer_count;

        if (t->vel_y != -THING_PLATFORM_VEL)
          play_sfx_flag = 1;

        t->vel_y = -THING_PLATFORM_VEL;
        t->state = STATE_NONE;
      }
      else if (t->state != STATE_RETRACTING_STOPPED)
      {
        if (t->vel_y != THING_RETRACT_VEL)
          play_sfx_flag = 1;

        t->vel_y = THING_RETRACT_VEL;
      }
    }
    else if (t->type == THING_TYPE_ANTI_BOLT_DOWN)
    {
      if (rider_indices[0] != -1)
      {
        if (t->state == STATE_RETRACTING_STOPPED)
          t->timer_offset = 240 - G_timer_count;

        if (t->vel_y != THING_PLATFORM_VEL)
          play_sfx_flag = 1;

        t->vel_y = THING_PLATFORM_VEL;
        t->state = STATE_NONE;
      }
      else if (t->state != STATE_RETRACTING_STOPPED)
      {
        if (t->vel_y != -THING_RETRACT_VEL)
          play_sfx_flag = 1;

        t->vel_y = -THING_RETRACT_VEL;
      }
    }

    /* move platform, elevator, etc */
    /* horizontal movement */
    if (t->vel_x != 0)
    {
      /* move platform */
      grid_move_thing(i, GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);

      /* if there was a collision, flip the     */
      /* direction of an elevator if necessary  */
      /* also, set the velocity of an extending */
      /* rocket block to 0.                     */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        if ((t->type == THING_TYPE_ELEVATOR_HORI_RIGHT) || 
            (t->type == THING_TYPE_ELEVATOR_HORI_LEFT))
        {
          t->vel_x = 0;
          t->state = STATE_ELEVATOR_TURNING;
          t->timer_offset = 240 - G_timer_count;
        }

        if (((t->type == THING_TYPE_ROCKET_BLOCK_RIGHT) && (t->vel_x > 0)) || 
            ((t->type == THING_TYPE_ROCKET_BLOCK_LEFT)  && (t->vel_x < 0)))
        {
          t->vel_x = 0;
        }
      }
      /* if there was no collision, move the riders */
      else
      {
        if (rider_indices[0] != -1)
          grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);

        if (rider_indices[1] != -1)
          grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);

        if (rider_indices[2] != -1)
          grid_move_thing(rider_indices[2], GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);
      }
    }
    /* upward movement */
    else if (t->vel_y < 0)
    {
      /* store old position (may be needed later) */
      old_pos_y = t->pos_y;

      /* move top riders */
      if (rider_indices[2] != -1)
        grid_move_thing(rider_indices[2], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      if (rider_indices[1] != -1)
        grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      /* move platform */
      grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      /* if there was a collision, flip the direction */
      /* of an elevator if necessary. also, set the   */
      /* velocity of a bolt or knob to 0.             */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        if (t->type == THING_TYPE_ELEVATOR_VERT_UP)
        {
          t->vel_y = 0;
          t->state = STATE_ELEVATOR_TURNING;
          t->timer_offset = 240 - G_timer_count;
        }

        if ((t->type == THING_TYPE_BASS_KNOB)       || 
            (t->type == THING_TYPE_TREBLE_KNOB)     || 
            (t->type == THING_TYPE_DROP_BOLT_DOWN)  || 
            (t->type == THING_TYPE_DROP_BOLT_UP)    || 
            (t->type == THING_TYPE_ANTI_BOLT_UP)    || 
            (t->type == THING_TYPE_ANTI_BOLT_DOWN))
        {
          t->vel_y = 0;
        }
      }

      /* move bottom rider */
      if (rider_indices[0] != -1)
      {
        grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

        /* note: if the bottom rider collided with something, and */
        /* that rider was activating this platform's movement,    */
        /* we should cancel the platform's original move.         */
        if (G_world_all_things[rider_indices[0]].flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
        {
          if ((t->type == THING_TYPE_DROP_BOLT_UP) || 
              ((t->type == THING_TYPE_TREBLE_KNOB) && (rider_indices[1] == -1)))
          {
            grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, old_pos_y - t->pos_y);

            t->vel_y = 0;
          }
        }
      }
    }
    /* downward movement */
    else if (t->vel_y > 0)
    {
      /* store old position (may be needed later) */
      old_pos_y = t->pos_y;

      /* move bottom rider */
      if (rider_indices[0] != -1)
        grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      /* move platform */
      grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      /* if there was a collision, flip the direction */
      /* of an elevator if necessary. also, set the   */
      /* velocity of a bolt or knob to 0.             */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        if (t->type == THING_TYPE_ELEVATOR_VERT_DOWN)
        {
          t->vel_y = 0;
          t->state = STATE_ELEVATOR_TURNING;
          t->timer_offset = 240 - G_timer_count;
        }

        if ((t->type == THING_TYPE_BASS_KNOB)       || 
            (t->type == THING_TYPE_TREBLE_KNOB)     || 
            (t->type == THING_TYPE_DROP_BOLT_DOWN)  || 
            (t->type == THING_TYPE_DROP_BOLT_UP)    || 
            (t->type == THING_TYPE_ANTI_BOLT_UP)    || 
            (t->type == THING_TYPE_ANTI_BOLT_DOWN))
        {
          t->vel_y = 0;
        }
      }

      /* move top riders */
      if (rider_indices[1] != -1)
      {
        grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

        /* note: if the top rider collided with something, and  */
        /* that rider was activating this platform's movement,  */
        /* we should cancel the platform's original move.       */
        if (G_world_all_things[rider_indices[1]].flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
        {
          if ((t->type == THING_TYPE_DROP_BOLT_DOWN) || 
              ((t->type == THING_TYPE_BASS_KNOB) && (rider_indices[0] == -1)))
          {
            grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, old_pos_y - t->pos_y);

            t->vel_y = 0;
          }
        }
      }

      if (rider_indices[2] != -1)
        grid_move_thing(rider_indices[2], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);
    }

    /* determine if a sound effect should be played             */
    /* this is for bolts, bass/treble knobs, and rocket blocks  */
    /* if they did not actually move this frame, cancel the sfx */
    /* for the elevators, just play the appropriate sfx         */
    if (play_sfx_flag == 1)
    {
      if ((t->type == THING_TYPE_BASS_KNOB)       || 
          (t->type == THING_TYPE_DROP_BOLT_DOWN)  || 
          (t->type == THING_TYPE_ANTI_BOLT_DOWN))
      {
        if (t->vel_y > 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_ON);
        else if (t->vel_y < 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_OFF);
      }
      else if ( (t->type == THING_TYPE_TREBLE_KNOB)   || 
                (t->type == THING_TYPE_DROP_BOLT_UP)  || 
                (t->type == THING_TYPE_ANTI_BOLT_UP))
      {
        if (t->vel_y < 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_ON);
        else if (t->vel_y > 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_OFF);
      }
      else if (t->type == THING_TYPE_ROCKET_BLOCK_RIGHT)
      {
        if (t->vel_x > 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_ON);
        else if (t->vel_x < 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_OFF);
      }
      else if (t->type == THING_TYPE_ROCKET_BLOCK_LEFT)
      {
        if (t->vel_x < 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_ON);
        else if (t->vel_x > 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_OFF);
      }
      else if ( (t->type == THING_TYPE_ELEVATOR_HORI_RIGHT) || 
                (t->type == THING_TYPE_ELEVATOR_HORI_LEFT))
      {
        if (t->vel_x != 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_ON);
        else if (t->vel_x == 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_OFF);
      }
      else if ( (t->type == THING_TYPE_ELEVATOR_VERT_UP) || 
                (t->type == THING_TYPE_ELEVATOR_VERT_DOWN))
      {
        if (t->vel_y != 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_ON);
        else if (t->vel_y == 0)
          doremi_play_sfx(SFX_INDEX_PLATFORM_OFF);
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_move_thrown_things()
*******************************************************************************/
short int logic_move_thrown_things()
{
  int i;

  thing* t;

  int rider_indices[2];

  int hori_collision_check;
  int vert_collision_check;

  /* move thrown things */
  for ( i = WORLD_THROWN_THINGS_START_INDEX; 
        i < WORLD_THROWN_THINGS_END_INDEX; 
        i++)
  {
    t = &G_world_all_things[i];

    if (t->type == THING_TYPE_NONE)
      continue;

    /* if this thrown thing is trying to snap to the grid, skip it for now */
    if (t->state == STATE_AWAITING_SNAP_TO_GRID)
      continue;

    /* if this is a flashing or changing marble, just have it remain    */
    /* motionless until the flash or change is finished (this is in     */
    /* case it runs into something like an arrow or the bunny jumping   */
    /* on it, and needs to set another state to handle that situation). */
    if ((t->state == STATE_MARBLE_MATCHED)          || 
        (t->state == STATE_MARBLE_READY_TO_CHANGE)  || 
        (t->state == STATE_MARBLE_FLASHING)         || 
        (t->state == STATE_MARBLE_CHANGING))
    {
      continue;
    }

    /* determine rider indices    */
    /* note that the bunny is the */
    /* only possible rider here   */
    rider_indices[0] = -1;
    rider_indices[1] = G_world_riders[i - WORLD_THROWN_THINGS_START_INDEX + WORLD_RIDERS_ON_THROWN_THING_START_INDEX];

    if ((rider_indices[1] == WORLD_BUNNY_INDEX) && (G_bunny_gravity == BUNNY_GRAVITY_UP))
    {
      rider_indices[0] = WORLD_BUNNY_INDEX;
      rider_indices[1] = -1;
    }

    /* initialize collision check flags */
    hori_collision_check = 0;
    vert_collision_check = 0;

    /* horizontal move */
    if (t->vel_x != 0)
      grid_move_thing(i, GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);

    /* check if there was a collision   */
    if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      hori_collision_check = 1;
    /* if there was no collision, move the riders */
    else
    {
      if (rider_indices[0] != -1)
        grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);

      if (rider_indices[1] != -1)
        grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);
    }

    /* upward move */
    if (t->vel_y < 0)
    {
      /* move top rider */
      if (rider_indices[1] != -1)
        grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      /* vertical move */
      grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      /* check if there was a collision */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
        vert_collision_check = 1;

      /* move bottom rider */
      if (rider_indices[0] != -1)
        grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);
    }
    /* downward move */
    else if (t->vel_y > 0)
    {
      /* move bottom rider */
      if (rider_indices[0] != -1)
        grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      /* vertical move */
      grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      /* check if there was a collision */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
        vert_collision_check = 1;

      /* move top rider */
      if (rider_indices[1] != -1)
        grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);
    }

    /* if there was a collision, bounce or land */
    if ((hori_collision_check == 1) || (vert_collision_check == 1))
    {
      if (t->type == THING_TYPE_THROWN_MARBLE)
      {
        /* the marble has landed! */
        /* (or, the marble ran into another marble in the grid) */
        if (((G_marble_gravity == MARBLE_GRAVITY_DOWN)  && (vert_collision_check == 1) && (t->vel_y > 0)) || 
            ((G_marble_gravity == MARBLE_GRAVITY_UP)    && (vert_collision_check == 1) && (t->vel_y < 0)) || 
            ((G_marble_gravity == MARBLE_GRAVITY_RIGHT) && (hori_collision_check == 1) && (t->vel_x > 0)) || 
            ((G_marble_gravity == MARBLE_GRAVITY_LEFT)  && (hori_collision_check == 1) && (t->vel_x < 0)) || 
            (t->state == STATE_THROWN_MARBLE_EARLY_LANDING))
        {
          /* if we have a bounce negation, ignore the landing */
          /* so the bunny can ride on this thrown thing       */
          if (t->state == STATE_THROWN_THING_NEGATE_BOUNCE)
          {
            t->state = STATE_NONE;
            continue;
          }

          /* otherwise, set the landed state! */
          t->state = STATE_THROWN_THING_LANDED;
        }
        /* if this is a non-diagonal bounce, count it as a potential  */
        /* landing (it may still be able to be zinged by an arrow)    */
        else if ( ((vert_collision_check == 1) && (t->vel_x == 0)) || 
                  ((hori_collision_check == 1) && (t->vel_y == 0)))
        {
          /* if we have a bounce negation, ignore the landing */
          /* so the bunny can ride on this thrown thing       */
          if (t->state == STATE_THROWN_THING_NEGATE_BOUNCE)
          {
            t->state = STATE_NONE;
            continue;
          }

          t->state = STATE_THROWN_THING_MAY_HAVE_LANDED;
        }
        /* otherwise, bounce the marble */
        else
        {
          if (hori_collision_check == 1)
          {
            t->vel_x = -t->vel_x;

            /* play bounce sound */
            doremi_play_sfx(SFX_INDEX_THROWN_THING_BOUNCE);
          }

          /* negate vertical bounce if necessary */
          if (t->state == STATE_THROWN_THING_NEGATE_BOUNCE)
            t->state = STATE_NONE;
          else if (vert_collision_check == 1)
          {
            t->vel_y = -t->vel_y;

            /* play bounce sound */
            doremi_play_sfx(SFX_INDEX_THROWN_THING_BOUNCE);
          }
        }
      }
      else if ( (t->type == THING_TYPE_THROWN_CRITTER_WALKER_1) || 
                (t->type == THING_TYPE_THROWN_CRITTER_WALKER_2) || 
                (t->type == THING_TYPE_THROWN_CRITTER_FLYER_1)  || 
                (t->type == THING_TYPE_THROWN_CRITTER_FLYER_2))
      {
        /* the critter has landed! */
        if ((vert_collision_check == 1) && (t->vel_y > 0))
        {
          /* if we have a bounce negation, ignore the landing */
          /* so the bunny can ride on this thrown thing       */
          if (t->state == STATE_THROWN_THING_NEGATE_BOUNCE)
          {
            t->state = STATE_NONE;
            continue;
          }

          /* otherwise, set the landed state! */
          t->state = STATE_THROWN_THING_LANDED;
        }
        /* if this is a non-diagonal bounce, count it as a potential  */
        /* landing (it may still be able to be zinged by an arrow)    */
        else if ( ((vert_collision_check == 1) && (t->vel_x == 0)) || 
                  ((hori_collision_check == 1) && (t->vel_y == 0)))
        {
          /* if we have a bounce negation, ignore the landing */
          /* so the bunny can ride on this thrown thing       */
          if (t->state == STATE_THROWN_THING_NEGATE_BOUNCE)
          {
            t->state = STATE_NONE;
            continue;
          }

          t->state = STATE_THROWN_THING_MAY_HAVE_LANDED;
        }
        /* otherwise, bounce the critter */
        else
        {
          if (hori_collision_check == 1)
          {
            t->vel_x = -t->vel_x;

            /* play bounce sound */
            doremi_play_sfx(SFX_INDEX_THROWN_THING_BOUNCE);
          }

          /* negate vertical bounce if necessary */
          if (t->state == STATE_THROWN_THING_NEGATE_BOUNCE)
            t->state = STATE_NONE;
          else if (vert_collision_check == 1)
          {
            t->vel_y = -t->vel_y;

            /* play bounce sound */
            doremi_play_sfx(SFX_INDEX_THROWN_THING_BOUNCE);
          }
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_move_bubbles()
*******************************************************************************/
short int logic_move_bubbles()
{
  int     i;

  thing*  t;

  int     pos_x;
  int     pos_y;

  unsigned int adjusted_timer_count;

  /* move bubbles */
  for ( i = WORLD_BUBBLES_START_INDEX; 
        i < WORLD_BUBBLES_END_INDEX; 
        i++)
  {
    t = &G_world_all_things[i];

    if (t->type == THING_TYPE_NONE)
      continue;

    /* determine adjusted timer count */
    adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

    /* remove popped bubble if necessary */
    if ((t->type == THING_TYPE_BUBBLE_POPPED) && 
        (adjusted_timer_count >= LIFETIME_BUBBLE_POPPED))
    {
      world_despawn_thing(i);
      continue;
    }

    /* change stressed bubble to popped if necessary */
    if ((t->type == THING_TYPE_BUBBLE_STRESSED) && 
        (adjusted_timer_count >= LIFETIME_BUBBLE_STRESSED))
    {
      pos_x = t->pos_x;
      pos_y = t->pos_y;

      world_despawn_thing(i);
      world_spawn_thing(THING_TYPE_BUBBLE_POPPED, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                        pos_x, pos_y, 0, 0, 240 - G_timer_count);

      /* play bubble popped sound */
      doremi_play_sfx(SFX_INDEX_BUBBLE_POPPED);

      continue;
    }

    /* move bubble */
    if ((t->type == THING_TYPE_BUBBLE_HORIZONTAL) || 
        (t->type == THING_TYPE_BUBBLE_VERTICAL))
    {
      /* if this bubble was already hit by something else, set it to pop */
      if (t->flag == THING_COLLISION_FLAG_BUBBLE_GOT_HIT)
      {
        pos_x = t->pos_x;
        pos_y = t->pos_y;

        world_despawn_thing(i);
        world_spawn_thing(THING_TYPE_BUBBLE_STRESSED, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                          pos_x, pos_y, 0, 0, 240 - G_timer_count);

        /* play bubble hit sound */
        doremi_play_sfx(SFX_INDEX_BUBBLE_HIT);

        continue;
      }

      /* move bubble horizontally or vertically */
      if (t->vel_x != 0)
        grid_move_thing(i, GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);
      else if (t->vel_y != 0)
        grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

      /* if there was a collision, change this to a stressed bubble */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        pos_x = t->pos_x;
        pos_y = t->pos_y;

        world_despawn_thing(i);
        world_spawn_thing(THING_TYPE_BUBBLE_STRESSED, COLOR_NONE, STATE_NONE, THING_ORIENT_NORMAL, 
                          pos_x, pos_y, 0, 0, 240 - G_timer_count);

        /* play bubble hit sound */
        doremi_play_sfx(SFX_INDEX_BUBBLE_HIT);

        continue;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_platforms_etc_check_for_tracks_and_stops()
*******************************************************************************/
short int logic_platforms_etc_check_for_tracks_and_stops()
{
  int i;

  int box_x;
  int box_y;

  grid_box* b;

  thing* t;

  int rider_indices[3];

  int cx;
  int cy;

  int amount_x;
  int amount_y;

  /* update platform velocities based on tracks */
  for ( i = WORLD_PLATFORMS_ETC_START_INDEX; 
        i < WORLD_PLATFORMS_ETC_END_INDEX; 
        i++)
  {
    t = &G_world_all_things[i];

    if (t->type == THING_TYPE_NONE)
      continue;

    /* if this is an elevator, continue */
    if ((t->type == THING_TYPE_ELEVATOR_HORI_RIGHT) || 
        (t->type == THING_TYPE_ELEVATOR_HORI_LEFT)  || 
        (t->type == THING_TYPE_ELEVATOR_VERT_UP)    || 
        (t->type == THING_TYPE_ELEVATOR_VERT_DOWN))
    {
      continue;
    }

    /* clear awaiting snap to grid flag if necessary */
    if (t->state == STATE_AWAITING_SNAP_TO_GRID)
      t->state = STATE_NONE;

    /* determine grid box that contains this platform's center */
    box_x = t->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
    box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

    /* wraparound box indices */
    box_x = (box_x + GRID_WIDTH) % GRID_WIDTH;
    box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

    b = &G_collision_grid[(box_y * GRID_WIDTH) + box_x];

    /* make sure there is a relevant track or stop here */
    if ((t->type == THING_TYPE_BASS_KNOB) && 
        (b->back_object != GRID_OBJECT_BASS_KNOB_STOP))
    {
      continue;
    }

    if ((t->type == THING_TYPE_TREBLE_KNOB) && 
        (b->back_object != GRID_OBJECT_TREBLE_KNOB_STOP))
    {
      continue;
    }

    if ((t->type == THING_TYPE_ROCKET_BLOCK_RIGHT) && 
        (b->back_object != GRID_OBJECT_ROCKET_BLOCK_RIGHT_STOP))
    {
      continue;
    }

    if ((t->type == THING_TYPE_ROCKET_BLOCK_LEFT) && 
        (b->back_object != GRID_OBJECT_ROCKET_BLOCK_LEFT_STOP))
    {
      continue;
    }

    if ((t->type == THING_TYPE_DROP_BOLT_DOWN) && 
        (b->back_object != GRID_OBJECT_DROP_BOLT_DOWN_STOP))
    {
      continue;
    }

    if ((t->type == THING_TYPE_DROP_BOLT_UP) && 
        (b->back_object != GRID_OBJECT_DROP_BOLT_UP_STOP))
    {
      continue;
    }

    if ((t->type == THING_TYPE_ANTI_BOLT_UP) && 
        (b->back_object != GRID_OBJECT_ANTI_BOLT_UP_STOP))
    {
      continue;
    }

    if ((t->type == THING_TYPE_ANTI_BOLT_DOWN) && 
        (b->back_object != GRID_OBJECT_ANTI_BOLT_DOWN_STOP))
    {
      continue;
    }

    if ((t->type == THING_TYPE_PLATFORM) && (t->vel_x > 0)    && 
        (b->back_object != GRID_OBJECT_TRACK_LEFT)            && 
        (b->back_object != GRID_OBJECT_TRACK_CORNER_UP_LEFT)  && 
        (b->back_object != GRID_OBJECT_TRACK_CORNER_DOWN_LEFT))
    {
      continue;
    }

    if ((t->type == THING_TYPE_PLATFORM) && (t->vel_x < 0)    && 
        (b->back_object != GRID_OBJECT_TRACK_RIGHT)           && 
        (b->back_object != GRID_OBJECT_TRACK_CORNER_UP_RIGHT) && 
        (b->back_object != GRID_OBJECT_TRACK_CORNER_DOWN_RIGHT))
    {
      continue;
    }

    if ((t->type == THING_TYPE_PLATFORM) && (t->vel_y > 0)    && 
        (b->back_object != GRID_OBJECT_TRACK_UP)              && 
        (b->back_object != GRID_OBJECT_TRACK_CORNER_UP_RIGHT) && 
        (b->back_object != GRID_OBJECT_TRACK_CORNER_UP_LEFT))
    {
      continue;
    }

    if ((t->type == THING_TYPE_PLATFORM) && (t->vel_y < 0)      && 
        (b->back_object != GRID_OBJECT_TRACK_DOWN)              && 
        (b->back_object != GRID_OBJECT_TRACK_CORNER_DOWN_LEFT)  && 
        (b->back_object != GRID_OBJECT_TRACK_CORNER_DOWN_RIGHT))
    {
      continue;
    }

    /* determine rider indices    */
    /* note that the bunny is the */
    /* only possible rider here   */
    rider_indices[0] = G_world_riders[i - WORLD_PLATFORMS_ETC_START_INDEX + WORLD_RIDERS_UNDER_PLATFORM_START_INDEX];
    rider_indices[1] = G_world_riders[i - WORLD_PLATFORMS_ETC_START_INDEX + WORLD_RIDERS_ON_PLATFORM_START_INDEX];

    if ((rider_indices[1] != -1) && (rider_indices[1] != WORLD_BUNNY_INDEX))
      rider_indices[2] = G_world_riders[rider_indices[1] - WORLD_CRITTERS_START_INDEX + WORLD_RIDERS_ON_CRITTER_START_INDEX];
    else
      rider_indices[2] = -1;

    /* compute box center position (in subpixels) */
    cx = ((box_x * GRID_BOX_SIZE) + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
    cy = ((box_y * GRID_BOX_SIZE) + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

    /* compute distance from center of grid box */
    amount_x = cx - t->pos_x;
    amount_y = cy - t->pos_y;

    /* check if a stop has been activated */
    if (((t->type == THING_TYPE_BASS_KNOB)          && (amount_y < 0)) || 
        ((t->type == THING_TYPE_TREBLE_KNOB)        && (amount_y > 0)) || 
        ((t->type == THING_TYPE_ROCKET_BLOCK_RIGHT) && (amount_x < 0)) || 
        ((t->type == THING_TYPE_ROCKET_BLOCK_LEFT)  && (amount_x > 0)) || 
        ((t->type == THING_TYPE_DROP_BOLT_DOWN)     && (amount_y < 0)) || 
        ((t->type == THING_TYPE_DROP_BOLT_UP)       && (amount_y > 0)) || 
        ((t->type == THING_TYPE_ANTI_BOLT_UP)       && (amount_y > 0)) || 
        ((t->type == THING_TYPE_ANTI_BOLT_DOWN)     && (amount_y < 0)))
    {
      continue;
    }

    /* check if a track has been activated */
    if (t->type == THING_TYPE_PLATFORM)
    {
      if (((t->vel_x < 0) && (amount_x < 0)) || 
          ((t->vel_x > 0) && (amount_x > 0)) || 
          ((t->vel_y < 0) && (amount_y < 0)) || 
          ((t->vel_y > 0) && (amount_y > 0)))
      {
        continue;
      }
    }

    /* try to snap to the grid if necessary */
    /* horizontal move */
    if (amount_x != 0)
    {
      grid_move_thing(i, GRID_MOVE_THING_MODE_HORIZONTAL, amount_x);

      /* check if there was a collision   */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        t->state = STATE_AWAITING_SNAP_TO_GRID;
        continue;
      }
      /* if there was no collision, move the riders */
      else
      {
        if (rider_indices[0] != -1)
          grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_HORIZONTAL, amount_x);

        if (rider_indices[1] != -1)
          grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_HORIZONTAL, amount_x);

        if (rider_indices[2] != -1)
          grid_move_thing(rider_indices[2], GRID_MOVE_THING_MODE_HORIZONTAL, amount_x);
      }
    }

    /* upward move */
    if (amount_y < 0)
    {
      /* move top riders */
      if (rider_indices[2] != -1)
        grid_move_thing(rider_indices[2], GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      if (rider_indices[1] != -1)
        grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      /* vertical move */
      grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      /* check if there was a collision */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        t->state = STATE_AWAITING_SNAP_TO_GRID;
        continue;
      }

      /* move bottom rider */
      if (rider_indices[0] != -1)
        grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, amount_y);
    }
    /* downward move */
    else if (amount_y > 0)
    {
      /* move bottom rider */
      if (rider_indices[0] != -1)
        grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      /* vertical move */
      grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      /* check if there was a collision */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        t->state = STATE_AWAITING_SNAP_TO_GRID;
        continue;
      }

      /* move top riders */
      if (rider_indices[1] != -1)
        grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      if (rider_indices[2] != -1)
        grid_move_thing(rider_indices[2], GRID_MOVE_THING_MODE_VERTICAL, amount_y);
    }

    /* set appropriate state and velocity */
    if ((t->type == THING_TYPE_BASS_KNOB)       || 
        (t->type == THING_TYPE_TREBLE_KNOB)     || 
        (t->type == THING_TYPE_DROP_BOLT_DOWN)  || 
        (t->type == THING_TYPE_DROP_BOLT_UP)    || 
        (t->type == THING_TYPE_ANTI_BOLT_UP)    || 
        (t->type == THING_TYPE_ANTI_BOLT_DOWN))
    {
      t->state = STATE_RETRACTING_STOPPED;
      t->vel_y = 0;
    }
    else if ( (t->type == THING_TYPE_ROCKET_BLOCK_RIGHT) || 
              (t->type == THING_TYPE_ROCKET_BLOCK_LEFT))
    {
      t->state = STATE_RETRACTING_STOPPED;
      t->vel_x = 0;
    }
    else if (t->type == THING_TYPE_PLATFORM)
    {
      if (b->back_object == GRID_OBJECT_TRACK_RIGHT)
      {
        t->vel_x = THING_PLATFORM_VEL;
        t->vel_y = 0;
      }
      else if (b->back_object == GRID_OBJECT_TRACK_LEFT)
      {
        t->vel_x = -THING_PLATFORM_VEL;
        t->vel_y = 0;
      }
      else if (b->back_object == GRID_OBJECT_TRACK_UP)
      {
        t->vel_x = 0;
        t->vel_y = -THING_PLATFORM_VEL;
      }
      else if (b->back_object == GRID_OBJECT_TRACK_DOWN)
      {
        t->vel_x = 0;
        t->vel_y = THING_PLATFORM_VEL;
      }
      else if (b->back_object == GRID_OBJECT_TRACK_CORNER_UP_RIGHT)
      {
        if (t->vel_x < 0)
        {
          t->vel_x = 0;
          t->vel_y = -THING_PLATFORM_VEL;
        }
        else if (t->vel_y > 0)
        {
          t->vel_x = THING_PLATFORM_VEL;
          t->vel_y = 0;
        }
      }
      else if (b->back_object == GRID_OBJECT_TRACK_CORNER_UP_LEFT)
      {
        if (t->vel_x > 0)
        {
          t->vel_x = 0;
          t->vel_y = -THING_PLATFORM_VEL;
        }
        else if (t->vel_y > 0)
        {
          t->vel_x = -THING_PLATFORM_VEL;
          t->vel_y = 0;
        }
      }
      else if (b->back_object == GRID_OBJECT_TRACK_CORNER_DOWN_LEFT)
      {
        if (t->vel_x > 0)
        {
          t->vel_x = 0;
          t->vel_y = THING_PLATFORM_VEL;
        }
        else if (t->vel_y < 0)
        {
          t->vel_x = -THING_PLATFORM_VEL;
          t->vel_y = 0;
        }
      }
      else if (b->back_object == GRID_OBJECT_TRACK_CORNER_DOWN_RIGHT)
      {
        if (t->vel_x < 0)
        {
          t->vel_x = 0;
          t->vel_y = THING_PLATFORM_VEL;
        }
        else if (t->vel_y < 0)
        {
          t->vel_x = THING_PLATFORM_VEL;
          t->vel_y = 0;
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_arrows_activate()
*******************************************************************************/
short int logic_arrows_activate()
{
  int i;

  int box_x;
  int box_y;

  grid_box* b;

  thing* t;

  int rider_indices[2];

  int cx;
  int cy;

  int amount_x;
  int amount_y;

  /* activate arrows */
  for ( i = WORLD_THROWN_THINGS_START_INDEX; 
        i < WORLD_THROWN_THINGS_END_INDEX; 
        i++)
  {
    t = &G_world_all_things[i];

    if (t->type == THING_TYPE_NONE)
      continue;

    /* if this thing is landing this frame, continue */
    if (t->state == STATE_THROWN_THING_LANDED)
      continue;

    /* clear awaiting snap to grid flag if necessary */
    if (t->state == STATE_AWAITING_SNAP_TO_GRID)
      t->state = STATE_NONE;

    /* determine grid box that contains this thrown thing's center */
    box_x = t->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
    box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

    /* wraparound box indices */
    box_x = (box_x + GRID_WIDTH) % GRID_WIDTH;
    box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

    b = &G_collision_grid[(box_y * GRID_WIDTH) + box_x];

    /* make sure there is an arrow here */
    if ((b->back_object != GRID_OBJECT_ARROWS_000) && 
        (b->back_object != GRID_OBJECT_ARROWS_045) && 
        (b->back_object != GRID_OBJECT_ARROWS_090) && 
        (b->back_object != GRID_OBJECT_ARROWS_135) && 
        (b->back_object != GRID_OBJECT_ARROWS_180) && 
        (b->back_object != GRID_OBJECT_ARROWS_225) && 
        (b->back_object != GRID_OBJECT_ARROWS_270) && 
        (b->back_object != GRID_OBJECT_ARROWS_315))
    {
      continue;
    }

    /* make sure this thing can be zinged */
    if ((t->type != THING_TYPE_THROWN_MARBLE)           && 
        (t->type != THING_TYPE_THROWN_CRITTER_WALKER_1) && 
        (t->type != THING_TYPE_THROWN_CRITTER_WALKER_2) && 
        (t->type != THING_TYPE_THROWN_CRITTER_FLYER_1)  && 
        (t->type != THING_TYPE_THROWN_CRITTER_FLYER_2))
    {
      continue;
    }

    /* make sure this is not a flashing or changing marble            */
    /* (otherwise, we could not set the awaiting snap to grid state)  */
    if ((t->state == STATE_MARBLE_MATCHED)          || 
        (t->state == STATE_MARBLE_READY_TO_CHANGE)  || 
        (t->state == STATE_MARBLE_FLASHING)         || 
        (t->state == STATE_MARBLE_CHANGING))
    {
      continue;
    }

    /* if this thing is falling, skip it */
    if ((t->vel_x == THING_THROWN_FALLING_VEL) && (t->vel_y == 0))
      continue;

    if ((t->vel_x == -THING_THROWN_FALLING_VEL) && (t->vel_y == 0))
      continue;

    if ((t->vel_x == 0) && (t->vel_y == THING_THROWN_FALLING_VEL))
      continue;

    if ((t->vel_x == 0) && (t->vel_y == -THING_THROWN_FALLING_VEL))
      continue;

    /* if we are already moving in the direction of this arrow, ignore it */
    if ((b->back_object == GRID_OBJECT_ARROWS_000)  && 
        (t->vel_x == THING_THROWN_STRAIGHT_VEL)     && 
        (t->vel_y == 0))
    {
      continue;
    }

    if ((b->back_object == GRID_OBJECT_ARROWS_045)  && 
        (t->vel_x == THING_THROWN_DIAGONAL_VEL)     && 
        (t->vel_y == -THING_THROWN_DIAGONAL_VEL))
    {
      continue;
    }

    if ((b->back_object == GRID_OBJECT_ARROWS_090)  && 
        (t->vel_x == 0)                             && 
        (t->vel_y == -THING_THROWN_STRAIGHT_VEL))
    {
      continue;
    }

    if ((b->back_object == GRID_OBJECT_ARROWS_135)  && 
        (t->vel_x == -THING_THROWN_DIAGONAL_VEL)    && 
        (t->vel_y == -THING_THROWN_DIAGONAL_VEL))
    {
      continue;
    }

    if ((b->back_object == GRID_OBJECT_ARROWS_180)  && 
        (t->vel_x == -THING_THROWN_STRAIGHT_VEL)    && 
        (t->vel_y == 0))
    {
      continue;
    }

    if ((b->back_object == GRID_OBJECT_ARROWS_225)  && 
        (t->vel_x == -THING_THROWN_DIAGONAL_VEL)    && 
        (t->vel_y == THING_THROWN_DIAGONAL_VEL))
    {
      continue;
    }

    if ((b->back_object == GRID_OBJECT_ARROWS_270)  && 
        (t->vel_x == 0)                             && 
        (t->vel_y == THING_THROWN_STRAIGHT_VEL))
    {
      continue;
    }

    if ((b->back_object == GRID_OBJECT_ARROWS_315)  && 
        (t->vel_x == THING_THROWN_DIAGONAL_VEL)     && 
        (t->vel_y == THING_THROWN_DIAGONAL_VEL))
    {
      continue;
    }

    /* determine rider indices    */
    /* note that the bunny is the */
    /* only possible rider here   */
    rider_indices[0] = -1;
    rider_indices[1] = G_world_riders[i - WORLD_THROWN_THINGS_START_INDEX + WORLD_RIDERS_ON_THROWN_THING_START_INDEX];

    if ((rider_indices[1] == WORLD_BUNNY_INDEX) && (G_bunny_gravity == BUNNY_GRAVITY_UP))
    {
      rider_indices[0] = WORLD_BUNNY_INDEX;
      rider_indices[1] = -1;
    }

    /* compute box center position (in subpixels) */
    cx = ((box_x * GRID_BOX_SIZE) + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;
    cy = ((box_y * GRID_BOX_SIZE) + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

    /* compute distance from center of grid box */
    amount_x = cx - t->pos_x;
    amount_y = cy - t->pos_y;

    /* if this thing is moving left or right, */
    /* check if it has crossed the vertical   */
    /* line through the center of this box.   */
    if (((t->vel_x > 0) && (t->vel_y == 0) && (amount_x > 0)) || 
        ((t->vel_x < 0) && (t->vel_y == 0) && (amount_x < 0)))
    {
      continue;
    }

    /* if this thing is moving up or down,    */
    /* check if it has crossed the horizontal */
    /* line through the center of this box.   */
    if (((t->vel_x == 0) && (t->vel_y > 0) && (amount_y > 0)) || 
        ((t->vel_x == 0) && (t->vel_y < 0) && (amount_y < 0)))
    {
      continue;
    }

    /* if this thing is moving diagonally,  */
    /* check if it has crossed the diagonal */
    /* line through the center of this box  */
    /* orthogonal to its path of motion.    */
    if (((t->vel_x > 0) && (t->vel_y > 0) && (amount_x + amount_y > 0)) || 
        ((t->vel_x < 0) && (t->vel_y < 0) && (amount_x + amount_y < 0)))
    {
      continue;
    }

    if (((t->vel_x < 0) && (t->vel_y > 0) && (amount_x - amount_y < 0)) || 
        ((t->vel_x > 0) && (t->vel_y < 0) && (amount_x - amount_y > 0)))
    {
      continue;
    }

    /* if arrow is activated, clear potential landing state if necessary */
    if (t->state == STATE_THROWN_THING_MAY_HAVE_LANDED)
      t->state = STATE_NONE;

    /* if arrow is activated, snap to grid and fire off in that direction */
    /* horizontal move */
    if (amount_x != 0)
    {
      grid_move_thing(i, GRID_MOVE_THING_MODE_HORIZONTAL, amount_x);

      /* check if there was a collision   */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        t->state = STATE_AWAITING_SNAP_TO_GRID;
        continue;
      }
      /* if there was no collision, move the riders */
      else
      {
        if (rider_indices[0] != -1)
          grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_HORIZONTAL, amount_x);

        if (rider_indices[1] != -1)
          grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_HORIZONTAL, amount_x);
      }
    }

    /* upward move */
    if (amount_y < 0)
    {
      /* move top rider */
      if (rider_indices[1] != -1)
        grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      /* vertical move */
      grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      /* check if there was a collision */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        t->state = STATE_AWAITING_SNAP_TO_GRID;
        continue;
      }

      /* move bottom rider */
      if (rider_indices[0] != -1)
        grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, amount_y);
    }
    /* downward move */
    else if (amount_y > 0)
    {
      /* move bottom rider */
      if (rider_indices[0] != -1)
        grid_move_thing(rider_indices[0], GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      /* vertical move */
      grid_move_thing(i, GRID_MOVE_THING_MODE_VERTICAL, amount_y);

      /* check if there was a collision */
      if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
      {
        t->state = STATE_AWAITING_SNAP_TO_GRID;
        continue;
      }

      /* move top rider */
      if (rider_indices[1] != -1)
        grid_move_thing(rider_indices[1], GRID_MOVE_THING_MODE_VERTICAL, amount_y);
    }

    /* set new velocity */
    if (b->back_object == GRID_OBJECT_ARROWS_000)
    {
      t->vel_x = THING_THROWN_STRAIGHT_VEL;
      t->vel_y = 0;
    }
    else if (b->back_object == GRID_OBJECT_ARROWS_045)
    {
      t->vel_x = THING_THROWN_DIAGONAL_VEL;
      t->vel_y = -THING_THROWN_DIAGONAL_VEL;
    }
    else if (b->back_object == GRID_OBJECT_ARROWS_090)
    {
      t->vel_x = 0;
      t->vel_y = -THING_THROWN_STRAIGHT_VEL;
    }
    else if (b->back_object == GRID_OBJECT_ARROWS_135)
    {
      t->vel_x = -THING_THROWN_DIAGONAL_VEL;
      t->vel_y = -THING_THROWN_DIAGONAL_VEL;
    }
    else if (b->back_object == GRID_OBJECT_ARROWS_180)
    {
      t->vel_x = -THING_THROWN_STRAIGHT_VEL;
      t->vel_y = 0;
    }
    else if (b->back_object == GRID_OBJECT_ARROWS_225)
    {
      t->vel_x = -THING_THROWN_DIAGONAL_VEL;
      t->vel_y = THING_THROWN_DIAGONAL_VEL;
    }
    else if (b->back_object == GRID_OBJECT_ARROWS_270)
    {
      t->vel_x = 0;
      t->vel_y = THING_THROWN_STRAIGHT_VEL;
    }
    else if (b->back_object == GRID_OBJECT_ARROWS_315)
    {
      t->vel_x = THING_THROWN_DIAGONAL_VEL;
      t->vel_y = THING_THROWN_DIAGONAL_VEL;
    }

    /* play arrows sound */
    doremi_play_sfx(SFX_INDEX_THROWN_THING_ARROWS);
  }

  return 0;
}

/*******************************************************************************
** logic_land_thrown_things()
*******************************************************************************/
short int logic_land_thrown_things()
{
  int i;

  thing* t;

  grid_box* b;

  int box_x;
  int box_y;

  int type;
  int orient;

  int pos_x;
  int pos_y;

  int vel_x;
  int vel_y;

  /* land thrown things */
  for ( i = WORLD_THROWN_THINGS_START_INDEX; 
        i < WORLD_THROWN_THINGS_END_INDEX; 
        i++)
  {
    t = &G_world_all_things[i];

    if (t->type == THING_TYPE_NONE)
      continue;

    /* see if this thing is landing! */
    if ((t->state == STATE_THROWN_THING_LANDED) || 
        (t->state == STATE_THROWN_THING_MAY_HAVE_LANDED))
    {
      /* reset state */
      t->state = STATE_NONE;

      if (t->type == THING_TYPE_THROWN_MARBLE)
      {
        /* set to a falling marble */
        if (G_marble_gravity == MARBLE_GRAVITY_DOWN)
        {
          t->vel_x = 0;
          t->vel_y = THING_THROWN_FALLING_VEL;
        }
        else if (G_marble_gravity == MARBLE_GRAVITY_UP)
        {
          t->vel_x = 0;
          t->vel_y = -THING_THROWN_FALLING_VEL;
        }
        else if (G_marble_gravity == MARBLE_GRAVITY_RIGHT)
        {
          t->vel_x = THING_THROWN_FALLING_VEL;
          t->vel_y = 0;
        }
        else if (G_marble_gravity == MARBLE_GRAVITY_LEFT)
        {
          t->vel_x = -THING_THROWN_FALLING_VEL;
          t->vel_y = 0;
        }
        else
        {
          t->vel_x = 0;
          t->vel_y = THING_THROWN_FALLING_VEL;
        }

        /* compute grid box that contains this marble's center */
        box_x = t->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
        box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        /* wraparound box indices */
        box_x = (box_x + GRID_WIDTH) % GRID_WIDTH;
        box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

        b = &G_collision_grid[(box_y * GRID_WIDTH) + box_x];

        /* try to snap to the grid */
        /* note that we check for num_things being 1, because */
        /* the marble is already a thing in this grid box     */
        if ((b->front_object == GRID_OBJECT_NONE) && 
            (b->num_things == 1))
        {
          /* insert marble into the grid */
          b->front_object = GRID_OBJECT_MARBLE;
          b->front_color = t->color;
          b->front_state = t->state;
          b->front_timer_offset = 0;

          /* despawn thing */
          world_despawn_thing(i);

          /* set flag to redraw the grid this frame */
          G_flag_redraw_grid_objects = 1;

          /* check for matches this frame */
          logic_marbles_check_for_matches();

          /* play landing sound */
          doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
        }
      }
      else if ( (t->type == THING_TYPE_THROWN_CRITTER_WALKER_1) || 
                (t->type == THING_TYPE_THROWN_CRITTER_WALKER_2) || 
                (t->type == THING_TYPE_THROWN_CRITTER_FLYER_1)  || 
                (t->type == THING_TYPE_THROWN_CRITTER_FLYER_2))
      {
        if (t->type == THING_TYPE_THROWN_CRITTER_WALKER_1)
          type = THING_TYPE_CRITTER_WALKER_1;
        else if (t->type == THING_TYPE_THROWN_CRITTER_WALKER_2)
          type = THING_TYPE_CRITTER_WALKER_2;
        else if (t->type == THING_TYPE_THROWN_CRITTER_FLYER_1)
          type = THING_TYPE_CRITTER_FLYER_1;
        else if (t->type == THING_TYPE_THROWN_CRITTER_FLYER_2)
          type = THING_TYPE_CRITTER_FLYER_2;
        else
          type = THING_TYPE_CRITTER_WALKER_1;

        pos_x = t->pos_x;
        pos_y = t->pos_y;

        if ((t->type == THING_TYPE_THROWN_CRITTER_WALKER_1) || 
            (t->type == THING_TYPE_THROWN_CRITTER_WALKER_2) || 
            (t->type == THING_TYPE_THROWN_CRITTER_FLYER_1))
        {
          if (t->vel_x == 0)
          {
            if ((t->orient == THING_ORIENT_NORMAL) || (t->orient == THING_ORIENT_FLIP_VERT))
            {
              vel_x = THING_CRITTER_VEL;
              orient = THING_ORIENT_NORMAL;
            }
            else
            {
              vel_x = -THING_CRITTER_VEL;
              orient = THING_ORIENT_FLIP_HORI;
            }
          }
          else if (t->vel_x > 0)
          {
            vel_x = THING_CRITTER_VEL;
            orient = THING_ORIENT_NORMAL;
          }
          else
          {
            vel_x = -THING_CRITTER_VEL;
            orient = THING_ORIENT_FLIP_HORI;
          }

          vel_y = 0;
        }
        else if (t->type == THING_TYPE_THROWN_CRITTER_FLYER_2)
        {
          vel_x = 0;

          if ((t->orient == THING_ORIENT_NORMAL) || (t->orient == THING_ORIENT_FLIP_VERT))
          {
            vel_y = THING_CRITTER_VEL;
            orient = THING_ORIENT_NORMAL;
          }
          else
          {
            vel_y = -THING_CRITTER_VEL;
            orient = THING_ORIENT_FLIP_HORI;
          }
        }

        /* despawn thrown critter, respawn as a normal critter */
        world_despawn_thing(i);
        world_spawn_thing(type, COLOR_NONE, STATE_CRITTER_STUNNED, orient, 
                          pos_x, pos_y, vel_x, vel_y, 240 - G_timer_count);

        /* play landing sound */
        doremi_play_sfx(SFX_INDEX_THROWN_THING_LANDED);
      }
    }
  }

  return 0;
}

/*******************************************************************************
** logic_update_held_thing()
*******************************************************************************/
short int logic_update_held_thing()
{
  thing* t;

  t = &G_world_all_things[WORLD_HELD_THING_INDEX];

  if (t->type == THING_TYPE_NONE)
    return 0;

  /* if the bunny is currently grabbing this thing,   */
  /* update its position using its own set velocity.  */
  if (G_bunny_action == BUNNY_ACTION_GRABBING)
  {
    t->pos_y += t->vel_y;
  }
  /* otherwise, set its position to directly above    */
  /* (or below) the bunny, based on the bunny gravity */
  else
  {
    t->pos_x = G_world_all_things[WORLD_BUNNY_INDEX].pos_x;
    t->pos_y = G_world_all_things[WORLD_BUNNY_INDEX].pos_y;

    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
      t->pos_y -= 16 * THING_NUM_SUBPIXELS;
    else
      t->pos_y += 16 * THING_NUM_SUBPIXELS;
  }

  /* if this is a held critter, set the sprite flip orientation */
  if ((t->type == THING_TYPE_HELD_CRITTER_WALKER_1) || 
      (t->type == THING_TYPE_HELD_CRITTER_WALKER_2) || 
      (t->type == THING_TYPE_HELD_CRITTER_FLYER_1)  || 
      (t->type == THING_TYPE_HELD_CRITTER_FLYER_2))
  {
    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
    {
      if (G_bunny_facing == BUNNY_FACING_RIGHT)
        t->orient = THING_ORIENT_FLIP_VERT;
      else
        t->orient = THING_ORIENT_ROTATE_180;
    }
    else
    {
      if (G_bunny_facing == BUNNY_FACING_RIGHT)
        t->orient = THING_ORIENT_NORMAL;
      else
        t->orient = THING_ORIENT_FLIP_HORI;
    }
  }

  return 0;
}

/*******************************************************************************
** logic_update_frame()
*******************************************************************************/
short int logic_update_frame()
{
  /* update grid */
  if (G_timer_count % 6 == 0)
  {
    logic_doors_and_invisible_blocks_activate();
    logic_complete_grid_objects_animations();

    logic_pickup_spawners_update();
    logic_bubble_pots_update();

    logic_magi_blocks_update();
    logic_fast_magi_blocks_update();

    if (G_timer_count % 12 == 0)
    {
      logic_marbles_apply_gravity();
      logic_marbles_clear_completed_flashes_and_changes();
      logic_marbles_check_for_matches();
      logic_marbles_activate_flashes_and_changes();
    }
  }

  /* update world */
  world_init_riders();

  bunny_move_bunny();

  logic_collect_pickups();

  logic_move_critters();
  logic_move_platforms_etc();
  logic_move_thrown_things();
  logic_move_bubbles();

  logic_platforms_etc_check_for_tracks_and_stops();
  logic_arrows_activate();

  logic_land_thrown_things();

  logic_update_held_thing();

  return 0;
}

