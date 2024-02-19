#ifndef SETTINGS_H
#define SETTINGS_H

#define NUM_CASES 6
#define NUM_BRIGHTNESS_LEVELS 3

char case_matris[NUM_CASES][5] = {
//led1  led2 led3 led4 strip  ==  1 front, 2 roof, 3 rear, 4 rack
  {'1', '0', '1', '0', '0'},   // case 1 //x = xenon
  {'1', 'x', '1', '0', '0'},   // case 2 //f = fade
  {'1', '1', '1', 'f', '0'},   // case 3 //b = blink
  {'0', '0', '0', '1', '0'},   // case 4 //0 = off
  {'b', 'b', 'b', 'b', '0'},   // case 5 //1 = on
  {'0', '0', '0', '0', '0'},   // case 6 //A,B,C = färger på strip (endast i kolumn 5) 0=strip off
};

char ljusstyrka_matris[NUM_CASES][4] = {
//led1  led2 led3 led4
  {'d', 'd', 'd', 'd'},   // case 1 //d = dimmas av switch
  {'d', 'd', 'd', 'd'},   // case 2 //1 = full ljusstyrka
  {'d', 'd', 'd', '3'},   // case 3 //2 = mellan ljusstyrka
  {'d', 'd', 'd', 'd'},   // case 4 //3 = låg ljusstyrka
  {'1', '1', '1', '1'},   // case 6 
  {'d', 'd', 'd', 'd'},   // case 6
};

int brightnessLevels[NUM_BRIGHTNESS_LEVELS] = {20, 100, 255}; // Brightness levels

#endif
