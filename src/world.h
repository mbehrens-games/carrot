/*******************************************************************************
** world.h (the world of right now)
*******************************************************************************/

#ifndef WORLD_H
#define WORLD_H

#include "subpixel.h"
#include "thing.h"

/* things array start / end indices and counts */

/* note that the maximum number of bunnies and held things is 1 */
/* of each, so we add an additional 2 to the max things amount  */
/* the maximum number of things should add up to 80!            */
#define WORLD_MAX_BUBBLES       24
#define WORLD_MAX_PLATFORMS_ETC 16
#define WORLD_MAX_PICKUPS       16
#define WORLD_MAX_CRITTERS      16
#define WORLD_MAX_THROWN_THINGS 6

#define WORLD_MAX_THINGS  ( WORLD_MAX_BUBBLES +                                \
                            WORLD_MAX_PLATFORMS_ETC +                          \
                            WORLD_MAX_PICKUPS +                                \
                            WORLD_MAX_CRITTERS +                               \
                            WORLD_MAX_THROWN_THINGS + 2)

#define WORLD_BUBBLES_START_INDEX         0
#define WORLD_BUBBLES_END_INDEX           (WORLD_BUBBLES_START_INDEX + WORLD_MAX_BUBBLES)

#define WORLD_PLATFORMS_ETC_START_INDEX   WORLD_BUBBLES_END_INDEX
#define WORLD_PLATFORMS_ETC_END_INDEX     (WORLD_PLATFORMS_ETC_START_INDEX + WORLD_MAX_PLATFORMS_ETC)

#define WORLD_PICKUPS_START_INDEX         WORLD_PLATFORMS_ETC_END_INDEX
#define WORLD_PICKUPS_END_INDEX           (WORLD_PICKUPS_START_INDEX + WORLD_MAX_PICKUPS)

#define WORLD_CRITTERS_START_INDEX        WORLD_PICKUPS_END_INDEX
#define WORLD_CRITTERS_END_INDEX          (WORLD_CRITTERS_START_INDEX + WORLD_MAX_CRITTERS)

#define WORLD_THROWN_THINGS_START_INDEX   WORLD_CRITTERS_END_INDEX
#define WORLD_THROWN_THINGS_END_INDEX     (WORLD_THROWN_THINGS_START_INDEX + WORLD_MAX_THROWN_THINGS)

#define WORLD_BUNNY_INDEX                 WORLD_THROWN_THINGS_END_INDEX
#define WORLD_HELD_THING_INDEX            (WORLD_BUNNY_INDEX + 1)

/* riders array start / end indices and counts */
#define WORLD_MAX_RIDERS_ON_CRITTER       16
#define WORLD_MAX_RIDERS_ON_PLATFORM      16
#define WORLD_MAX_RIDERS_UNDER_PLATFORM   16
#define WORLD_MAX_RIDERS_ON_THROWN_THING  6

#define WORLD_MAX_RIDERS  ( WORLD_MAX_RIDERS_ON_CRITTER +                      \
                            WORLD_MAX_RIDERS_ON_PLATFORM +                     \
                            WORLD_MAX_RIDERS_UNDER_PLATFORM +                  \
                            WORLD_MAX_RIDERS_ON_THROWN_THING)

#define WORLD_RIDERS_ON_CRITTER_START_INDEX       0
#define WORLD_RIDERS_ON_CRITTER_END_INDEX         (WORLD_RIDERS_ON_CRITTER_START_INDEX + WORLD_MAX_RIDERS_ON_CRITTER)

#define WORLD_RIDERS_ON_PLATFORM_START_INDEX      WORLD_RIDERS_ON_CRITTER_END_INDEX
#define WORLD_RIDERS_ON_PLATFORM_END_INDEX        (WORLD_RIDERS_ON_PLATFORM_START_INDEX + WORLD_MAX_RIDERS_ON_PLATFORM)

#define WORLD_RIDERS_UNDER_PLATFORM_START_INDEX   WORLD_RIDERS_ON_PLATFORM_END_INDEX
#define WORLD_RIDERS_UNDER_PLATFORM_END_INDEX     (WORLD_RIDERS_UNDER_PLATFORM_START_INDEX + WORLD_MAX_RIDERS_UNDER_PLATFORM)

#define WORLD_RIDERS_ON_THROWN_THING_START_INDEX  WORLD_RIDERS_UNDER_PLATFORM_END_INDEX
#define WORLD_RIDERS_ON_THROWN_THING_END_INDEX    (WORLD_RIDERS_ON_THROWN_THING_START_INDEX + WORLD_MAX_RIDERS_ON_THROWN_THING)

extern thing  G_world_all_things[WORLD_MAX_THINGS];
extern int    G_world_riders[WORLD_MAX_RIDERS];

/* function declarations */
short int world_reset();
short int world_init_riders();

short int world_spawn_thing(int type, int color, int state, int orient, 
                            int pos_x, int pos_y, int vel_x, int vel_y, 
                            unsigned int timer_offset);
short int world_despawn_thing(int index);

short int world_load_things_from_tilemap();

#endif
