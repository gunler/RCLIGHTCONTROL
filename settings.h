#ifndef SETTINGS_H
#define SETTINGS_H

#define NUM_CASES 6
#define NUM_BRIGHTNESS_LEVELS 3

char cases[NUM_CASES][6] = {
  {'0', '0', '1', '0', '0', '3'},   // All off, first LED controlled by switch
  {'0', '1', '0', '0', 'B', 'd'},   // First LED on, LED strip color A
  {'0', 'x', '0', '0', '0', 'd'},   // Second LED on, LED strip color B
  {'0', '0', 'f', '0', 'C', 'd'},   // Third LED on, LED strip color C
  {'0', '0', '0', '0', '0', '0'},   // Fourth LED on, LED strip color A
  {'0', '0', 'b', '0', 'A', '1'},   // All on with xenon effect, LED strip color B
};

int brightnessLevels[NUM_BRIGHTNESS_LEVELS] = {20, 100, 255}; // Brightness levels

#endif
