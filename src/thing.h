/*******************************************************************************
** thing.h (things)
*******************************************************************************/

#ifndef THING_H
#define THING_H

enum
{
  THING_TYPE_NONE = 0,
  /* bunny */
  THING_TYPE_BUNNY,
  /* pickups */
  THING_TYPE_CARROT,
  THING_TYPE_BOOK,
  THING_TYPE_MUSIC_NOTES,
  THING_TYPE_MAGNET_DOWN,
  THING_TYPE_MAGNET_UP,
  THING_TYPE_MAGNET_RIGHT,
  THING_TYPE_MAGNET_LEFT,
  THING_TYPE_GEM,
  THING_TYPE_KEY,
  THING_TYPE_MUSHROOM,
  /* critters */
  THING_TYPE_CRITTER_WALKER_1,
  THING_TYPE_CRITTER_WALKER_2,
  THING_TYPE_CRITTER_FLYER_1,
  THING_TYPE_CRITTER_FLYER_2,
  /* platforms etc */
  THING_TYPE_BASS_KNOB,
  THING_TYPE_TREBLE_KNOB,
  THING_TYPE_ROCKET_BLOCK_RIGHT,
  THING_TYPE_ROCKET_BLOCK_LEFT,
  THING_TYPE_ELEVATOR_HORI_RIGHT,
  THING_TYPE_ELEVATOR_HORI_LEFT,
  THING_TYPE_ELEVATOR_VERT_UP,
  THING_TYPE_ELEVATOR_VERT_DOWN,
  THING_TYPE_DROP_BOLT_DOWN,
  THING_TYPE_DROP_BOLT_UP,
  THING_TYPE_ANTI_BOLT_UP,
  THING_TYPE_ANTI_BOLT_DOWN,
  THING_TYPE_PLATFORM,
  /* held */
  THING_TYPE_HELD_MARBLE,
  THING_TYPE_HELD_CRITTER_WALKER_1,
  THING_TYPE_HELD_CRITTER_WALKER_2,
  THING_TYPE_HELD_CRITTER_FLYER_1,
  THING_TYPE_HELD_CRITTER_FLYER_2,
  /* thrown */
  THING_TYPE_THROWN_MARBLE,
  THING_TYPE_THROWN_CRITTER_WALKER_1,
  THING_TYPE_THROWN_CRITTER_WALKER_2,
  THING_TYPE_THROWN_CRITTER_FLYER_1,
  THING_TYPE_THROWN_CRITTER_FLYER_2,
  /* bubbles */
  THING_TYPE_BUBBLE_HORIZONTAL,
  THING_TYPE_BUBBLE_VERTICAL,
  THING_TYPE_BUBBLE_STRESSED,
  THING_TYPE_BUBBLE_POPPED
};

enum
{
  THING_ORIENT_NORMAL,
  THING_ORIENT_FLIP_HORI,
  THING_ORIENT_FLIP_VERT,
  THING_ORIENT_ROTATE_180
};

enum
{
  THING_COLLISION_FLAG_NONE = 0,
  THING_COLLISION_FLAG_WE_HIT_SOMETHING,
  THING_COLLISION_FLAG_PICKUP_COLLECTED,
  THING_COLLISION_FLAG_BUBBLE_GOT_HIT
};

/* thing positions, velocities, and hitbox  */
/* sizes are stored in terms of subpixels   */
/* there are 16 subpixels per pixel         */
/* (in other words, these values are fixed  */
/* point with a 4 bit mantissa).            */

#define THING_BUNNY_VEL_X_WALK        20
#define THING_BUNNY_VEL_X_ACCEL       5
#define THING_BUNNY_VEL_X_DECEL       10

#define THING_BUNNY_VEL_X_AIRSPEED    20

#define THING_BUNNY_ACCEL_TIME_NORMAL 8
#define THING_BUNNY_DECEL_TIME_NORMAL 8

#define THING_BUNNY_ACCEL_TIME_ICE    20
#define THING_BUNNY_DECEL_TIME_ICE    28

/* note that the bunny's jump velocity is chosen so   */
/* that it will equal 256 exactly after 4 frames.     */
/* each frame, the velocity is decremented by the     */
/* gravitational acceleration before movement.        */
/* so, we have 70 + 66 + 62 + 58 = 256                */
/* this means the bunny can jump up 1 block into a    */
/* 1 block tall narrow opening and line up perfectly  */

/* similarly, the release velocity is chosen so that  */
/* if the jump button is released on the right frame, */
/* the total height travelled will be 512, which will */
/* allow the bunny to jump up 2 blocks into a 1 block */
/* tall opening and line up perfectly.                */
/* so, continuing from the first part of the jump, we */
/* 54 + 50 + 46 + 42 + 38 + 15 + 11 = 256             */
#define THING_BUNNY_VEL_Y_JUMP        74
#define THING_BUNNY_VEL_Y_CUTOFF      32
#define THING_BUNNY_VEL_Y_RELEASE     19

/* the hop velocity is chosen so that the hop will  */
/* last 8 frames, giving the bunny enough time to   */
/* clear the ladder before a d-pad up read will     */
/* cause the bunny to try to grab the ladder again  */
#define THING_BUNNY_VEL_Y_SUPER       120
#define THING_BUNNY_VEL_Y_HOP         31

#define THING_BUNNY_VEL_Y_CLIMBING    18

#define THING_BUNNY_GRAB_TIME         24
#define THING_BUNNY_THROW_TIME        24
#define THING_BUNNY_KICK_TIME         24

#define THING_TERMINAL_VEL            80
#define THING_GRAVITATIONAL_ACC       4

#define THING_BUBBLE_VEL              10

#define THING_CRITTER_VEL             12
#define THING_CRITTER_TURN_TIME       12
#define THING_CRITTER_STUNNED_TIME    24

#define THING_PLATFORM_VEL            16
#define THING_RETRACT_VEL             8

#define THING_ELEVATOR_TURN_TIME      12

#define THING_GRABBED_VEL             20

#define THING_THROWN_STRAIGHT_VEL     40
#define THING_THROWN_DIAGONAL_VEL     28
#define THING_THROWN_FALLING_VEL      20

typedef struct thing
{
  /* thing type, color, state, orientation */
  int type;
  int color;
  int state;
  int orient;

  /* thing collision flag */
  int flag;

  /* position (in subpixels) */
  int pos_x;
  int pos_y;

  /* velocity (in subpixels / frame) */
  int vel_x;
  int vel_y;

  /* hitbox half-width and half-height (in subpixels) */
  int hx;
  int hy;

  /* timer offset */
  unsigned int timer_offset;
} thing;

#endif
