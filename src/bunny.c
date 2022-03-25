/*******************************************************************************
** bunny.c (bunny update functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "color.h"
#include "doremi.h"
#include "global.h"
#include "grid.h"
#include "logic.h"
#include "state.h"
#include "thing.h"
#include "world.h"

/*******************************************************************************
** bunny_player_input_d_pad_left_on()
*******************************************************************************/
short int bunny_player_input_d_pad_left_on()
{
  thing* t;

  grid_box* b[2];

  int box_x[2];
  int box_y;

  unsigned int adjusted_timer_count;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* determine adjusted timer count */
  adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

  /* try to move left */
  if (G_bunny_facing == BUNNY_FACING_LEFT)
  {
    if (G_bunny_action == BUNNY_ACTION_STANDING)
    {
      G_bunny_action = BUNNY_ACTION_ACCELERATING;

      t->vel_x = -THING_BUNNY_VEL_X_ACCEL;
      t->timer_offset = 240 - G_timer_count;
    }
    else if ( (G_bunny_action == BUNNY_ACTION_HOPPING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
              (G_bunny_action == BUNNY_ACTION_FALLING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
              (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
    {
      t->vel_x = -THING_BUNNY_VEL_X_AIRSPEED;
      t->timer_offset = 0;
    }
    else if (G_bunny_action == BUNNY_ACTION_ACCELERATING)
    {
      /* check for ice blocks beneath the bunny */
      if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && ((t->pos_y + t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)) || 
          ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && ((t->pos_y - t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)))
      {
        /* determine grid box (or boxes) beneath the bunny */
        box_x[0] = (t->pos_x - t->hx) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
        box_x[1] = (t->pos_x + t->hx - 1) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        if (G_bunny_gravity == BUNNY_GRAVITY_UP)
          box_y -= 1;
        else
          box_y += 1;

        /* wraparound box indices */
        box_x[0] = (box_x[0] + GRID_WIDTH) % GRID_WIDTH;
        box_x[1] = (box_x[1] + GRID_WIDTH) % GRID_WIDTH;

        box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

        b[0] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[0]];
        b[1] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[1]];

        /* if there is ice here, use increased acceleration time */
        if ((b[0]->front_object == GRID_OBJECT_ICE_SINGLE)  || 
            (b[0]->front_object == GRID_OBJECT_ICE_LEFT)    || 
            (b[0]->front_object == GRID_OBJECT_ICE_RIGHT)   || 
            (b[1]->front_object == GRID_OBJECT_ICE_SINGLE)  || 
            (b[1]->front_object == GRID_OBJECT_ICE_LEFT)    || 
            (b[1]->front_object == GRID_OBJECT_ICE_RIGHT))
        {
          if (adjusted_timer_count >= THING_BUNNY_ACCEL_TIME_ICE)
          {
            G_bunny_action = BUNNY_ACTION_WALKING;

            t->vel_x = -THING_BUNNY_VEL_X_WALK;
            t->timer_offset = 0;
          }
        }
        /* otherwise, use the normal acceleration time */
        else if (adjusted_timer_count >= THING_BUNNY_ACCEL_TIME_NORMAL)
        {
          G_bunny_action = BUNNY_ACTION_WALKING;

          t->vel_x = -THING_BUNNY_VEL_X_WALK;
          t->timer_offset = 0;
        }
      }
      else if (adjusted_timer_count >= THING_BUNNY_ACCEL_TIME_NORMAL)
      {
        G_bunny_action = BUNNY_ACTION_WALKING;

        t->vel_x = -THING_BUNNY_VEL_X_WALK;
        t->timer_offset = 0;
      }
    }
    else if (G_bunny_action == BUNNY_ACTION_DECELERATING)
    {
      G_bunny_action = BUNNY_ACTION_WALKING;

      t->vel_x = -THING_BUNNY_VEL_X_WALK;
      t->timer_offset = 0;
    }
  }
  else if (G_bunny_facing == BUNNY_FACING_RIGHT)
  {
    if (G_bunny_action == BUNNY_ACTION_STANDING)
    {
      G_bunny_action = BUNNY_ACTION_ACCELERATING;
      G_bunny_facing = BUNNY_FACING_LEFT;

      t->vel_x = -THING_BUNNY_VEL_X_ACCEL;
      t->timer_offset = 240 - G_timer_count;
    }
    else if ( (G_bunny_action == BUNNY_ACTION_HOPPING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
              (G_bunny_action == BUNNY_ACTION_FALLING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
              (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
    {
      G_bunny_facing = BUNNY_FACING_LEFT;

      t->vel_x = -THING_BUNNY_VEL_X_AIRSPEED;
      t->timer_offset = 0;
    }
    else if (G_bunny_action == BUNNY_ACTION_ACCELERATING)
    {
      G_bunny_action = BUNNY_ACTION_ACCELERATING;
      G_bunny_facing = BUNNY_FACING_LEFT;

      t->vel_x = -THING_BUNNY_VEL_X_ACCEL;
      /*t->timer_offset = 240 - G_timer_count;*/
    }
    else if (G_bunny_action == BUNNY_ACTION_WALKING)
    {
      G_bunny_action = BUNNY_ACTION_WALKING;
      G_bunny_facing = BUNNY_FACING_LEFT;

      t->vel_x = -THING_BUNNY_VEL_X_WALK;
      t->timer_offset = 0;
    }
    else if (G_bunny_action == BUNNY_ACTION_DECELERATING)
    {
      G_bunny_action = BUNNY_ACTION_ACCELERATING;
      G_bunny_facing = BUNNY_FACING_LEFT;

      t->vel_x = -THING_BUNNY_VEL_X_ACCEL;
      /*t->timer_offset = 240 - G_timer_count;*/
    }
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_left_off()
*******************************************************************************/
short int bunny_player_input_d_pad_left_off()
{
  thing* t;

  grid_box* b[2];

  int box_x[2];
  int box_y;

  unsigned int adjusted_timer_count;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* determine adjusted timer count */
  adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

  /* try to stop moving left */
  if (G_bunny_facing == BUNNY_FACING_LEFT)
  {
    if (G_bunny_action == BUNNY_ACTION_WALKING)
    {
      G_bunny_action = BUNNY_ACTION_DECELERATING;

      t->vel_x = -THING_BUNNY_VEL_X_DECEL;
      t->timer_offset = 240 - G_timer_count;
    }
    else if (G_bunny_action == BUNNY_ACTION_ACCELERATING)
    {
      G_bunny_action = BUNNY_ACTION_STANDING;

      t->vel_x = 0;
      t->timer_offset = 0;
    }
    else if (G_bunny_action == BUNNY_ACTION_DECELERATING)
    {
      /* check for ice blocks beneath the bunny */
      if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && ((t->pos_y + t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)) || 
          ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && ((t->pos_y - t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)))
      {
        /* determine grid box (or boxes) beneath the bunny */
        box_x[0] = (t->pos_x - t->hx) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
        box_x[1] = (t->pos_x + t->hx - 1) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        if (G_bunny_gravity == BUNNY_GRAVITY_UP)
          box_y -= 1;
        else
          box_y += 1;

        /* wraparound box indices */
        box_x[0] = (box_x[0] + GRID_WIDTH) % GRID_WIDTH;
        box_x[1] = (box_x[1] + GRID_WIDTH) % GRID_WIDTH;

        box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

        b[0] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[0]];
        b[1] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[1]];

        /* if there is ice here, use increased deceleration time */
        if ((b[0]->front_object == GRID_OBJECT_ICE_SINGLE)  || 
            (b[0]->front_object == GRID_OBJECT_ICE_LEFT)    || 
            (b[0]->front_object == GRID_OBJECT_ICE_RIGHT)   || 
            (b[1]->front_object == GRID_OBJECT_ICE_SINGLE)  || 
            (b[1]->front_object == GRID_OBJECT_ICE_LEFT)    || 
            (b[1]->front_object == GRID_OBJECT_ICE_RIGHT))
        {
          if (adjusted_timer_count >= THING_BUNNY_DECEL_TIME_ICE)
          {
            G_bunny_action = BUNNY_ACTION_STANDING;

            t->vel_x = 0;
          }
        }
        /* otherwise, use the normal deceleration time */
        else if (adjusted_timer_count >= THING_BUNNY_DECEL_TIME_NORMAL)
        {
          G_bunny_action = BUNNY_ACTION_STANDING;

          t->vel_x = 0;
        }
      }
      else if (adjusted_timer_count >= THING_BUNNY_DECEL_TIME_NORMAL)
      {
        G_bunny_action = BUNNY_ACTION_STANDING;

        t->vel_x = 0;
      }
    }
    else if ( (G_bunny_action == BUNNY_ACTION_HOPPING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
              (G_bunny_action == BUNNY_ACTION_FALLING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
              (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
    {
      t->vel_x = 0;
    }
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_right_on()
*******************************************************************************/
short int bunny_player_input_d_pad_right_on()
{
  thing* t;

  grid_box* b[2];

  int box_x[2];
  int box_y;

  unsigned int adjusted_timer_count;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* determine adjusted timer count */
  adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

  /* try to move right */
  if (G_bunny_facing == BUNNY_FACING_RIGHT)
  {
    if (G_bunny_action == BUNNY_ACTION_STANDING)
    {
      G_bunny_action = BUNNY_ACTION_ACCELERATING;

      t->vel_x = THING_BUNNY_VEL_X_ACCEL;
      t->timer_offset = 240 - G_timer_count;
    }
    else if ( (G_bunny_action == BUNNY_ACTION_HOPPING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
              (G_bunny_action == BUNNY_ACTION_FALLING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
              (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
    {
      t->vel_x = THING_BUNNY_VEL_X_AIRSPEED;
      t->timer_offset = 0;
    }
    else if (G_bunny_action == BUNNY_ACTION_ACCELERATING)
    {
      /* check for ice blocks beneath the bunny */
      if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && ((t->pos_y + t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)) || 
          ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && ((t->pos_y - t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)))
      {
        /* determine grid box (or boxes) beneath the bunny */
        box_x[0] = (t->pos_x - t->hx) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
        box_x[1] = (t->pos_x + t->hx - 1) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        if (G_bunny_gravity == BUNNY_GRAVITY_UP)
          box_y -= 1;
        else
          box_y += 1;

        /* wraparound box indices */
        box_x[0] = (box_x[0] + GRID_WIDTH) % GRID_WIDTH;
        box_x[1] = (box_x[1] + GRID_WIDTH) % GRID_WIDTH;

        box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

        b[0] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[0]];
        b[1] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[1]];

        /* if there is ice here, use increased acceleration time */
        if ((b[0]->front_object == GRID_OBJECT_ICE_SINGLE)  || 
            (b[0]->front_object == GRID_OBJECT_ICE_LEFT)    || 
            (b[0]->front_object == GRID_OBJECT_ICE_RIGHT)   || 
            (b[1]->front_object == GRID_OBJECT_ICE_SINGLE)  || 
            (b[1]->front_object == GRID_OBJECT_ICE_LEFT)    || 
            (b[1]->front_object == GRID_OBJECT_ICE_RIGHT))
        {
          if (adjusted_timer_count >= THING_BUNNY_ACCEL_TIME_ICE)
          {
            G_bunny_action = BUNNY_ACTION_WALKING;

            t->vel_x = THING_BUNNY_VEL_X_WALK;
            t->timer_offset = 0;
          }
        }
        /* otherwise, use the normal acceleration time */
        else if (adjusted_timer_count >= THING_BUNNY_ACCEL_TIME_NORMAL)
        {
          G_bunny_action = BUNNY_ACTION_WALKING;

          t->vel_x = THING_BUNNY_VEL_X_WALK;
          t->timer_offset = 0;
        }
      }
      else if (adjusted_timer_count >= THING_BUNNY_ACCEL_TIME_NORMAL)
      {
        G_bunny_action = BUNNY_ACTION_WALKING;

        t->vel_x = THING_BUNNY_VEL_X_WALK;
        t->timer_offset = 0;
      }
    }
    else if (G_bunny_action == BUNNY_ACTION_DECELERATING)
    {
      G_bunny_action = BUNNY_ACTION_WALKING;

      t->vel_x = THING_BUNNY_VEL_X_WALK;
      t->timer_offset = 0;
    }
  }
  else if (G_bunny_facing == BUNNY_FACING_LEFT)
  {
    if (G_bunny_action == BUNNY_ACTION_STANDING)
    {
      G_bunny_action = BUNNY_ACTION_ACCELERATING;
      G_bunny_facing = BUNNY_FACING_RIGHT;

      t->vel_x = THING_BUNNY_VEL_X_ACCEL;
      t->timer_offset = 240 - G_timer_count;
    }
    else if ( (G_bunny_action == BUNNY_ACTION_HOPPING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
              (G_bunny_action == BUNNY_ACTION_FALLING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
              (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
    {
      G_bunny_facing = BUNNY_FACING_RIGHT;

      t->vel_x = THING_BUNNY_VEL_X_AIRSPEED;
      t->timer_offset = 0;
    }
    else if (G_bunny_action == BUNNY_ACTION_ACCELERATING)
    {
      G_bunny_action = BUNNY_ACTION_ACCELERATING;
      G_bunny_facing = BUNNY_FACING_RIGHT;

      t->vel_x = THING_BUNNY_VEL_X_ACCEL;
      /*t->timer_offset = 240 - G_timer_count;*/
    }
    else if (G_bunny_action == BUNNY_ACTION_WALKING)
    {
      G_bunny_action = BUNNY_ACTION_WALKING;
      G_bunny_facing = BUNNY_FACING_RIGHT;

      t->vel_x = THING_BUNNY_VEL_X_WALK;
      t->timer_offset = 0;
    }
    else if (G_bunny_action == BUNNY_ACTION_DECELERATING)
    {
      G_bunny_action = BUNNY_ACTION_ACCELERATING;
      G_bunny_facing = BUNNY_FACING_RIGHT;

      t->vel_x = THING_BUNNY_VEL_X_ACCEL;
      /*t->timer_offset = 240 - G_timer_count;*/
    }
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_right_off()
*******************************************************************************/
short int bunny_player_input_d_pad_right_off()
{
  thing* t;

  grid_box* b[2];

  int box_x[2];
  int box_y;

  unsigned int adjusted_timer_count;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* determine adjusted timer count */
  adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

  /* try to stop moving right */
  if (G_bunny_facing == BUNNY_FACING_RIGHT)
  {
    if (G_bunny_action == BUNNY_ACTION_WALKING)
    {
      G_bunny_action = BUNNY_ACTION_DECELERATING;

      t->vel_x = THING_BUNNY_VEL_X_DECEL;
      t->timer_offset = 240 - G_timer_count;
    }
    else if (G_bunny_action == BUNNY_ACTION_ACCELERATING)
    {
      G_bunny_action = BUNNY_ACTION_STANDING;

      t->vel_x = 0;
      t->timer_offset = 0;
    }
    else if (G_bunny_action == BUNNY_ACTION_DECELERATING)
    {
      /* check for ice blocks beneath the bunny */
      if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && ((t->pos_y + t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)) || 
          ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && ((t->pos_y - t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)))
      {
        /* determine grid box (or boxes) beneath the bunny */
        box_x[0] = (t->pos_x - t->hx) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
        box_x[1] = (t->pos_x + t->hx - 1) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        if (G_bunny_gravity == BUNNY_GRAVITY_UP)
          box_y -= 1;
        else
          box_y += 1;

        /* wraparound box indices */
        box_x[0] = (box_x[0] + GRID_WIDTH) % GRID_WIDTH;
        box_x[1] = (box_x[1] + GRID_WIDTH) % GRID_WIDTH;

        box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

        b[0] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[0]];
        b[1] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[1]];

        /* if there is ice here, use increased deceleration time */
        if ((b[0]->front_object == GRID_OBJECT_ICE_SINGLE)  || 
            (b[0]->front_object == GRID_OBJECT_ICE_LEFT)    || 
            (b[0]->front_object == GRID_OBJECT_ICE_RIGHT)   || 
            (b[1]->front_object == GRID_OBJECT_ICE_SINGLE)  || 
            (b[1]->front_object == GRID_OBJECT_ICE_LEFT)    || 
            (b[1]->front_object == GRID_OBJECT_ICE_RIGHT))
        {
          if (adjusted_timer_count >= THING_BUNNY_DECEL_TIME_ICE)
          {
            G_bunny_action = BUNNY_ACTION_STANDING;

            t->vel_x = 0;
          }
        }
        /* otherwise, use the normal deceleration time */
        else if (adjusted_timer_count >= THING_BUNNY_DECEL_TIME_NORMAL)
        {
          G_bunny_action = BUNNY_ACTION_STANDING;

          t->vel_x = 0;
        }
      }
      else if (adjusted_timer_count >= THING_BUNNY_DECEL_TIME_NORMAL)
      {
        G_bunny_action = BUNNY_ACTION_STANDING;

        t->vel_x = 0;
      }
    }
    else if ( (G_bunny_action == BUNNY_ACTION_HOPPING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
              (G_bunny_action == BUNNY_ACTION_FALLING)        || 
              (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
              (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
    {
      t->vel_x = 0;
    }
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_up_on()
*******************************************************************************/
short int bunny_player_input_d_pad_up_on()
{
  thing* t;

  grid_box* b[2];

  int box_x;
  int box_y[2];

  int move_amount;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* if currently holding something, return */
  if (G_world_all_things[WORLD_HELD_THING_INDEX].type != THING_TYPE_NONE)
    return 0;

  /* determine grid boxes behind the bunny */
  box_x = t->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

  box_y[0] = ((t->pos_y - t->hy) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE));
  box_y[1] = ((t->pos_y + t->hy - 1) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE));

  /* wraparound box indices */
  box_x = (box_x + GRID_WIDTH) % GRID_WIDTH;

  box_y[0] = (box_y[0] + GRID_HEIGHT) % GRID_HEIGHT;
  box_y[1] = (box_y[1] + GRID_HEIGHT) % GRID_HEIGHT;

  b[0] = &G_collision_grid[(box_y[0] * GRID_WIDTH) + box_x];
  b[1] = &G_collision_grid[(box_y[1] * GRID_WIDTH) + box_x];

  /* try to climb up */
  if (G_bunny_action == BUNNY_ACTION_CLIMBING)
  {
    /* if there is no ladder here, switch to falling */
    if ((b[0]->back_object != GRID_OBJECT_LADDER) && 
        (b[1]->back_object != GRID_OBJECT_LADDER))
    {
      G_bunny_action = BUNNY_ACTION_FALLING;
    }
    /* if this is the top of the ladder, stop climbing */
    else if ( (G_bunny_gravity == BUNNY_GRAVITY_DOWN) && 
              (b[0]->back_object != GRID_OBJECT_LADDER))
    {
      t->vel_y = 0;
    }
    /* otherwise, try to move upward */
    else
    {
      t->vel_y = -THING_BUNNY_VEL_Y_CLIMBING;
    }
  }
  /* try to grab ladder */
  else if ( (G_bunny_action == BUNNY_ACTION_STANDING)       || 
            (G_bunny_action == BUNNY_ACTION_ACCELERATING)   || 
            (G_bunny_action == BUNNY_ACTION_WALKING)        || 
            (G_bunny_action == BUNNY_ACTION_DECELERATING)   || 
            (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
            (G_bunny_action == BUNNY_ACTION_FALLING)        || 
            (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
            (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
  {
    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
    {
      if (b[0]->back_object == GRID_OBJECT_LADDER)
      {
        /* if the bunny is not exactly centered on  */
        /* the ladder, try to move so that she is   */
        move_amount = ((box_x * GRID_BOX_SIZE + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS) - t->pos_x;

        if (move_amount != 0)
        {
          /* move bunny horizontally */
          grid_move_thing(WORLD_BUNNY_INDEX, GRID_MOVE_THING_MODE_HORIZONTAL, move_amount);

          /* if the bunny is now centered, grab the ladder */
          if (t->pos_x == (box_x * GRID_BOX_SIZE + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS)
          {
            G_bunny_action = BUNNY_ACTION_CLIMBING;
            G_bunny_facing = BUNNY_FACING_RIGHT;

            t->vel_x = 0;
            t->vel_y = 0;
          }
        }
        else
        {
          G_bunny_action = BUNNY_ACTION_CLIMBING;
          G_bunny_facing = BUNNY_FACING_RIGHT;

          t->vel_x = 0;
          t->vel_y = 0;
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_up_off()
*******************************************************************************/
short int bunny_player_input_d_pad_up_off()
{
  thing* t;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* try to stop climbing */
  if ((G_bunny_action == BUNNY_ACTION_CLIMBING) && (t->vel_y < 0))
  {
    t->vel_x = 0;
    t->vel_y = 0;
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_down_on()
*******************************************************************************/
short int bunny_player_input_d_pad_down_on()
{
  thing* t;

  grid_box* b[2];

  int box_x;
  int box_y[2];

  int move_amount;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* if currently holding something, return */
  if (G_world_all_things[WORLD_HELD_THING_INDEX].type != THING_TYPE_NONE)
    return 0;

  /* determine grid boxes behind the bunny */
  box_x = t->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

  box_y[0] = ((t->pos_y - t->hy) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE));
  box_y[1] = ((t->pos_y + t->hy - 1) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE));

  /* wraparound box indices */
  box_x = (box_x + GRID_WIDTH) % GRID_WIDTH;

  box_y[0] = (box_y[0] + GRID_HEIGHT) % GRID_HEIGHT;
  box_y[1] = (box_y[1] + GRID_HEIGHT) % GRID_HEIGHT;

  b[0] = &G_collision_grid[(box_y[0] * GRID_WIDTH) + box_x];
  b[1] = &G_collision_grid[(box_y[1] * GRID_WIDTH) + box_x];

  /* try to climb down */
  if (G_bunny_action == BUNNY_ACTION_CLIMBING)
  {
    /* if there is no ladder here, switch to falling */
    if ((b[0]->back_object != GRID_OBJECT_LADDER) && 
        (b[1]->back_object != GRID_OBJECT_LADDER))
    {
      G_bunny_action = BUNNY_ACTION_FALLING;
    }
    /* if this is the bottom of the ladder, stop climbing */
    else if ( (G_bunny_gravity == BUNNY_GRAVITY_UP) && 
              (b[1]->back_object != GRID_OBJECT_LADDER))
    {
      t->vel_y = 0;
    }
    /* otherwise, try to move downward */
    else
    {
      t->vel_y = THING_BUNNY_VEL_Y_CLIMBING;
    }
  }
  /* try to grab ladder */
  else if ( (G_bunny_action == BUNNY_ACTION_STANDING)       || 
            (G_bunny_action == BUNNY_ACTION_ACCELERATING)   || 
            (G_bunny_action == BUNNY_ACTION_WALKING)        || 
            (G_bunny_action == BUNNY_ACTION_DECELERATING)   || 
            (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
            (G_bunny_action == BUNNY_ACTION_FALLING)        || 
            (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
            (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
  {
    if (G_bunny_gravity == BUNNY_GRAVITY_UP)
    {
      if (b[1]->back_object == GRID_OBJECT_LADDER)
      {
        /* if the bunny is not exactly centered on  */
        /* the ladder, try to move so that she is   */
        move_amount = ((box_x * GRID_BOX_SIZE + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS) - t->pos_x;

        if (move_amount != 0)
        {
          /* move bunny horizontally */
          grid_move_thing(WORLD_BUNNY_INDEX, GRID_MOVE_THING_MODE_HORIZONTAL, move_amount);

          /* if the bunny is now centered, grab the ladder */
          if (t->pos_x == (box_x * GRID_BOX_SIZE + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS)
          {
            G_bunny_action = BUNNY_ACTION_CLIMBING;
            G_bunny_facing = BUNNY_FACING_RIGHT;

            t->vel_x = 0;
            t->vel_y = 0;
          }
        }
        else
        {
          G_bunny_action = BUNNY_ACTION_CLIMBING;
          G_bunny_facing = BUNNY_FACING_RIGHT;

          t->vel_x = 0;
          t->vel_y = 0;
        }
      }
    }
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_down_off()
*******************************************************************************/
short int bunny_player_input_d_pad_down_off()
{
  thing* t;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* try to stop climbing */
  if ((G_bunny_action == BUNNY_ACTION_CLIMBING) && (t->vel_y > 0))
  {
    t->vel_x = 0;
    t->vel_y = 0;
  }

  return 0;
}

/*******************************************************************************
** bunny_throw_thing()
*******************************************************************************/
short int bunny_throw_thing(int pos_x, int pos_y, int vel_x, int vel_y)
{
  thing* t1;
  thing* t2;

  int k;

  int space_check;

  int old_pos_x;
  int old_pos_y;

  int type;
  int color;
  int state;
  int orient;

  int timer_offset;

  t1 = &G_world_all_things[WORLD_BUNNY_INDEX];
  t2 = &G_world_all_things[WORLD_HELD_THING_INDEX];

  if (t1->type != THING_TYPE_BUNNY)
    return 0;

  if (t2->type == THING_TYPE_NONE)
    return 0;

  /* make sure there is a space for this dropped / thrown thing */
  space_check = 0;

  for ( k = WORLD_THROWN_THINGS_START_INDEX; 
        k < WORLD_THROWN_THINGS_END_INDEX; 
        k++)
  {
    if (G_world_all_things[k].type == THING_TYPE_NONE)
    {
      space_check = 1;
      break;
    }
  }

  if (space_check == 0)
    return 0;

  /* test the current held thing's potential new position */
  old_pos_x = t2->pos_x;
  old_pos_y = t2->pos_y;

  t2->pos_x = pos_x;
  t2->pos_y = pos_y;

  grid_move_thing(WORLD_HELD_THING_INDEX, GRID_MOVE_THING_MODE_TEST_POSITION, 0);

  /* if held thing collided with something, cancel the throw */
  if (t2->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
  {
    t2->pos_x = old_pos_x;
    t2->pos_y = old_pos_y;

    return 0;
  }
  else
  {
    /* obtain held thing's information */
    type = t2->type;
    color = t2->color;
    state = t2->state;
    orient = t2->orient;

    timer_offset = t2->timer_offset;

    /* convert type from held to thrown */
    if (type == THING_TYPE_HELD_MARBLE)
      type = THING_TYPE_THROWN_MARBLE;
    else if (type == THING_TYPE_HELD_CRITTER_WALKER_1)
      type = THING_TYPE_THROWN_CRITTER_WALKER_1;
    else if (type == THING_TYPE_HELD_CRITTER_WALKER_2)
      type = THING_TYPE_THROWN_CRITTER_WALKER_2;
    else if (type == THING_TYPE_HELD_CRITTER_FLYER_1)
      type = THING_TYPE_THROWN_CRITTER_FLYER_1;
    else if (type == THING_TYPE_HELD_CRITTER_FLYER_2)
      type = THING_TYPE_THROWN_CRITTER_FLYER_2;
    else
    {
      /* unknown type, so cancel the throw */
      t2->pos_x = old_pos_x;
      t2->pos_y = old_pos_y;

      return 0;
    }

    /* despawn held thing, respawn it as a thrown thing */
    world_despawn_thing(WORLD_HELD_THING_INDEX);
    world_spawn_thing(type, color, state, orient, 
                      pos_x, pos_y, vel_x, vel_y, timer_offset);

    /* play drop or throw sound */
    if (vel_y == THING_THROWN_FALLING_VEL)
      doremi_play_sfx(SFX_INDEX_BUNNY_DROP);
    else
      doremi_play_sfx(SFX_INDEX_BUNNY_THROW);

    /* update bunny state */
    G_bunny_action = BUNNY_ACTION_THROWING;

    t1->timer_offset = 240 - G_timer_count;

    t1->vel_x = 0;
    t1->vel_y = 0;
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_up_on_and_button_b_pressed()
*******************************************************************************/
short int bunny_player_input_d_pad_up_on_and_button_b_pressed()
{
  thing* t;

  int pos_x;
  int pos_y;

  int vel_x;
  int vel_y;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* if currently holding something, try to throw it upward */
  if (G_world_all_things[WORLD_HELD_THING_INDEX].type != THING_TYPE_NONE)
  {
    /* compute new position and velocity */
    if (G_bunny_facing == BUNNY_FACING_RIGHT)
    {
      pos_x = t->pos_x + (16 * THING_NUM_SUBPIXELS);

      vel_x = THING_THROWN_DIAGONAL_VEL;
      vel_y = -THING_THROWN_DIAGONAL_VEL;
    }
    else
    {
      pos_x = t->pos_x - (16 * THING_NUM_SUBPIXELS);

      vel_x = -THING_THROWN_DIAGONAL_VEL;
      vel_y = -THING_THROWN_DIAGONAL_VEL;
    }

    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
      pos_y = t->pos_y - (16 * THING_NUM_SUBPIXELS);
    else
      pos_y = t->pos_y;

    /* attempt the throw */
    bunny_throw_thing(pos_x, pos_y, vel_x, vel_y);
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_down_on_and_button_b_pressed()
*******************************************************************************/
short int bunny_player_input_d_pad_down_on_and_button_b_pressed()
{
  thing* t;

  int pos_x;
  int pos_y;

  int vel_x;
  int vel_y;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* if currently holding something, try to throw it downward */
  if (G_world_all_things[WORLD_HELD_THING_INDEX].type != THING_TYPE_NONE)
  {
    /* compute new position and velocity */
    if (G_bunny_facing == BUNNY_FACING_RIGHT)
    {
      pos_x = t->pos_x + (16 * THING_NUM_SUBPIXELS);

      vel_x = THING_THROWN_DIAGONAL_VEL;
      vel_y = THING_THROWN_DIAGONAL_VEL;
    }
    else
    {
      pos_x = t->pos_x - (16 * THING_NUM_SUBPIXELS);

      vel_x = -THING_THROWN_DIAGONAL_VEL;
      vel_y = THING_THROWN_DIAGONAL_VEL;
    }

    if (G_bunny_gravity == BUNNY_GRAVITY_UP)
      pos_y = t->pos_y + (16 * THING_NUM_SUBPIXELS);
    else
      pos_y = t->pos_y;

    /* attempt the throw */
    bunny_throw_thing(pos_x, pos_y, vel_x, vel_y);
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_left_on_and_button_b_pressed()
*******************************************************************************/
short int bunny_player_input_d_pad_left_on_and_button_b_pressed()
{
  thing* t;

  grid_box* b[2];

  int box_x[2];
  int box_y;

  int pos_x;
  int pos_y;

  int vel_x;
  int vel_y;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* if not currently holding anything, try to kick */
  if (G_world_all_things[WORLD_HELD_THING_INDEX].type == THING_TYPE_NONE)
  {
    if ((G_bunny_action == BUNNY_ACTION_STANDING)     || 
        (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
        (G_bunny_action == BUNNY_ACTION_WALKING)      || 
        (G_bunny_action == BUNNY_ACTION_DECELERATING))
    {
      if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && ((t->pos_y + t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)) || 
          ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && ((t->pos_y - t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)))
      {
        if ((t->pos_x - t->hx) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)
        {
          /* determine grid boxes left of the bunny */
          box_x[0] = (t->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) - 1;
          box_x[1] = (t->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) - 2;

          box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

          /* wraparound box indices */
          box_x[0] = (box_x[0] + GRID_WIDTH) % GRID_WIDTH;
          box_x[1] = (box_x[1] + GRID_WIDTH) % GRID_WIDTH;

          box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

          b[0] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[0]];
          b[1] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[1]];

          /* if there is a marble here, and open space behind it, kick it! */
          if ((b[0]->front_object == GRID_OBJECT_MARBLE)  && 
              (b[0]->front_state  == STATE_NONE)          && 
              (b[1]->front_object == GRID_OBJECT_NONE)    && 
              (b[1]->num_things == 0))
          {
            b[1]->front_object = GRID_OBJECT_MARBLE;
            b[1]->front_color = b[0]->front_color;
            b[1]->front_state = STATE_NONE;
            b[1]->front_timer_offset = 0;

            b[0]->front_object = GRID_OBJECT_NONE;
            b[0]->front_color = COLOR_NONE;
            b[0]->front_state = STATE_NONE;
            b[0]->front_timer_offset = 0;

            G_bunny_action = BUNNY_ACTION_KICKING;
            G_bunny_facing = BUNNY_FACING_LEFT;

            t->timer_offset = 240 - G_timer_count;

            t->vel_x = 0;
            t->vel_y = 0;

            /* check for matches this frame */
            logic_marbles_check_for_matches();

            /* play kick sound */
            doremi_play_sfx(SFX_INDEX_BUNNY_KICK);
          }
        }
      }
    }
  }
  /* if currently holding something, try to throw   */
  /* it downward (or upward, if we are upside down) */
  else
  {
    /* compute new position */
    pos_x = t->pos_x - (16 * THING_NUM_SUBPIXELS);
    pos_y = t->pos_y;

    /* compute new velocity */
    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
    {
      vel_x = -THING_THROWN_DIAGONAL_VEL;
      vel_y = THING_THROWN_DIAGONAL_VEL;
    }
    else
    {
      vel_x = -THING_THROWN_DIAGONAL_VEL;
      vel_y = -THING_THROWN_DIAGONAL_VEL;
    }

    /* attempt the throw */
    bunny_throw_thing(pos_x, pos_y, vel_x, vel_y);
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_right_on_and_button_b_pressed()
*******************************************************************************/
short int bunny_player_input_d_pad_right_on_and_button_b_pressed()
{
  thing* t;

  grid_box* b[2];

  int box_x[2];
  int box_y;

  int pos_x;
  int pos_y;

  int vel_x;
  int vel_y;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* if not currently holding anything, try to kick */
  if (G_world_all_things[WORLD_HELD_THING_INDEX].type == THING_TYPE_NONE)
  {
    if ((G_bunny_action == BUNNY_ACTION_STANDING)     || 
        (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
        (G_bunny_action == BUNNY_ACTION_WALKING)      || 
        (G_bunny_action == BUNNY_ACTION_DECELERATING))
    {
      if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && ((t->pos_y + t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)) || 
          ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && ((t->pos_y - t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)))
      {
        if ((t->pos_x + t->hx) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)
        {
          /* determine grid boxes left of the bunny */
          box_x[0] = (t->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) + 1;
          box_x[1] = (t->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE)) + 2;

          box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

          /* wraparound box indices */
          box_x[0] = (box_x[0] + GRID_WIDTH) % GRID_WIDTH;
          box_x[1] = (box_x[1] + GRID_WIDTH) % GRID_WIDTH;

          box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

          b[0] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[0]];
          b[1] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[1]];

          /* if there is a marble here, and open space behind it, kick it! */
          if ((b[0]->front_object == GRID_OBJECT_MARBLE)  && 
              (b[0]->front_state  == STATE_NONE)          && 
              (b[1]->front_object == GRID_OBJECT_NONE)    && 
              (b[1]->num_things == 0))
          {
            b[1]->front_object = GRID_OBJECT_MARBLE;
            b[1]->front_color = b[0]->front_color;
            b[1]->front_state = STATE_NONE;
            b[1]->front_timer_offset = 0;

            b[0]->front_object = GRID_OBJECT_NONE;
            b[0]->front_color = COLOR_NONE;
            b[0]->front_state = STATE_NONE;
            b[0]->front_timer_offset = 0;

            G_bunny_action = BUNNY_ACTION_KICKING;
            G_bunny_facing = BUNNY_FACING_RIGHT;

            t->timer_offset = 240 - G_timer_count;

            t->vel_x = 0;
            t->vel_y = 0;

            /* check for matches this frame */
            logic_marbles_check_for_matches();

            /* play kick sound */
            doremi_play_sfx(SFX_INDEX_BUNNY_KICK);
          }
        }
      }
    }
  }
  /* if currently holding something, try to throw   */
  /* it downward (or upward, if we are upside down) */
  else
  {
    /* compute new position */
    pos_x = t->pos_x + (16 * THING_NUM_SUBPIXELS);
    pos_y = t->pos_y;

    /* compute new velocity */
    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
    {
      vel_x = THING_THROWN_DIAGONAL_VEL;
      vel_y = THING_THROWN_DIAGONAL_VEL;
    }
    else
    {
      vel_x = THING_THROWN_DIAGONAL_VEL;
      vel_y = -THING_THROWN_DIAGONAL_VEL;
    }

    /* attempt the throw */
    bunny_throw_thing(pos_x, pos_y, vel_x, vel_y);
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_d_pad_neutral_and_button_b_pressed()
*******************************************************************************/
short int bunny_player_input_d_pad_neutral_and_button_b_pressed()
{
  thing* t;

  int pos_x;
  int pos_y;

  int vel_x;
  int vel_y;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* if currently holding something, try to drop it */
  if (G_world_all_things[WORLD_HELD_THING_INDEX].type != THING_TYPE_NONE)
  {
    /* compute new position */
    if (G_bunny_facing == BUNNY_FACING_RIGHT)
    {
      pos_x = t->pos_x + (16 * THING_NUM_SUBPIXELS);
      pos_y = t->pos_y;
    }
    else
    {
      pos_x = t->pos_x - (16 * THING_NUM_SUBPIXELS);
      pos_y = t->pos_y;
    }

    /* compute new velocity */
    if (G_world_all_things[WORLD_HELD_THING_INDEX].type == THING_TYPE_HELD_MARBLE)
    {
      if (G_marble_gravity == MARBLE_GRAVITY_DOWN)
      {
        vel_x = 0;
        vel_y = THING_THROWN_FALLING_VEL;
      }
      else if (G_marble_gravity == MARBLE_GRAVITY_UP)
      {
        vel_x = 0;
        vel_y = -THING_THROWN_FALLING_VEL;
      }
      else if (G_marble_gravity == MARBLE_GRAVITY_RIGHT)
      {
        vel_x = THING_THROWN_FALLING_VEL;
        vel_y = 0;
      }
      else if (G_marble_gravity == MARBLE_GRAVITY_LEFT)
      {
        vel_x = -THING_THROWN_FALLING_VEL;
        vel_y = 0;
      }
      else
      {
        vel_x = 0;
        vel_y = THING_THROWN_FALLING_VEL;
      }
    }
    else
    {
      vel_x = 0;
      vel_y = THING_THROWN_FALLING_VEL;
    }

    /* attempt the drop */
    bunny_throw_thing(pos_x, pos_y, vel_x, vel_y);
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_always_when_button_b_pressed()
*******************************************************************************/
short int bunny_player_input_always_when_button_b_pressed()
{
  thing* t1;
  thing* t2;

  grid_box* b;

  int box_x;
  int box_y;

  int k;

  int space_check;

  int grab_index;

  int type;
  int color;
  int state;

  int timer_offset;

  int pos_x;
  int pos_y;

  int vel_x;
  int vel_y;

  t1 = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t1->type != THING_TYPE_BUNNY)
    return 0;

  /* if there is no space available for another thrown thing,   */
  /* don't grab another one (or we couldn't put it down later!) */
  space_check = 0;

  for ( k = WORLD_THROWN_THINGS_START_INDEX; 
        k < WORLD_THROWN_THINGS_END_INDEX; 
        k++)
  {
    if (G_world_all_things[k].type == THING_TYPE_NONE)
    {
      space_check = 1;
      break;
    }
  }

  if (space_check == 0)
    return 0;

  /* if not currently holding anything, try to grab something */
  if (G_world_all_things[WORLD_HELD_THING_INDEX].type == THING_TYPE_NONE)
  {
    if ((G_bunny_action == BUNNY_ACTION_STANDING)     || 
        (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
        (G_bunny_action == BUNNY_ACTION_WALKING)      || 
        (G_bunny_action == BUNNY_ACTION_DECELERATING))
    {
      /* check for a marble in the grid box beneath this one */
      if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && ((t1->pos_y + t1->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)) || 
          ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && ((t1->pos_y - t1->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)))
      {
        box_x = t1->pos_x / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
        box_y = t1->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

        if (G_bunny_gravity == BUNNY_GRAVITY_UP)
          box_y -= 1;
        else
          box_y += 1;

        /* wraparound box indices */
        box_x = (box_x + GRID_WIDTH) % GRID_WIDTH;
        box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

        b = &G_collision_grid[(box_y * GRID_WIDTH) + box_x];

        /* marble found! (make sure that    */
        /* it is not flashing or changing)  */
        if ((b->front_object == GRID_OBJECT_MARBLE) && (b->front_state == STATE_NONE))
        {
          G_bunny_action = BUNNY_ACTION_GRABBING;

          t1->timer_offset = 240 - G_timer_count;

          t1->vel_x = 0;
          t1->vel_y = 0;

          /* compute marble position and color  */
          /* note that it immediately snaps to  */
          /* being below (or above) the bunny   */
          pos_x = t1->pos_x;
          pos_y = ((box_y * GRID_BOX_SIZE) + GRID_BOX_SIZE_HALF) * THING_NUM_SUBPIXELS;

          vel_x = 0;

          if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
            vel_y = -THING_GRABBED_VEL;
          else
            vel_y = THING_GRABBED_VEL;

          /* obtain marble information */
          color = b->front_color;
          state = b->front_state;

          timer_offset = b->front_timer_offset;

          /* remove marble from grid */
          b->front_object = GRID_OBJECT_NONE;
          b->front_color = COLOR_NONE;
          b->front_state = STATE_NONE;
          b->front_timer_offset = 0;

          /* spawn held marble */
          world_spawn_thing(THING_TYPE_HELD_MARBLE, color, state, THING_ORIENT_NORMAL, 
                            pos_x, pos_y, vel_x, vel_y, timer_offset);

          /* set flag to redraw the grid this frame */
          G_flag_redraw_grid_objects = 1;

          /* play grab sound */
          doremi_play_sfx(SFX_INDEX_BUNNY_GRAB);

          return 0;
        }
      }

      /* otherwise, see if we are riding on a critter or a thrown thing */
      /* note that technically, the rider indices are from the previous */
      /* frame, as they are cleared before the things are updated.      */
      /* however, since there are no teleports or anything, this info   */
      /* should be accurate enough.                                     */
      grab_index = -1;

      for ( k = WORLD_RIDERS_ON_CRITTER_START_INDEX; 
            k < WORLD_RIDERS_ON_CRITTER_END_INDEX; 
            k++)
      {
        if (G_world_riders[k] == WORLD_BUNNY_INDEX)
        {
          grab_index = k - WORLD_RIDERS_ON_CRITTER_START_INDEX + WORLD_CRITTERS_START_INDEX;
          break;
        }
      }

      for ( k = WORLD_RIDERS_ON_THROWN_THING_START_INDEX; 
            k < WORLD_RIDERS_ON_THROWN_THING_END_INDEX; 
            k++)
      {
        if (G_world_riders[k] == WORLD_BUNNY_INDEX)
        {
          grab_index = k - WORLD_RIDERS_ON_THROWN_THING_START_INDEX + WORLD_THROWN_THINGS_START_INDEX;
          break;
        }
      }

      /* if a critter was found, grab it! */
      if (grab_index != -1)
      {
        t2 = &G_world_all_things[grab_index];

        /* determine type */
        if (t2->type == THING_TYPE_THROWN_MARBLE)
          type = THING_TYPE_HELD_MARBLE;
        else if ( (t2->type == THING_TYPE_CRITTER_WALKER_1) || 
                  (t2->type == THING_TYPE_THROWN_CRITTER_WALKER_1))
        {
          type = THING_TYPE_HELD_CRITTER_WALKER_1;
        }
        else if ( (t2->type == THING_TYPE_CRITTER_WALKER_2) || 
                  (t2->type == THING_TYPE_THROWN_CRITTER_WALKER_2))
        {
          type = THING_TYPE_HELD_CRITTER_WALKER_2;
        }
        else if ( (t2->type == THING_TYPE_CRITTER_FLYER_1) || 
                  (t2->type == THING_TYPE_THROWN_CRITTER_FLYER_1))
        {
          type = THING_TYPE_HELD_CRITTER_FLYER_1;
        }
        else if ( (t2->type == THING_TYPE_CRITTER_FLYER_2) || 
                  (t2->type == THING_TYPE_THROWN_CRITTER_FLYER_2))
        {
          type = THING_TYPE_HELD_CRITTER_FLYER_2;
        }
        else
        {
          /* unknown type, so cancel the grab */
          return 0;
        }

        /* set color and state if necessary */
        color = t2->color;
        state = t2->state;
        timer_offset = t2->timer_offset;

        /* set position and initial velocity */
        pos_x = t1->pos_x;

        if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
          pos_y = t1->pos_y + (16 * THING_NUM_SUBPIXELS);
        else
          pos_y = t1->pos_y - (16 * THING_NUM_SUBPIXELS);

        vel_x = 0;

        if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
          vel_y = -THING_GRABBED_VEL;
        else
          vel_y = THING_GRABBED_VEL;

        /* grab! */
        G_bunny_action = BUNNY_ACTION_GRABBING;

        t1->timer_offset = 240 - G_timer_count;

        t1->vel_x = 0;
        t1->vel_y = 0;

        /* despawn the critter, and respawn as a held critter */
        world_despawn_thing(grab_index);
        world_spawn_thing(type, color, state, THING_ORIENT_NORMAL, 
                          pos_x, pos_y, vel_x, vel_y, timer_offset);

        /* play grab sound */
        doremi_play_sfx(SFX_INDEX_BUNNY_GRAB);

        return 0;
      }
    }
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_button_a_pressed()
*******************************************************************************/
short int bunny_player_input_button_a_pressed()
{
  thing* t;

  grid_box* b[2];

  int box_x[2];
  int box_y;

  int super_jump_flag;
  int double_jump_flag;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* try to jump */
  if ((G_bunny_action == BUNNY_ACTION_STANDING)     || 
      (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
      (G_bunny_action == BUNNY_ACTION_WALKING)      || 
      (G_bunny_action == BUNNY_ACTION_DECELERATING))
  {
    /* initialize flags */
    super_jump_flag = 0;
    double_jump_flag = 0;

    /* check for diamond or heart blocks beneath the bunny */
    if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && ((t->pos_y + t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)) || 
        ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && ((t->pos_y - t->hy) % (THING_NUM_SUBPIXELS * GRID_BOX_SIZE) == 0)))
    {
      /* determine grid box (or boxes) beneath the bunny */
      box_x[0] = (t->pos_x - t->hx) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);
      box_x[1] = (t->pos_x + t->hx - 1) / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

      box_y = t->pos_y / (THING_NUM_SUBPIXELS * GRID_BOX_SIZE);

      if (G_bunny_gravity == BUNNY_GRAVITY_UP)
        box_y -= 1;
      else
        box_y += 1;

      /* wraparound box indices */
      box_x[0] = (box_x[0] + GRID_WIDTH) % GRID_WIDTH;
      box_x[1] = (box_x[1] + GRID_WIDTH) % GRID_WIDTH;

      box_y = (box_y + GRID_HEIGHT) % GRID_HEIGHT;

      b[0] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[0]];
      b[1] = &G_collision_grid[(box_y * GRID_WIDTH) + box_x[1]];

      /* if there is a diamond block here, set super jump flag */
      if (b[0]->front_object == GRID_OBJECT_DIAMOND_BLOCK)
      {
        super_jump_flag = 1;

        if (G_bunny_gravity == BUNNY_GRAVITY_UP)
          b[0]->front_state = STATE_DIAMOND_BLOCK_FLASHING_BOTTOM;
        else
          b[0]->front_state = STATE_DIAMOND_BLOCK_FLASHING_TOP;

        b[0]->front_timer_offset = 240 - G_timer_count;
      }

      if (b[1]->front_object == GRID_OBJECT_DIAMOND_BLOCK)
      {
        super_jump_flag = 1;

        if (G_bunny_gravity == BUNNY_GRAVITY_UP)
          b[1]->front_state = STATE_DIAMOND_BLOCK_FLASHING_BOTTOM;
        else
          b[1]->front_state = STATE_DIAMOND_BLOCK_FLASHING_TOP;

        b[1]->front_timer_offset = 240 - G_timer_count;
      }

      /* if there is a heart block here, set double jump flag */
      if (b[0]->front_object == GRID_OBJECT_HEART_BLOCK)
      {
        double_jump_flag = 1;

        if (G_bunny_gravity == BUNNY_GRAVITY_UP)
          b[0]->front_state = STATE_HEART_BLOCK_FLASHING_BOTTOM;
        else
          b[0]->front_state = STATE_HEART_BLOCK_FLASHING_TOP;

        b[0]->front_timer_offset = 240 - G_timer_count;
      }

      if (b[1]->front_object == GRID_OBJECT_HEART_BLOCK)
      {
        double_jump_flag = 1;

        if (G_bunny_gravity == BUNNY_GRAVITY_UP)
          b[1]->front_state = STATE_HEART_BLOCK_FLASHING_BOTTOM;
        else
          b[1]->front_state = STATE_HEART_BLOCK_FLASHING_TOP;

        b[1]->front_timer_offset = 240 - G_timer_count;
      }
    }

    /* set jump state */
    if (double_jump_flag == 1)
      G_bunny_action = BUNNY_ACTION_JUMPING_DOUBLE;
    else
      G_bunny_action = BUNNY_ACTION_JUMPING;

    /* set x velocity */
    if (t->vel_x > 0)
      t->vel_x = THING_BUNNY_VEL_X_AIRSPEED;
    else if (t->vel_x < 0)
      t->vel_x = -THING_BUNNY_VEL_X_AIRSPEED;

    /* set y velocity */
    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
    {
      if (super_jump_flag == 1)
        t->vel_y = -THING_BUNNY_VEL_Y_SUPER;
      else
        t->vel_y = -THING_BUNNY_VEL_Y_JUMP;
    }
    else if (G_bunny_gravity == BUNNY_GRAVITY_UP)
    {
      if (super_jump_flag == 1)
        t->vel_y = THING_BUNNY_VEL_Y_SUPER;
      else
        t->vel_y = THING_BUNNY_VEL_Y_JUMP;
    }

    t->timer_offset = 0;

    /* play diamond or heart block sound */
    if (super_jump_flag == 1)
      doremi_play_sfx(SFX_INDEX_DIAMOND_BLOCK);

    if (double_jump_flag == 1)
      doremi_play_sfx(SFX_INDEX_HEART_BLOCK);
  }
  /* double jump */
  else if ( (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
            (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
  {
    G_bunny_action = BUNNY_ACTION_JUMPING;

    /* set x velocity */
    if (t->vel_x > 0)
      t->vel_x = THING_BUNNY_VEL_X_AIRSPEED;
    else if (t->vel_x < 0)
      t->vel_x = -THING_BUNNY_VEL_X_AIRSPEED;

    /* set y velocity */
    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
      t->vel_y = -THING_BUNNY_VEL_Y_JUMP;
    else if (G_bunny_gravity == BUNNY_GRAVITY_UP)
      t->vel_y = THING_BUNNY_VEL_Y_JUMP;

    t->timer_offset = 0;
  }
  /* jump off of ladder */
  else if (G_bunny_action == BUNNY_ACTION_CLIMBING)
  {
    G_bunny_action = BUNNY_ACTION_HOPPING;

    t->vel_x = 0;

    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
      t->vel_y = -THING_BUNNY_VEL_Y_HOP;
    else if (G_bunny_gravity == BUNNY_GRAVITY_UP)
      t->vel_y = THING_BUNNY_VEL_Y_HOP;

    t->timer_offset = 0;
  }

  return 0;
}

/*******************************************************************************
** bunny_player_input_button_a_released()
*******************************************************************************/
short int bunny_player_input_button_a_released()
{
  thing* t;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* if jumping, attenuate it */
  if ((G_bunny_action == BUNNY_ACTION_JUMPING) || 
      (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE))
  {
    if ((G_bunny_gravity == BUNNY_GRAVITY_DOWN) && 
        (t->vel_y <= -THING_BUNNY_VEL_Y_CUTOFF))
    {
      t->vel_y = -THING_BUNNY_VEL_Y_RELEASE;
    }
    else if ( (G_bunny_gravity == BUNNY_GRAVITY_UP) && 
              (t->vel_y >= THING_BUNNY_VEL_Y_CUTOFF))
    {
      t->vel_y = THING_BUNNY_VEL_Y_RELEASE;
    }

    t->timer_offset = 0;
  }

  return 0;
}

/*******************************************************************************
** bunny_move_bunny()
*******************************************************************************/
short int bunny_move_bunny()
{
  thing* t;

  unsigned int adjusted_timer_count;

  t = &G_world_all_things[WORLD_BUNNY_INDEX];

  if (t->type != THING_TYPE_BUNNY)
    return 0;

  /* determine adjusted timer count */
  adjusted_timer_count = (G_timer_count + t->timer_offset) % 240;

  /* transition from grabbing to standing if necessary      */
  /* note that the bunny should be immobile during the grab */
  if (G_bunny_action == BUNNY_ACTION_GRABBING)
  {
    if (adjusted_timer_count >= THING_BUNNY_GRAB_TIME)
    {
      G_bunny_action = BUNNY_ACTION_STANDING;

      t->vel_x = 0;
      t->vel_y = 0;
      t->timer_offset = 0;
    }
    else
      return 0;
  }

  /* transition from throwing to standing if necessary        */
  /* note that the bunny should be immobile during the throw  */
  if (G_bunny_action == BUNNY_ACTION_THROWING)
  {
    if (adjusted_timer_count >= THING_BUNNY_THROW_TIME)
    {
      G_bunny_action = BUNNY_ACTION_STANDING;

      t->vel_x = 0;
      t->vel_y = 0;
      t->timer_offset = 0;
    }
    else
      return 0;
  }

  /* transition from kicking to standing if necessary       */
  /* note that the bunny should be immobile during the kick */
  if (G_bunny_action == BUNNY_ACTION_KICKING)
  {
    if (adjusted_timer_count >= THING_BUNNY_KICK_TIME)
    {
      G_bunny_action = BUNNY_ACTION_STANDING;

      t->vel_x = 0;
      t->vel_y = 0;
      t->timer_offset = 0;
    }
    else
      return 0;
  }

  /* update velocity based on gravity */
  if ((G_bunny_action == BUNNY_ACTION_STANDING)       || 
      (G_bunny_action == BUNNY_ACTION_ACCELERATING)   || 
      (G_bunny_action == BUNNY_ACTION_WALKING)        || 
      (G_bunny_action == BUNNY_ACTION_DECELERATING)   || 
      (G_bunny_action == BUNNY_ACTION_HOPPING)        || 
      (G_bunny_action == BUNNY_ACTION_JUMPING)        || 
      (G_bunny_action == BUNNY_ACTION_FALLING)        || 
      (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE) || 
      (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
  {
    if (G_bunny_gravity == BUNNY_GRAVITY_DOWN)
    {
      t->vel_y += THING_GRAVITATIONAL_ACC;

      if (t->vel_y > THING_TERMINAL_VEL)
        t->vel_y = THING_TERMINAL_VEL;
    }
    else if (G_bunny_gravity == BUNNY_GRAVITY_UP)
    {
      t->vel_y -= THING_GRAVITATIONAL_ACC;

      if (t->vel_y < -THING_TERMINAL_VEL)
        t->vel_y = -THING_TERMINAL_VEL;
    }
  }

  /* transition from jumping to falling if necessary */
  if (G_bunny_action == BUNNY_ACTION_JUMPING_DOUBLE)
  {
    if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && (t->vel_y > 0)) || 
        ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && (t->vel_y < 0)))
    {
      G_bunny_action = BUNNY_ACTION_FALLING_DOUBLE;
    }
  }

  if ((G_bunny_action == BUNNY_ACTION_JUMPING) || 
      (G_bunny_action == BUNNY_ACTION_HOPPING))
  {
    if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && (t->vel_y > 0)) || 
        ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && (t->vel_y < 0)))
    {
      G_bunny_action = BUNNY_ACTION_FALLING;
    }
  }

  /* move bunny horizontally, then vertically */
  grid_move_thing(WORLD_BUNNY_INDEX, GRID_MOVE_THING_MODE_HORIZONTAL, t->vel_x);

  if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
    t->vel_x = 0;

  grid_move_thing(WORLD_BUNNY_INDEX, GRID_MOVE_THING_MODE_VERTICAL, t->vel_y);

  if (t->flag == THING_COLLISION_FLAG_WE_HIT_SOMETHING)
  {
    /* revert to falling if moving down a ladder was blocked */
    if (G_bunny_action == BUNNY_ACTION_CLIMBING)
    {
      if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN) && (t->vel_y > 0)) || 
          ((G_bunny_gravity == BUNNY_GRAVITY_UP) && (t->vel_y < 0)))
      {
        G_bunny_action = BUNNY_ACTION_FALLING;
      }
    }

    /* play landing sound if we have been falling for more than 1 frame */
    if ((G_bunny_action == BUNNY_ACTION_FALLING) || 
        (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE))
    {
      if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN) && (t->vel_y > THING_GRAVITATIONAL_ACC)) || 
          ((G_bunny_gravity == BUNNY_GRAVITY_UP) && (t->vel_y < -THING_GRAVITATIONAL_ACC)))
      {
        doremi_play_sfx(SFX_INDEX_BUNNY_LANDED);
      }
    }

    t->vel_y = 0;
  }

  /* transition from walking to standing if necessary */
  if ((G_bunny_action == BUNNY_ACTION_WALKING)      || 
      (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
      (G_bunny_action == BUNNY_ACTION_DECELERATING))
  {
    if (t->vel_x == 0)
      G_bunny_action = BUNNY_ACTION_STANDING;
  }

  /* transition from walking / standing to falling if necessary */
  if ((G_bunny_action == BUNNY_ACTION_STANDING)     || 
      (G_bunny_action == BUNNY_ACTION_WALKING)      || 
      (G_bunny_action == BUNNY_ACTION_ACCELERATING) || 
      (G_bunny_action == BUNNY_ACTION_DECELERATING))
  {
    if (((G_bunny_gravity == BUNNY_GRAVITY_DOWN)  && (t->vel_y > 0)) || 
        ((G_bunny_gravity == BUNNY_GRAVITY_UP)    && (t->vel_y < 0)))
    {
      G_bunny_action = BUNNY_ACTION_FALLING;

      if (t->vel_x > 0)
        t->vel_x = THING_BUNNY_VEL_X_AIRSPEED;
      else if (t->vel_x < 0)
        t->vel_x = -THING_BUNNY_VEL_X_AIRSPEED;
    }
  }

  /* transition from falling to standing/walking if necessary */
  if (((G_bunny_action == BUNNY_ACTION_FALLING) || (G_bunny_action == BUNNY_ACTION_FALLING_DOUBLE)) && 
      (t->vel_y == 0))
  {
    if (t->vel_x > 0)
    {
      G_bunny_action = BUNNY_ACTION_WALKING;
      G_bunny_facing = BUNNY_FACING_RIGHT;

      t->vel_x = THING_BUNNY_VEL_X_WALK;

      t->timer_offset = 0;
    }
    else if (t->vel_x < 0)
    {
      G_bunny_action = BUNNY_ACTION_WALKING;
      G_bunny_facing = BUNNY_FACING_LEFT;

      t->vel_x = -THING_BUNNY_VEL_X_WALK;

      t->timer_offset = 0;
    }
    else
      G_bunny_action = BUNNY_ACTION_STANDING;
  }

  return 0;
}

