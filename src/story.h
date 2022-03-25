/*******************************************************************************
** story.h (story scenes)
*******************************************************************************/

#ifndef STORY_H
#define STORY_H

#define STORY_NUM_SCENES      4
#define STORY_TITLE_SIZE      16

#define STORY_LINES_PER_SCENE 10
#define STORY_LINE_SIZE       32

extern char G_story_scene_titles[STORY_NUM_SCENES][STORY_TITLE_SIZE];
extern char G_story_scene_lines[STORY_NUM_SCENES][STORY_LINES_PER_SCENE][STORY_LINE_SIZE];

/* function declarations */
short int story_init_arrays();
short int story_load_all_from_file(char* filename);

#endif
