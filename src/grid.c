/*******************************************************************************
** grid.c (spatial partitioning grid and collision checking)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "animate.h"
#include "color.h"
#include "global.h"
#include "grid.h"
#include "level.h"
#include "state.h"
#include "thing.h"
#include "world.h"

#define GRID_DETERMINE_OVERLAPPED_BOXES(t)                                                \
  /* determine corner positions (in subpixels) */                                         \
  corner_low_x  = t->pos_x - t->hx;                                                       \
  corner_high_x = t->pos_x + t->hx - 1;                                                   \
  corner_low_y  = t->pos_y - t->hy;                                                       \
  corner_high_y = t->pos_y + t->hy - 1;                                                   \
                                                                                          \
  /* wraparound corner positions */                                                       \
  corner_low_x  = (corner_low_x + GRID_WIDTH_IN_SUBPIXELS) % GRID_WIDTH_IN_SUBPIXELS;     \
  corner_high_x = (corner_high_x + GRID_WIDTH_IN_SUBPIXELS) % GRID_WIDTH_IN_SUBPIXELS;    \
  corner_low_y  = (corner_low_y + GRID_HEIGHT_IN_SUBPIXELS) % GRID_HEIGHT_IN_SUBPIXELS;   \
  corner_high_y = (corner_high_y + GRID_HEIGHT_IN_SUBPIXELS) % GRID_HEIGHT_IN_SUBPIXELS;  \
                                                                                          \
  /* determine boxes overlapped by this thing */                                          \
  box_low_x   = corner_low_x / GRID_BOX_SIZE_IN_SUBPIXELS;                                \
  box_high_x  = corner_high_x / GRID_BOX_SIZE_IN_SUBPIXELS;                               \
  box_low_y   = corner_low_y / GRID_BOX_SIZE_IN_SUBPIXELS;                                \
  box_high_y  = corner_high_y / GRID_BOX_SIZE_IN_SUBPIXELS;                               \
                                                                                          \
  /* wraparound box indices (should be unnecessary) */                                    \
  box_low_x   = (box_low_x + GRID_WIDTH_IN_BOXES) % GRID_WIDTH_IN_BOXES;                  \
  box_high_x  = (box_high_x + GRID_WIDTH_IN_BOXES) % GRID_WIDTH_IN_BOXES;                 \
  box_low_y   = (box_low_y + GRID_HEIGHT_IN_BOXES) % GRID_HEIGHT_IN_BOXES;                \
  box_high_y  = (box_high_y + GRID_HEIGHT_IN_BOXES) % GRID_HEIGHT_IN_BOXES;               \
                                                                                          \
  /* determine number of grid boxes in each direction */                                  \
  num_boxes_x = box_high_x - box_low_x + 1;                                               \
  num_boxes_y = box_high_y - box_low_y + 1;                                               \
                                                                                          \
  /* wraparound number of boxes */                                                        \
  num_boxes_x = (num_boxes_x + GRID_WIDTH_IN_BOXES) % GRID_WIDTH_IN_BOXES;                \
  num_boxes_y = (num_boxes_y + GRID_HEIGHT_IN_BOXES) % GRID_HEIGHT_IN_BOXES;

#define GRID_COLLISION_TEST_THINGS(t1, t2, shift_x, shift_y)                   \
  /* obtain x projections */                                                   \
  l[0] = t1->pos_x - t1->hx;                                                   \
  h[0] = t1->pos_x + t1->hx - 1;                                               \
                                                                               \
  l[1] = t2->pos_x - t2->hx;                                                   \
  h[1] = t2->pos_x + t2->hx - 1;                                               \
                                                                               \
  /* obtain y projections */                                                   \
  l[2] = t1->pos_y - t1->hy;                                                   \
  h[2] = t1->pos_y + t1->hy - 1;                                               \
                                                                               \
  l[3] = t2->pos_y - t2->hy;                                                   \
  h[3] = t2->pos_y + t2->hy - 1;                                               \
                                                                               \
  /* apply shift */                                                            \
  l[1] += shift_x * GRID_WIDTH_IN_SUBPIXELS;                                   \
  h[1] += shift_x * GRID_WIDTH_IN_SUBPIXELS;                                   \
  l[3] += shift_y * GRID_HEIGHT_IN_SUBPIXELS;                                  \
  h[3] += shift_y * GRID_HEIGHT_IN_SUBPIXELS;                                  \
                                                                               \
  /* check for collision */                                                    \
  if ((l[0] <= h[1]) && (l[1] <= h[0]) && (l[2] <= h[3]) && (l[3] <= h[2]))    \
    collision_check = 1;

#define GRID_COLLISION_TEST_BOXES(t1, m, n, shift_x, shift_y)                  \
  /* obtain x projections */                                                   \
  l[0] = t1->pos_x - t1->hx;                                                   \
  h[0] = t1->pos_x + t1->hx - 1;                                               \
                                                                               \
  l[1] = m * GRID_BOX_SIZE_IN_SUBPIXELS;                                       \
  h[1] = ((m + 1) * GRID_BOX_SIZE_IN_SUBPIXELS) - 1;                           \
                                                                               \
  /* obtain y projections */                                                   \
  l[2] = t1->pos_y - t1->hy;                                                   \
  h[2] = t1->pos_y + t1->hy - 1;                                               \
                                                                               \
  l[3] = n * GRID_BOX_SIZE_IN_SUBPIXELS;                                       \
  h[3] = ((n + 1) * GRID_BOX_SIZE_IN_SUBPIXELS) - 1;                           \
                                                                               \
  /* apply shift */                                                            \
  l[1] += shift_x * GRID_WIDTH_IN_SUBPIXELS;                                   \
  h[1] += shift_x * GRID_WIDTH_IN_SUBPIXELS;                                   \
  l[3] += shift_y * GRID_HEIGHT_IN_SUBPIXELS;                                  \
  h[3] += shift_y * GRID_HEIGHT_IN_SUBPIXELS;                                  \
                                                                               \
  /* check for collision */                                                    \
  if ((l[0] <= h[1]) && (l[1] <= h[0]) && (l[2] <= h[3]) && (l[3] <= h[2]))    \
    collision_check = 1;

grid_box G_collision_grid[GRID_NUM_BOXES];

/*******************************************************************************
** grid_reset()
*******************************************************************************/
short int grid_reset()
{
  int i;
  int k;

  for (i = 0; i < GRID_NUM_BOXES; i++)
  {
    /* reset objects */
    G_collision_grid[i].back_object = GRID_OBJECT_NONE;
    G_collision_grid[i].back_color  = COLOR_NONE;
    G_collision_grid[i].back_state  = STATE_NONE;
    G_collision_grid[i].back_timer_offset = 0;

    G_collision_grid[i].front_object = GRID_OBJECT_NONE;
    G_collision_grid[i].front_color  = COLOR_NONE;
    G_collision_grid[i].front_state  = STATE_NONE;
    G_collision_grid[i].front_timer_offset = 0;

    /* reset things */
    for (k = 0; k < GRID_BOX_MAX_THINGS; k++)
      G_collision_grid[i].thing_indices[k] = 0;

    G_collision_grid[i].num_things = 0;
  }

  return 0;
}

/*******************************************************************************
** grid_add_thing()
*******************************************************************************/
short int grid_add_thing(int index)
{
  thing*    t;
  grid_box* b;

  int corner_low_x;
  int corner_high_x;
  int corner_low_y;
  int corner_high_y;

  int box_low_x;
  int box_high_x;
  int box_low_y;
  int box_high_y;

  int num_boxes_x;
  int num_boxes_y;

  int m;
  int n;

  int i;
  int j;

  int k;
  int thing_i;
  int insert_i;

  if ((index < 0) || (index >= WORLD_MAX_THINGS))
    return 1;

  t = &G_world_all_things[index];

  /* determine boxes overlapped by this thing */
  GRID_DETERMINE_OVERLAPPED_BOXES(t)

  /* add thing to appropriate grid boxes */
  for (j = 0; j < num_boxes_y; j++)
  {
    for (i = 0; i < num_boxes_x; i++)
    {
      n = (box_low_y + j) % GRID_HEIGHT_IN_BOXES;
      m = (box_low_x + i) % GRID_WIDTH_IN_BOXES;

      b = &(G_collision_grid[(n * GRID_WIDTH_IN_BOXES) + m]);

      /* check if this thing is already in this grid box    */
      /* note that the indices should be sorted (ascending) */
      /* if it is not in this box, set the insert index     */
      thing_i = -1;
      insert_i = -1;

      for (k = 0; k < b->num_things; k++)
      {
        if (b->thing_indices[k] == index)
        {
          thing_i = k;
          break;
        }
        else if (b->thing_indices[k] > index)
        {
          insert_i = k;
          break;
        }
      }

      /* if this thing is already in this grid box, continue */
      if (thing_i != -1)
        continue;

      /* if an insert index was not found, insert this thing at the end */
      if (insert_i == -1)
      {
        b->thing_indices[b->num_things] = index;
        b->num_things += 1;
        continue;
      }

      /* add the thing to this grid box */
      if (b->num_things < GRID_BOX_MAX_THINGS - 1)
      {
        for (k = b->num_things - 1; k >= insert_i; k--)
          b->thing_indices[k + 1] = b->thing_indices[k];

        b->thing_indices[insert_i] = index;
        b->num_things += 1;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** grid_remove_thing()
*******************************************************************************/
short int grid_remove_thing(int index)
{
  thing*    t;
  grid_box* b;

  int corner_low_x;
  int corner_high_x;
  int corner_low_y;
  int corner_high_y;

  int box_low_x;
  int box_high_x;
  int box_low_y;
  int box_high_y;

  int num_boxes_x;
  int num_boxes_y;

  int m;
  int n;

  int i;
  int j;

  int k;
  int thing_i;

  if ((index < 0) || (index >= WORLD_MAX_THINGS))
    return 1;

  t = &G_world_all_things[index];

  /* determine boxes overlapped by this thing */
  GRID_DETERMINE_OVERLAPPED_BOXES(t)

  /* remove thing from appropriate grid boxes */
  for (j = 0; j < num_boxes_y; j++)
  {
    for (i = 0; i < num_boxes_x; i++)
    {
      n = (box_low_y + j) % GRID_HEIGHT_IN_BOXES;
      m = (box_low_x + i) % GRID_WIDTH_IN_BOXES;

      b = &(G_collision_grid[(n * GRID_WIDTH_IN_BOXES) + m]);

      /* find index of thing in this grid box */
      thing_i = -1;

      for (k = 0; k < b->num_things; k++)
      {
        if (b->thing_indices[k] == index)
        {
          thing_i = k;
          break;
        }
      }

      /* if this thing was not found in this grid box, continue */
      if (thing_i == -1)
        continue;

      /* remove this thing from this grid box */
      if (b->num_things > 0)
      {
        for (k = thing_i; k < b->num_things - 1; k++)
          b->thing_indices[k] = b->thing_indices[k + 1];

        b->num_things -= 1;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** grid_move_thing()
*******************************************************************************/
short int grid_move_thing(int index, int mode, int amount)
{
  thing*    t1;
  thing*    t2;
  grid_box* b;

  int corner_low_x;
  int corner_high_x;
  int corner_low_y;
  int corner_high_y;

  int box_low_x;
  int box_high_x;
  int box_low_y;
  int box_high_y;

  int num_boxes_x;
  int num_boxes_y;

  int m;
  int n;

  int i;
  int j;

  int k;

  int l[4];
  int h[4];

  int collision_check;
  int num_collisions;

  if ((index < 0) || (index >= WORLD_MAX_THINGS))
    return 1;

  t1 = &G_world_all_things[index];

  /* make sure the mode is valid */
  if ((mode != GRID_MOVE_THING_MODE_HORIZONTAL) && 
      (mode != GRID_MOVE_THING_MODE_VERTICAL)   && 
      (mode != GRID_MOVE_THING_MODE_TEST_POSITION))
  {
    return 0;
  }

  /* initialize collision flag */
  t1->flag = THING_COLLISION_FLAG_NONE;

  /* make sure a move is occuring here */
  if ((mode == GRID_MOVE_THING_MODE_HORIZONTAL) && (amount == 0))
    return 0;

  if ((mode == GRID_MOVE_THING_MODE_VERTICAL) && (amount == 0))
    return 0;

  /* remove thing from grid */
  if (mode != GRID_MOVE_THING_MODE_TEST_POSITION)
    grid_remove_thing(index);

  /* horizontal movement */
  if (mode == GRID_MOVE_THING_MODE_HORIZONTAL)
  {
    /* move thing */
    t1->pos_x += amount;

    /* wraparound thing position */
    t1->pos_x = (t1->pos_x + GRID_WIDTH_IN_SUBPIXELS) % GRID_WIDTH_IN_SUBPIXELS;
  }
  /* vertical movement */
  else if (mode == GRID_MOVE_THING_MODE_VERTICAL)
  {
    /* move thing */
    t1->pos_y += amount;

    /* wraparound thing position */
    t1->pos_y = (t1->pos_y + GRID_HEIGHT_IN_SUBPIXELS) % GRID_HEIGHT_IN_SUBPIXELS;
  }

  /* determine boxes overlapped by this thing */
  GRID_DETERMINE_OVERLAPPED_BOXES(t1)

  /* initialize number of collisions */
  num_collisions = 0;

  /* test thing position against other things */
  for (j = 0; j < num_boxes_y; j++)
  {
    for (i = 0; i < num_boxes_x; i++)
    {
      n = (box_low_y + j) % GRID_HEIGHT_IN_BOXES;
      m = (box_low_x + i) % GRID_WIDTH_IN_BOXES;

      b = &(G_collision_grid[(n * GRID_WIDTH_IN_BOXES) + m]);

      for (k = 0; k < b->num_things; k++)
      {
        t2 = &G_world_all_things[b->thing_indices[k]];

        /* iniitalize collision check */
        collision_check = 0;

        /* extra collision test for left / right wraparound */
        if (t2->pos_x + t2->hx - 1 >= GRID_WIDTH_IN_SUBPIXELS)
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, -1, 0)
        }
        else if (t2->pos_x - t2->hx < 0)
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 1, 0)
        }
        else if (t1->pos_x + t1->hx - 1 >= GRID_WIDTH_IN_SUBPIXELS)
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 1, 0)
        }
        else if (t1->pos_x - t1->hx < 0)
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, -1, 0)
        }

        /* extra collision test for top / bottom wraparound */
        if (t2->pos_y + t2->hy - 1 >= GRID_HEIGHT_IN_SUBPIXELS)
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 0, -1)
        }
        else if (t2->pos_y - t2->hy < 0)
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 0, 1)
        }
        else if (t1->pos_y + t1->hy - 1 >= GRID_HEIGHT_IN_SUBPIXELS)
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 0, 1)
        }
        else if (t1->pos_y - t1->hy < 0)
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 0, -1)
        }

        /* extra collision test for corner wraparound */
        if ((t2->pos_x + t2->hx - 1 >= GRID_WIDTH_IN_SUBPIXELS) && 
            (t2->pos_y + t2->hy - 1 >= GRID_HEIGHT_IN_SUBPIXELS))
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, -1, -1)
        }
        else if ( (t2->pos_x - t2->hx < 0) && 
                  (t2->pos_y + t2->hy - 1 >= GRID_HEIGHT_IN_SUBPIXELS))
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 1, -1)
        }
        else if ( (t2->pos_x + t2->hx - 1 >= GRID_WIDTH_IN_SUBPIXELS) && 
                  (t2->pos_y - t2->hy < 0))
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, -1, 1)
        }
        else if ( (t2->pos_x - t2->hx < 0) && 
                  (t2->pos_y - t2->hy < 0))
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 1, 1)
        }
        else if ( (t1->pos_x + t1->hx - 1 >= GRID_WIDTH_IN_SUBPIXELS) && 
                  (t1->pos_y + t1->hy - 1 >= GRID_HEIGHT_IN_SUBPIXELS))
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 1, 1)
        }
        else if ( (t1->pos_x - t1->hx < 0) && 
                  (t1->pos_y + t1->hy - 1 >= GRID_HEIGHT_IN_SUBPIXELS))
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, -1, 1)
        }
        else if ( (t1->pos_x + t1->hx - 1 >= GRID_WIDTH_IN_SUBPIXELS) && 
                  (t1->pos_y - t1->hy < 0))
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, 1, -1)
        }
        else if ( (t1->pos_x - t1->hx < 0) && 
                  (t1->pos_y - t1->hy < 0))
        {
          GRID_COLLISION_TEST_THINGS(t1, t2, -1, -1)
        }

        /* standard collision check */
        GRID_COLLISION_TEST_THINGS(t1, t2, 0, 0)

        /* if collision found, clamp to edge */
        if (collision_check == 1)
        {
          /* note bunny / pickup collisions         */
          /* we continue here because we don't want */
          /* the bunny to clamp onto the pickup     */
          if (t1->type == THING_TYPE_BUNNY)
          {
            if ((t2->type == THING_TYPE_CARROT)       || 
                (t2->type == THING_TYPE_BOOK)         || 
                (t2->type == THING_TYPE_MUSIC_NOTES)  || 
                (t2->type == THING_TYPE_MAGNET_DOWN)  || 
                (t2->type == THING_TYPE_MAGNET_UP)    || 
                (t2->type == THING_TYPE_MAGNET_RIGHT) || 
                (t2->type == THING_TYPE_MAGNET_LEFT)  || 
                (t2->type == THING_TYPE_GEM)          || 
                (t2->type == THING_TYPE_KEY)          || 
                (t2->type == THING_TYPE_MUSHROOM))
            {
              t2->flag = THING_COLLISION_FLAG_PICKUP_COLLECTED;
              continue;
            }
          }

          /* note bubble collisions (if this thing  */
          /* hits a bubble, set the bubble to pop)  */
          if ((t2->type == THING_TYPE_BUBBLE_HORIZONTAL)  || 
              (t2->type == THING_TYPE_BUBBLE_VERTICAL))
          {
            t2->flag = THING_COLLISION_FLAG_BUBBLE_GOT_HIT;
          }

          /* note rider collisions */
          if (mode == GRID_MOVE_THING_MODE_VERTICAL)
          {
            /* check if bunny landed on a critter, platform, or thrown thing */
            if (t1->type == THING_TYPE_BUNNY)
            {
              if ((G_bunny_action == BUNNY_ACTION_STANDING)     || 
                  (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
                  (G_bunny_action == BUNNY_ACTION_WALKING)      || 
                  (G_bunny_action == BUNNY_ACTION_DECELERATING) || 
                  (G_bunny_action == BUNNY_ACTION_FALLING)      || 
                  (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
              {
                if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && (amount > 0)) || 
                    ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && (amount < 0)))
                {
                  if ((t2->type == THING_TYPE_CRITTER_WALKER_1) || 
                      (t2->type == THING_TYPE_CRITTER_WALKER_2) || 
                      (t2->type == THING_TYPE_CRITTER_FLYER_1)  || 
                      (t2->type == THING_TYPE_CRITTER_FLYER_2))
                  {
                    G_world_riders[b->thing_indices[k] - WORLD_CRITTERS_START_INDEX + WORLD_RIDERS_ON_CRITTER_START_INDEX] = index;
                  }
                  else if ( (t2->type == THING_TYPE_BASS_KNOB)            || 
                            (t2->type == THING_TYPE_TREBLE_KNOB)          || 
                            (t2->type == THING_TYPE_ROCKET_BLOCK_RIGHT)   || 
                            (t2->type == THING_TYPE_ROCKET_BLOCK_LEFT)    || 
                            (t2->type == THING_TYPE_ELEVATOR_HORI_RIGHT)  || 
                            (t2->type == THING_TYPE_ELEVATOR_HORI_LEFT)   || 
                            (t2->type == THING_TYPE_ELEVATOR_VERT_UP)     || 
                            (t2->type == THING_TYPE_ELEVATOR_VERT_DOWN)   || 
                            (t2->type == THING_TYPE_DROP_BOLT_DOWN)       || 
                            (t2->type == THING_TYPE_DROP_BOLT_UP)         || 
                            (t2->type == THING_TYPE_ANTI_BOLT_UP)         || 
                            (t2->type == THING_TYPE_ANTI_BOLT_DOWN)       || 
                            (t2->type == THING_TYPE_PLATFORM))
                  {
                    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
                      G_world_riders[b->thing_indices[k] - WORLD_PLATFORMS_ETC_START_INDEX + WORLD_RIDERS_ON_PLATFORM_START_INDEX] = index;
                    else
                      G_world_riders[b->thing_indices[k] - WORLD_PLATFORMS_ETC_START_INDEX + WORLD_RIDERS_UNDER_PLATFORM_START_INDEX] = index;
                  }
                  else if ( (t2->type == THING_TYPE_THROWN_MARBLE)            || 
                            (t2->type == THING_TYPE_THROWN_CRITTER_WALKER_1)  || 
                            (t2->type == THING_TYPE_THROWN_CRITTER_WALKER_2)  || 
                            (t2->type == THING_TYPE_THROWN_CRITTER_FLYER_1)   || 
                            (t2->type == THING_TYPE_THROWN_CRITTER_FLYER_2))
                  {
                    G_world_riders[b->thing_indices[k] - WORLD_THROWN_THINGS_START_INDEX + WORLD_RIDERS_ON_THROWN_THING_START_INDEX] = index;
                  }
                }
              }
            }

            /* check if a critter landed on a platform */
            if ((t1->type == THING_TYPE_CRITTER_WALKER_1) || 
                (t1->type == THING_TYPE_CRITTER_WALKER_2))
            {
              if ((t2->type == THING_TYPE_BASS_KNOB)            || 
                  (t2->type == THING_TYPE_TREBLE_KNOB)          || 
                  (t2->type == THING_TYPE_ROCKET_BLOCK_RIGHT)   || 
                  (t2->type == THING_TYPE_ROCKET_BLOCK_LEFT)    || 
                  (t2->type == THING_TYPE_ELEVATOR_HORI_RIGHT)  || 
                  (t2->type == THING_TYPE_ELEVATOR_HORI_LEFT)   || 
                  (t2->type == THING_TYPE_ELEVATOR_VERT_UP)     || 
                  (t2->type == THING_TYPE_ELEVATOR_VERT_DOWN)   || 
                  (t2->type == THING_TYPE_DROP_BOLT_DOWN)       || 
                  (t2->type == THING_TYPE_DROP_BOLT_UP)         || 
                  (t2->type == THING_TYPE_ANTI_BOLT_UP)         || 
                  (t2->type == THING_TYPE_ANTI_BOLT_DOWN)       || 
                  (t2->type == THING_TYPE_PLATFORM))
              {
                G_world_riders[b->thing_indices[k] - WORLD_PLATFORMS_ETC_START_INDEX + WORLD_RIDERS_ON_PLATFORM_START_INDEX] = index;
              }
            }

            /* check if a vertical flying critter flew into the     */
            /* bunny (this is to handle the case where the critter  */
            /* sometimes turns around when you try to jump on it)   */
            /* note that it only happens when the bunny is not      */
            /* currently a rider on this critter.                   */
            if (t1->type == THING_TYPE_CRITTER_FLYER_2)
            {
              if (t2->type == THING_TYPE_BUNNY)
              {
                if ((G_bunny_action == BUNNY_ACTION_STANDING)     || 
                    (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
                    (G_bunny_action == BUNNY_ACTION_WALKING)      || 
                    (G_bunny_action == BUNNY_ACTION_DECELERATING) || 
                    (G_bunny_action == BUNNY_ACTION_FALLING)      || 
                    (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
                {
                  if (((t1->vel_y < 0) && (G_bunny_gravity == BUNNY_GRAVITY_DOWN)) || 
                      ((t1->vel_y > 0) && (G_bunny_gravity == BUNNY_GRAVITY_UP)))
                  {
                    if (G_world_riders[index - WORLD_CRITTERS_START_INDEX + WORLD_RIDERS_ON_CRITTER_START_INDEX] == -1)
                      t1->state = STATE_CRITTER_NEGATE_TURN;
                  }
                }
              }
            }

            /* check if a vertically moving thrown thing moved  */
            /* into the bunny (this is to handle a similar case */
            /* to the above, where the thrown thing bounces off */
            /* the bunny's feet as she tries to jump on it).    */
            if ((t1->type == THING_TYPE_THROWN_MARBLE)            || 
                (t1->type == THING_TYPE_THROWN_CRITTER_WALKER_1)  || 
                (t1->type == THING_TYPE_THROWN_CRITTER_WALKER_2)  || 
                (t1->type == THING_TYPE_THROWN_CRITTER_FLYER_1)   || 
                (t1->type == THING_TYPE_THROWN_CRITTER_FLYER_2))
            {
              if (t2->type == THING_TYPE_BUNNY)
              {
                if ((G_bunny_action == BUNNY_ACTION_STANDING)     || 
                    (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
                    (G_bunny_action == BUNNY_ACTION_WALKING)      || 
                    (G_bunny_action == BUNNY_ACTION_DECELERATING) || 
                    (G_bunny_action == BUNNY_ACTION_FALLING)      || 
                    (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
                {
                  if (((t1->vel_y < 0) && (G_bunny_gravity == BUNNY_GRAVITY_DOWN)) || 
                      ((t1->vel_y > 0) && (G_bunny_gravity == BUNNY_GRAVITY_UP)))
                  {
                    if (G_world_riders[index - WORLD_THROWN_THINGS_START_INDEX + WORLD_RIDERS_ON_THROWN_THING_START_INDEX] == -1)
                      t1->state = STATE_THROWN_THING_NEGATE_BOUNCE;
                  }
                }
              }
            }
          }

          /* add this collision */
          num_collisions += 1;

          /* clamp to edge */
          if (mode == GRID_MOVE_THING_MODE_HORIZONTAL)
          {
            /* the right edge of a thing is t->pos_x + t->hx - 1  */
            /* the left edge of a thing is t->pos_x - t->hx       */

            /* so, for t1 moving to the right, we want its right  */
            /* edge to be one less than the left edge of t2       */
            /* t1->pos_x + t1->hx - 1 = (t2->pos_x - t2->hx) - 1  */
            /* -> t1->pos_x = (t2->pos_x - t2->hx) - t1->hx       */

            /* and, for t1 moving to the left, we want its left   */
            /* edge to be one more than the right edge of t2      */
            /* t1->pos_x - t1->hx = (t2->pos_x + t2->hx - 1) + 1  */
            /* -> t1->pos_x = (t2->pos_x + t2->hx) + t1->hx       */
            if (amount > 0)
              t1->pos_x = t2->pos_x - t2->hx - t1->hx;
            else if (amount < 0)
              t1->pos_x = t2->pos_x + t2->hx + t1->hx;

            /* wraparound thing position */
            t1->pos_x = (t1->pos_x + GRID_WIDTH_IN_SUBPIXELS) % GRID_WIDTH_IN_SUBPIXELS;
          }
          else if (mode == GRID_MOVE_THING_MODE_VERTICAL)
          {
            /* the bottom edge of a thing is t->pos_y + t->hy - 1 */
            /* the top edge of a thing is t->pos_y - t->hy        */
            if (amount > 0)
              t1->pos_y = (t2->pos_y - t2->hy) - t1->hy;
            else if (amount < 0)
              t1->pos_y = (t2->pos_y + t2->hy) + t1->hy;

            /* wraparound thing position */
            t1->pos_y = (t1->pos_y + GRID_HEIGHT_IN_SUBPIXELS) % GRID_HEIGHT_IN_SUBPIXELS;
          }
        }
      }
    }
  }

  /* test thing position against front grid objects */
  for (j = 0; j < num_boxes_y; j++)
  {
    for (i = 0; i < num_boxes_x; i++)
    {
      n = (box_low_y + j) % GRID_HEIGHT_IN_BOXES;
      m = (box_low_x + i) % GRID_WIDTH_IN_BOXES;

      b = &(G_collision_grid[(n * GRID_WIDTH_IN_BOXES) + m]);

      /* see if there is an object to collide with here */
      if (b->front_object == GRID_OBJECT_NONE)
      {
        continue;
      }

      /* iniitalize collision check */
      collision_check = 0;

      /* extra collision test for left / right wraparound */
      if (t1->pos_x + t1->hx - 1 >= GRID_WIDTH_IN_SUBPIXELS)
      {
        GRID_COLLISION_TEST_BOXES(t1, m, n, 1, 0)
      }
      else if (t1->pos_x - t1->hx < 0)
      {
        GRID_COLLISION_TEST_BOXES(t1, m, n, -1, 0)
      }

      /* extra collision test for top / bottom wraparound */
      if (t1->pos_y + t1->hy - 1 >= GRID_HEIGHT_IN_SUBPIXELS)
      {
        GRID_COLLISION_TEST_BOXES(t1, m, n, 0, 1)
      }
      else if (t1->pos_y - t1->hy < 0)
      {
        GRID_COLLISION_TEST_BOXES(t1, m, n, 0, -1)
      }

      /* extra collision test for corner wraparound */
      if ((t1->pos_x + t1->hx - 1 >= GRID_WIDTH_IN_SUBPIXELS) && 
          (t1->pos_y + t1->hy - 1 >= GRID_HEIGHT_IN_SUBPIXELS))
      {
        GRID_COLLISION_TEST_BOXES(t1, m, n, 1, 1)
      }
      else if ( (t1->pos_x - t1->hx < 0) && 
                (t1->pos_y + t1->hy - 1 >= GRID_HEIGHT_IN_SUBPIXELS))
      {
        GRID_COLLISION_TEST_BOXES(t1, m, n, -1, 1)
      }
      else if ( (t1->pos_x + t1->hx - 1 >= GRID_WIDTH_IN_SUBPIXELS) && 
                (t1->pos_y - t1->hy < 0))
      {
        GRID_COLLISION_TEST_BOXES(t1, m, n, 1, -1)
      }
      else if ( (t1->pos_x - t1->hx < 0) && 
                (t1->pos_y - t1->hy < 0))
      {
        GRID_COLLISION_TEST_BOXES(t1, m, n, -1, -1)
      }

      /* standard collision check */
      GRID_COLLISION_TEST_BOXES(t1, m, n, 0, 0)

      /* if collision found, clamp to edge */
      if (collision_check == 1)
      {
        num_collisions += 1;

        /* check if a thrown marble hit a marble in this grid space */
        if ((t1->type == THING_TYPE_THROWN_MARBLE) && 
            (b->front_object == GRID_OBJECT_MARBLE))
        {
          if (t1->state == STATE_NONE)
            t1->state = STATE_THROWN_MARBLE_EARLY_LANDING;
        }

        /* clamp to edge */
        if (mode == GRID_MOVE_THING_MODE_HORIZONTAL)
        {
          /* the right edge of a thing is t->pos_x + t->hx - 1  */
          /* the left edge of a thing is t->pos_x - t->hx       */
          if (amount > 0)
            t1->pos_x = (m * GRID_BOX_SIZE_IN_SUBPIXELS) - t1->hx;
          else if (amount < 0)
            t1->pos_x = ((m + 1) * GRID_BOX_SIZE_IN_SUBPIXELS) + t1->hx;

          /* wraparound thing position */
          t1->pos_x = (t1->pos_x + GRID_WIDTH_IN_SUBPIXELS) % GRID_WIDTH_IN_SUBPIXELS;
        }
        else if (mode == GRID_MOVE_THING_MODE_VERTICAL)
        {
          /* the bottom edge of a thing is t->pos_y + t->hy - 1 */
          /* the top edge of a thing is t->pos_y - t->hy        */
          if (amount > 0)
            t1->pos_y = (n * GRID_BOX_SIZE_IN_SUBPIXELS) - t1->hy;
          else if (amount < 0)
            t1->pos_y = ((n + 1) * GRID_BOX_SIZE_IN_SUBPIXELS) + t1->hy;

          /* wraparound thing position */
          t1->pos_y = (t1->pos_y + GRID_HEIGHT_IN_SUBPIXELS) % GRID_HEIGHT_IN_SUBPIXELS;
        }
      }
    }
  }

  /* if a collision occurred, set collision flag */
  if (num_collisions > 0)
  {
    t1->flag = THING_COLLISION_FLAG_WE_HIT_SOMETHING;
  }

  /* add thing back to grid */
  if (mode != GRID_MOVE_THING_MODE_TEST_POSITION)
    grid_add_thing(index);

  return 0;
}

/*******************************************************************************
** grid_load_objects_from_tilemap()
*******************************************************************************/
short int grid_load_objects_from_tilemap()
{
  int m;
  int n;

  unsigned char* room_tilemap;

  int tilemap_index;
  int grid_index;

  /* set room tilemap */
  if ((G_current_room >= 0) && (G_current_room < LEVEL_NUM_ROOMS))
    room_tilemap = &G_room_tilemaps[G_current_room][0];
  else
    room_tilemap = &G_room_tilemaps[0][0];

  /* reset grid */
  grid_reset();

  /* load room tilemap to grid */
  for (n = 0; n < LEVEL_ROOM_HEIGHT; n++)
  {
    for (m = 0; m < LEVEL_ROOM_WIDTH; m++)
    {
      tilemap_index = (n * LEVEL_ROOM_WIDTH) + m;
      grid_index = (n * GRID_WIDTH_IN_BOXES) + m;

      if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BOOK)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_BOOK_SPAWNER;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_METAL_BALL)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_MARBLE;
        G_collision_grid[grid_index].front_color = COLOR_METAL;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_WILD_CARD)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_MARBLE;
        G_collision_grid[grid_index].front_color = COLOR_WILD;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BLOCK_SINGLE)
        G_collision_grid[grid_index].front_object = GRID_OBJECT_BLOCK_SINGLE;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BLOCK_LEFT)
        G_collision_grid[grid_index].front_object = GRID_OBJECT_BLOCK_LEFT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BLOCK_RIGHT)
        G_collision_grid[grid_index].front_object = GRID_OBJECT_BLOCK_RIGHT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_LADDER)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_LADDER;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MARBLE_ORANGE)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_MARBLE;
        G_collision_grid[grid_index].front_color = COLOR_ORANGE;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MARBLE_PINK)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_MARBLE;
        G_collision_grid[grid_index].front_color = COLOR_PINK;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MARBLE_BLUE)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_MARBLE;
        G_collision_grid[grid_index].front_color = COLOR_BLUE;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MARBLE_GREEN)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_MARBLE;
        G_collision_grid[grid_index].front_color = COLOR_GREEN;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ICE_SINGLE)
        G_collision_grid[grid_index].front_object = GRID_OBJECT_ICE_SINGLE;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ICE_LEFT)
        G_collision_grid[grid_index].front_object = GRID_OBJECT_ICE_LEFT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ICE_RIGHT)
        G_collision_grid[grid_index].front_object = GRID_OBJECT_ICE_RIGHT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_HEART_BLOCK)
        G_collision_grid[grid_index].front_object = GRID_OBJECT_HEART_BLOCK;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_DIAMOND_BLOCK)
        G_collision_grid[grid_index].front_object = GRID_OBJECT_DIAMOND_BLOCK;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGNET_DOWN)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_MAGNET_DOWN_SPAWNER;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGNET_RIGHT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_MAGNET_RIGHT_SPAWNER;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGNET_LEFT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_MAGNET_LEFT_SPAWNER;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BASS_KNOB)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_BASS_KNOB_STOP;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_TREBLE_KNOB)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_TREBLE_KNOB_STOP;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ROCKET_BLOCK_RIGHT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ROCKET_BLOCK_RIGHT_STOP;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ROCKET_BLOCK_LEFT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ROCKET_BLOCK_LEFT_STOP;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_DROP_BOLT_DOWN)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_DROP_BOLT_DOWN_STOP;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_DROP_BOLT_UP)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_DROP_BOLT_UP_STOP;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ANTI_BOLT_UP)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ANTI_BOLT_UP_STOP;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ANTI_BOLT_DOWN)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ANTI_BOLT_DOWN_STOP;
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1010) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0101))
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_MAGI_SPAWNER_1_1;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1000) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0100) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0010) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0001))
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_MAGI_SPAWNER_1_3;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1100) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0110) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0011) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1001))
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_MAGI_SPAWNER_2_2;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0111) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1011) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1101) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1110))
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_MAGI_SPAWNER_3_1;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1010)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0101))
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_FAST_MAGI_SPAWNER_1_1;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1000)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0100)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0010)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0001))
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_FAST_MAGI_SPAWNER_1_3;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1100)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0110)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0011)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1001))
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_FAST_MAGI_SPAWNER_2_2;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0111)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1011)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1101)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1110))
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_FAST_MAGI_SPAWNER_3_1;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_DOOR_BLOCK_ORANGE)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_DOOR_BLOCK;
        G_collision_grid[grid_index].front_color = COLOR_ORANGE;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_DOOR_BLOCK_PINK)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_DOOR_BLOCK;
        G_collision_grid[grid_index].front_color = COLOR_PINK;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_DOOR_BLOCK_BLUE)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_DOOR_BLOCK;
        G_collision_grid[grid_index].front_color = COLOR_BLUE;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_DOOR_BLOCK_GREEN)
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_DOOR_BLOCK;
        G_collision_grid[grid_index].front_color = COLOR_GREEN;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_INVISIBLE_BLOCK_ORANGE)
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_INVISIBLE_BLOCK;
        G_collision_grid[grid_index].back_color = COLOR_ORANGE;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_INVISIBLE_BLOCK_PINK)
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_INVISIBLE_BLOCK;
        G_collision_grid[grid_index].back_color = COLOR_PINK;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_INVISIBLE_BLOCK_BLUE)
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_INVISIBLE_BLOCK;
        G_collision_grid[grid_index].back_color = COLOR_BLUE;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_INVISIBLE_BLOCK_GREEN)
      {
        G_collision_grid[grid_index].back_object = GRID_OBJECT_INVISIBLE_BLOCK;
        G_collision_grid[grid_index].back_color = COLOR_GREEN;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_1_UP) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_2_UP) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_3_UP))
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_BUBBLE_POT_UP;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_1_RIGHT)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_2_RIGHT)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_3_RIGHT))
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_BUBBLE_POT_RIGHT;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_1_DOWN) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_2_DOWN) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_3_DOWN))
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_BUBBLE_POT_DOWN;
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_1_LEFT) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_2_LEFT) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_3_LEFT))
      {
        G_collision_grid[grid_index].front_object = GRID_OBJECT_BUBBLE_POT_LEFT;
      }
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_TRACK_RIGHT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_TRACK_RIGHT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_TRACK_LEFT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_TRACK_LEFT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_TRACK_UP)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_TRACK_UP;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_TRACK_DOWN)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_TRACK_DOWN;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_TRACK_CORNER_TOP_RIGHT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_TRACK_CORNER_UP_RIGHT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_TRACK_CORNER_TOP_LEFT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_TRACK_CORNER_UP_LEFT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_TRACK_CORNER_BOTTOM_LEFT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_TRACK_CORNER_DOWN_LEFT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_TRACK_CORNER_BOTTOM_RIGHT)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_TRACK_CORNER_DOWN_RIGHT;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ARROWS_000)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ARROWS_000;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ARROWS_045)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ARROWS_045;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ARROWS_090)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ARROWS_090;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ARROWS_135)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ARROWS_135;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ARROWS_180)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ARROWS_180;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ARROWS_225)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ARROWS_225;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ARROWS_270)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ARROWS_270;
      else if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ARROWS_315)
        G_collision_grid[grid_index].back_object = GRID_OBJECT_ARROWS_315;

      /* set timer offsets */
      if (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ICE_SINGLE)
      {
        G_collision_grid[grid_index].front_timer_offset = (240 - ((m * 12) % ANIM_ICE_TOTAL_LENGTH));
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ICE_LEFT)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_ICE_RIGHT))
      {
        G_collision_grid[grid_index].front_timer_offset = (240 - ((n * 12) % ANIM_ICE_TOTAL_LENGTH));
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0101) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0100) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0110) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0111))
      {
        G_collision_grid[grid_index].back_timer_offset = (240 - 60);
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0010) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0011) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1011))
      {
        G_collision_grid[grid_index].back_timer_offset = (240 - 120);
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_0001) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1001) || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_MAGI_BLOCK_1101))
      {
        G_collision_grid[grid_index].back_timer_offset = (240 - 180);
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0101)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0100)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0110)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0111))
      {
        G_collision_grid[grid_index].back_timer_offset = (240 - 30);
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0010)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0011)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1011))
      {
        G_collision_grid[grid_index].back_timer_offset = (240 - 60);
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0001)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1001)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1101))
      {
        G_collision_grid[grid_index].back_timer_offset = (240 - 90);
      }
      else if ( (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_2_UP)     || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_2_RIGHT)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_2_DOWN)   || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_2_LEFT))
      {
        G_collision_grid[grid_index].front_timer_offset = (240 - 42);
      }
      else  if ((room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_3_UP)     || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_3_RIGHT)  || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_3_DOWN)   || 
                (room_tilemap[tilemap_index] == LEVEL_ROOM_TILE_BUBBLE_POT_3_LEFT))
      {
        G_collision_grid[grid_index].front_timer_offset = (240 - 78);
      }
    }
  }

  return 0;
}

