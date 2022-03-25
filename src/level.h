/*******************************************************************************
** level.h (level)
*******************************************************************************/

#ifndef LEVEL_H
#define LEVEL_H

/* sky tiles */
#define LEVEL_SKY_TILE_1  0x00
#define LEVEL_SKY_TILE_2  0x01
#define LEVEL_SKY_TILE_3  0x02
#define LEVEL_SKY_TILE_4  0x03
#define LEVEL_SKY_TILE_5  0x04
#define LEVEL_SKY_TILE_6  0x05
#define LEVEL_SKY_TILE_7  0x06
#define LEVEL_SKY_TILE_8  0x07

/* with 32x32 tiles, each screen consists of 10x7 tiles */
#define LEVEL_SKY_WIDTH     10
#define LEVEL_SKY_HEIGHT    7
#define LEVEL_SKY_NUM_TILES (LEVEL_SKY_WIDTH * LEVEL_SKY_HEIGHT)

#define LEVEL_SKY_TILE_SIZE       32
#define LEVEL_SKY_TILE_SIZE_HALF  (LEVEL_SKY_TILE_SIZE / 2)

#define LEVEL_NUM_SKYS 8

/* backdrop tiles */
#define LEVEL_BACKDROP_TILE_OPEN                      0x00
#define LEVEL_BACKDROP_TILE_MIDDLE                    0x01
#define LEVEL_BACKDROP_TILE_SIDE_TOP                  0x02
#define LEVEL_BACKDROP_TILE_SIDE_BOTTOM               0x03
#define LEVEL_BACKDROP_TILE_SIDE_LEFT                 0x04
#define LEVEL_BACKDROP_TILE_SIDE_RIGHT                0x05
#define LEVEL_BACKDROP_TILE_OUTER_CORNER_TOP_LEFT     0x06
#define LEVEL_BACKDROP_TILE_OUTER_CORNER_TOP_RIGHT    0x07
#define LEVEL_BACKDROP_TILE_OUTER_CORNER_BOTTOM_LEFT  0x08
#define LEVEL_BACKDROP_TILE_OUTER_CORNER_BOTTOM_RIGHT 0x09
#define LEVEL_BACKDROP_TILE_INNER_CORNER_TOP_LEFT     0x0A
#define LEVEL_BACKDROP_TILE_INNER_CORNER_TOP_RIGHT    0x0B
#define LEVEL_BACKDROP_TILE_INNER_CORNER_BOTTOM_LEFT  0x0C
#define LEVEL_BACKDROP_TILE_INNER_CORNER_BOTTOM_RIGHT 0x0D

/* room tiles */
#define LEVEL_ROOM_TILE_BLANK                         0x00
#define LEVEL_ROOM_TILE_PLAYER_START                  0x01
#define LEVEL_ROOM_TILE_CARROT                        0x02
#define LEVEL_ROOM_TILE_BOOK                          0x03
#define LEVEL_ROOM_TILE_MUSIC_NOTES_ORANGE            0x04
#define LEVEL_ROOM_TILE_MUSIC_NOTES_PINK              0x05
#define LEVEL_ROOM_TILE_MUSIC_NOTES_BLUE              0x06
#define LEVEL_ROOM_TILE_MUSIC_NOTES_GREEN             0x07

#define LEVEL_ROOM_TILE_GEM_METAL                     0x08
#define LEVEL_ROOM_TILE_GEM_ORANGE                    0x09
#define LEVEL_ROOM_TILE_GEM_PINK                      0x0A
#define LEVEL_ROOM_TILE_GEM_BLUE                      0x0B
#define LEVEL_ROOM_TILE_GEM_GREEN                     0x0C
#define LEVEL_ROOM_TILE_GEM_WILD                      0x0D
#define LEVEL_ROOM_TILE_METAL_BALL                    0x0E
#define LEVEL_ROOM_TILE_WILD_CARD                     0x0F

#define LEVEL_ROOM_TILE_BLOCK_SINGLE                  0x10
#define LEVEL_ROOM_TILE_BLOCK_LEFT                    0x11
#define LEVEL_ROOM_TILE_BLOCK_RIGHT                   0x12
#define LEVEL_ROOM_TILE_LADDER                        0x13
#define LEVEL_ROOM_TILE_MARBLE_ORANGE                 0x14
#define LEVEL_ROOM_TILE_MARBLE_PINK                   0x15
#define LEVEL_ROOM_TILE_MARBLE_BLUE                   0x16
#define LEVEL_ROOM_TILE_MARBLE_GREEN                  0x17

#define LEVEL_ROOM_TILE_ICE_SINGLE                    0x18
#define LEVEL_ROOM_TILE_ICE_LEFT                      0x19
#define LEVEL_ROOM_TILE_ICE_RIGHT                     0x1A
#define LEVEL_ROOM_TILE_HEART_BLOCK                   0x1B
#define LEVEL_ROOM_TILE_DIAMOND_BLOCK                 0x1C
#define LEVEL_ROOM_TILE_MAGNET_DOWN                   0x1D
#define LEVEL_ROOM_TILE_MAGNET_RIGHT                  0x1E
#define LEVEL_ROOM_TILE_MAGNET_LEFT                   0x1F

#define LEVEL_ROOM_TILE_CRITTER_WALKER_1_RIGHT        0x20
#define LEVEL_ROOM_TILE_CRITTER_WALKER_1_LEFT         0x21
#define LEVEL_ROOM_TILE_CRITTER_WALKER_2_RIGHT        0x22
#define LEVEL_ROOM_TILE_CRITTER_WALKER_2_LEFT         0x23
#define LEVEL_ROOM_TILE_CRITTER_FLYER_1_RIGHT         0x24
#define LEVEL_ROOM_TILE_CRITTER_FLYER_1_LEFT          0x25
#define LEVEL_ROOM_TILE_CRITTER_FLYER_2_UP            0x26
#define LEVEL_ROOM_TILE_CRITTER_FLYER_2_DOWN          0x27

#define LEVEL_ROOM_TILE_BASS_KNOB                     0x28
#define LEVEL_ROOM_TILE_TREBLE_KNOB                   0x29
#define LEVEL_ROOM_TILE_ROCKET_BLOCK_RIGHT            0x2A
#define LEVEL_ROOM_TILE_ROCKET_BLOCK_LEFT             0x2B
#define LEVEL_ROOM_TILE_ELEVATOR_HORI_RIGHT           0x2C
#define LEVEL_ROOM_TILE_ELEVATOR_HORI_LEFT            0x2D
#define LEVEL_ROOM_TILE_ELEVATOR_VERT_UP              0x2E
#define LEVEL_ROOM_TILE_ELEVATOR_VERT_DOWN            0x2F

#define LEVEL_ROOM_TILE_DROP_BOLT_DOWN                0x30
#define LEVEL_ROOM_TILE_DROP_BOLT_UP                  0x31
#define LEVEL_ROOM_TILE_MAGI_BLOCK_1010               0x32
#define LEVEL_ROOM_TILE_MAGI_BLOCK_0101               0x33
#define LEVEL_ROOM_TILE_MAGI_BLOCK_1000               0x34
#define LEVEL_ROOM_TILE_MAGI_BLOCK_0100               0x35
#define LEVEL_ROOM_TILE_MAGI_BLOCK_0010               0x36
#define LEVEL_ROOM_TILE_MAGI_BLOCK_0001               0x37

#define LEVEL_ROOM_TILE_MAGI_BLOCK_1100               0x38
#define LEVEL_ROOM_TILE_MAGI_BLOCK_0110               0x39
#define LEVEL_ROOM_TILE_MAGI_BLOCK_0011               0x3A
#define LEVEL_ROOM_TILE_MAGI_BLOCK_1001               0x3B
#define LEVEL_ROOM_TILE_MAGI_BLOCK_0111               0x3C
#define LEVEL_ROOM_TILE_MAGI_BLOCK_1011               0x3D
#define LEVEL_ROOM_TILE_MAGI_BLOCK_1101               0x3E
#define LEVEL_ROOM_TILE_MAGI_BLOCK_1110               0x3F

#define LEVEL_ROOM_TILE_ANTI_BOLT_UP                  0x40
#define LEVEL_ROOM_TILE_ANTI_BOLT_DOWN                0x41
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1010          0x42
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0101          0x43
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1000          0x44
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0100          0x45
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0010          0x46
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0001          0x47

#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1100          0x48
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0110          0x49
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0011          0x4A
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1001          0x4B
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_0111          0x4C
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1011          0x4D
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1101          0x4E
#define LEVEL_ROOM_TILE_FAST_MAGI_BLOCK_1110          0x4F

#define LEVEL_ROOM_TILE_KEY_ORANGE                    0x50
#define LEVEL_ROOM_TILE_KEY_PINK                      0x51
#define LEVEL_ROOM_TILE_KEY_BLUE                      0x52
#define LEVEL_ROOM_TILE_KEY_GREEN                     0x53
#define LEVEL_ROOM_TILE_DOOR_BLOCK_ORANGE             0x54
#define LEVEL_ROOM_TILE_DOOR_BLOCK_PINK               0x55
#define LEVEL_ROOM_TILE_DOOR_BLOCK_BLUE               0x56
#define LEVEL_ROOM_TILE_DOOR_BLOCK_GREEN              0x57

#define LEVEL_ROOM_TILE_MUSHROOM_ORANGE               0x58
#define LEVEL_ROOM_TILE_MUSHROOM_PINK                 0x59
#define LEVEL_ROOM_TILE_MUSHROOM_BLUE                 0x5A
#define LEVEL_ROOM_TILE_MUSHROOM_GREEN                0x5B
#define LEVEL_ROOM_TILE_INVISIBLE_BLOCK_ORANGE        0x5C
#define LEVEL_ROOM_TILE_INVISIBLE_BLOCK_PINK          0x5D
#define LEVEL_ROOM_TILE_INVISIBLE_BLOCK_BLUE          0x5E
#define LEVEL_ROOM_TILE_INVISIBLE_BLOCK_GREEN         0x5F

#define LEVEL_ROOM_TILE_BUBBLE_POT_1_UP               0x60
#define LEVEL_ROOM_TILE_BUBBLE_POT_2_UP               0x61
#define LEVEL_ROOM_TILE_BUBBLE_POT_3_UP               0x62
#define LEVEL_ROOM_TILE_BUBBLE_POT_1_RIGHT            0x63
#define LEVEL_ROOM_TILE_BUBBLE_POT_2_RIGHT            0x64
#define LEVEL_ROOM_TILE_BUBBLE_POT_3_RIGHT            0x65
#define LEVEL_ROOM_TILE_PLATFORM_RIGHT                0x66
#define LEVEL_ROOM_TILE_PLATFORM_LEFT                 0x67

#define LEVEL_ROOM_TILE_BUBBLE_POT_1_DOWN             0x68
#define LEVEL_ROOM_TILE_BUBBLE_POT_2_DOWN             0x69
#define LEVEL_ROOM_TILE_BUBBLE_POT_3_DOWN             0x6A
#define LEVEL_ROOM_TILE_BUBBLE_POT_1_LEFT             0x6B
#define LEVEL_ROOM_TILE_BUBBLE_POT_2_LEFT             0x6C
#define LEVEL_ROOM_TILE_BUBBLE_POT_3_LEFT             0x6D
#define LEVEL_ROOM_TILE_PLATFORM_UP                   0x6E
#define LEVEL_ROOM_TILE_PLATFORM_DOWN                 0x6F

#define LEVEL_ROOM_TILE_TRACK_RIGHT                   0x70
#define LEVEL_ROOM_TILE_TRACK_LEFT                    0x71
#define LEVEL_ROOM_TILE_TRACK_UP                      0x72
#define LEVEL_ROOM_TILE_TRACK_DOWN                    0x73
#define LEVEL_ROOM_TILE_TRACK_CORNER_TOP_RIGHT        0x74
#define LEVEL_ROOM_TILE_TRACK_CORNER_TOP_LEFT         0x75
#define LEVEL_ROOM_TILE_TRACK_CORNER_BOTTOM_LEFT      0x76
#define LEVEL_ROOM_TILE_TRACK_CORNER_BOTTOM_RIGHT     0x77

#define LEVEL_ROOM_TILE_ARROWS_000                    0x78
#define LEVEL_ROOM_TILE_ARROWS_045                    0x79
#define LEVEL_ROOM_TILE_ARROWS_090                    0x7A
#define LEVEL_ROOM_TILE_ARROWS_135                    0x7B
#define LEVEL_ROOM_TILE_ARROWS_180                    0x7C
#define LEVEL_ROOM_TILE_ARROWS_225                    0x7D
#define LEVEL_ROOM_TILE_ARROWS_270                    0x7E
#define LEVEL_ROOM_TILE_ARROWS_315                    0x7F

/* with 16x16 tiles, each screen consists of 20x14 tiles  */
#define LEVEL_ROOM_WIDTH      20
#define LEVEL_ROOM_HEIGHT     14
#define LEVEL_ROOM_NUM_TILES  (LEVEL_ROOM_WIDTH * LEVEL_ROOM_HEIGHT)

#define LEVEL_ROOM_TILE_SIZE      16
#define LEVEL_ROOM_TILE_SIZE_HALF (LEVEL_ROOM_TILE_SIZE / 2)

#define LEVEL_NUM_BACKDROPS   32
#define LEVEL_NUM_ROOMS       96

#define LEVEL_NUM_WORLDS      12
#define LEVEL_ROOMS_PER_WORLD 8

#define LEVEL_WORLD_NAME_SIZE 16
#define LEVEL_ROOM_NAME_SIZE  16

/* global variables */
extern unsigned char G_sky_tilemaps[LEVEL_NUM_SKYS][LEVEL_SKY_NUM_TILES];
extern unsigned char G_backdrop_tilemaps[LEVEL_NUM_BACKDROPS][LEVEL_ROOM_NUM_TILES];
extern unsigned char G_room_tilemaps[LEVEL_NUM_ROOMS][LEVEL_ROOM_NUM_TILES];

extern char G_world_names[LEVEL_NUM_WORLDS][LEVEL_WORLD_NAME_SIZE];
extern char G_room_names[LEVEL_NUM_ROOMS][LEVEL_ROOM_NAME_SIZE];

/* function declarations */
short int level_init_arrays();

short int level_load_all_from_file(char* filename);

#endif
