/*******************************************************************************
** world.h (the world of right now)
*******************************************************************************/

#ifndef WORLD_H
#define WORLD_H

#include "thing.h"

#define WORLD_MAX_THINGS  80

#define WORLD_BUNNY_INDEX               78
#define WORLD_HELD_THING_INDEX          79

#define WORLD_BUBBLES_START_INDEX       0
#define WORLD_PLATFORMS_ETC_START_INDEX 24
#define WORLD_PICKUPS_START_INDEX       40
#define WORLD_CRITTERS_START_INDEX      56
#define WORLD_THROWN_THINGS_START_INDEX 72

#define WORLD_BUBBLES_END_INDEX         (WORLD_BUBBLES_START_INDEX + 24)
#define WORLD_PLATFORMS_ETC_END_INDEX   (WORLD_PLATFORMS_ETC_START_INDEX + 16)
#define WORLD_PICKUPS_END_INDEX         (WORLD_PICKUPS_START_INDEX + 16)
#define WORLD_CRITTERS_END_INDEX        (WORLD_CRITTERS_START_INDEX + 16)
#define WORLD_THROWN_THINGS_END_INDEX   (WORLD_THROWN_THINGS_START_INDEX + 6)

#define WORLD_MAX_RIDERS  54

#define WORLD_RIDERS_ON_CRITTER_START_INDEX       0
#define WORLD_RIDERS_ON_PLATFORM_START_INDEX      16
#define WORLD_RIDERS_UNDER_PLATFORM_START_INDEX   32
#define WORLD_RIDERS_ON_THROWN_THING_START_INDEX  48

#define WORLD_RIDERS_ON_CRITTER_END_INDEX       (WORLD_RIDERS_ON_CRITTER_START_INDEX + 16)
#define WORLD_RIDERS_ON_PLATFORM_END_INDEX      (WORLD_RIDERS_ON_PLATFORM_START_INDEX + 16)
#define WORLD_RIDERS_UNDER_PLATFORM_END_INDEX   (WORLD_RIDERS_UNDER_PLATFORM_START_INDEX + 16)
#define WORLD_RIDERS_ON_THROWN_THING_END_INDEX  (WORLD_RIDERS_ON_THROWN_THING_START_INDEX + 6)

#define WORLD_WIDTH_IN_SUBPIXELS  (320 * THING_NUM_SUBPIXELS)
#define WORLD_HEIGHT_IN_SUBPIXELS (224 * THING_NUM_SUBPIXELS)

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
