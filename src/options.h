/*******************************************************************************
** options.h (options save)
*******************************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H

enum
{
  OPTIONS_ENTRY_WINDOW_SIZE = 0,
  OPTIONS_ENTRY_FULLSCREEN,
  OPTIONS_ENTRY_VSYNC,
  OPTIONS_ENTRY_VOLUME,
  OPTIONS_ENTRY_BUTTON_LAYOUT,
  OPTIONS_ENTRY_UPSCALING_MODE,
  OPTIONS_ENTRY_BRIGHTNESS,
  OPTIONS_ENTRY_CONTRAST,
  OPTIONS_ENTRY_TINT,
  OPTIONS_ENTRY_SATURATION,
  OPTIONS_ENTRY_GAMMA,
  OPTIONS_ENTRY_BLUR_AMOUNT,
  OPTIONS_ENTRY_PHOSPHOR_MASK,
  OPTIONS_NUM_ENTRIES
};

/* function declarations */
short int options_load();
short int options_save();

#endif
