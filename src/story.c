/*******************************************************************************
** story.c (story scenes)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "story.h"

char G_story_scene_titles[STORY_NUM_SCENES][STORY_TITLE_SIZE];
char G_story_scene_lines[STORY_NUM_SCENES][STORY_LINES_PER_SCENE][STORY_LINE_SIZE];

/*******************************************************************************
** story_init_arrays()
*******************************************************************************/
short int story_init_arrays()
{
  int m;
  int n;
  int k;

  /* initialize story scene titles array */
  for (m = 0; m < STORY_NUM_SCENES; m++)
  {
    for (k = 0; k < STORY_TITLE_SIZE; k++)
    {
      G_story_scene_titles[m][k] = '\0';
    }
  }

  /* initialize story scene text array */
  for (m = 0; m < STORY_NUM_SCENES; m++)
  {
    for (n = 0; n < STORY_LINES_PER_SCENE; n++)
    {
      for (k = 0; k < STORY_LINE_SIZE; k++)
      {
        G_story_scene_lines[m][n][k] = '\0';
      }
    }
  }

  return 0;
}

/*******************************************************************************
** story_load_all_from_file()
*******************************************************************************/
short int story_load_all_from_file(char* filename)
{
  FILE* fp;

  int   m;
  int   n;
  int   k;

  char  signature[4];
  char  type[4];

  if (filename == NULL)
    return 1;

  /* open file */
  fp = fopen(filename, "rb");

  /* if file did not open, return error */
  if (fp == NULL)
  {
    fclose(fp);
    return 1;
  }

  /* read signature */
  if (fread(signature, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if ((signature[0] != 'C') || 
      (signature[1] != 'A') || 
      (signature[2] != 'R') || 
      (signature[3] != 'R'))
  {
    fclose(fp);
    return 1;
  }

  /* read type */
  if (fread(type, 1, 4, fp) < 4)
  {
    fclose(fp);
    return 1;
  }

  if ((type[0] != 'S') || 
      (type[1] != 'T') || 
      (type[2] != 'R') || 
      (type[3] != 'Y'))
  {
    fclose(fp);
    return 1;
  }

  /* read story scene titles & lines */
  for (m = 0; m < STORY_NUM_SCENES; m++)
  {
    /* read title */
    if (fread(&G_story_scene_titles[m][0], 1, STORY_TITLE_SIZE, fp) == 0)
    {
      fclose(fp);
      return 1;
    }

    /* read lines */
    for (n = 0; n < STORY_LINES_PER_SCENE; n++)
    {
      if (fread(&G_story_scene_lines[m][n][0], 1, STORY_LINE_SIZE, fp) == 0)
      {
        fclose(fp);
        return 1;
      }
    }
  }

  /* decode text data, and convert any      */
  /* invalid characters to null terminators */

  /* the ascii range used is 32 to 127 */
  /* the coded values are from 1 to 96 */
  for (m = 0; m < STORY_NUM_SCENES; m++)
  {
    for (k = 0; k < STORY_TITLE_SIZE; k++)
    {
      if ((G_story_scene_titles[m][k] >= 1) && (G_story_scene_titles[m][k] <= 96))
        G_story_scene_titles[m][k] = G_story_scene_titles[m][k] + 31;
      else
        G_story_scene_titles[m][k] = '\0';
    }

    for (n = 0; n < STORY_LINES_PER_SCENE; n++)
    {
      for (k = 0; k < STORY_LINE_SIZE; k++)
      {
        if ((G_story_scene_lines[m][n][k] >= 1) && (G_story_scene_lines[m][n][k] <= 96))
          G_story_scene_lines[m][n][k] = G_story_scene_lines[m][n][k] + 31;
        else
          G_story_scene_lines[m][n][k] = '\0';
      }
    }
  }

  /* close file */
  fclose(fp);

  return 0;
}
