#ifndef SETTINGS_H
#define SETTINGS_H

#define NUM_CASES 6
#define NUM_BRIGHTNESS_LEVELS 3

char cases[NUM_CASES][6] = {
//led1  led2  led3  led4  strip  dim     // x=xenon f=fade b=blink A,B,C = färg enligt ledcolor.h (endast på strip) d, dim med switch 1,2,3 ljustyrka
//  1     x     f     b     A     d      // EX. led1 on, led2 xenon, led3 fade, led4 blink, strip färg A, Led dimmas med 3switchknapp
  {'0',  '0',  '1',  '0',  '0',  '3'},   // Läge 1  
  {'0',  '1',  '0',  '0',  'B',  'd'},   // Läge 2
  {'0',  'x',  '0',  '0',  '0',  'd'},   // Läge 3
  {'0',  '0',  'f',  '0',  'C',  'd'},   // Läge 4
  {'0',  '0',  '0',  '0',  '0',  '0'},   // Läge 5
  {'0',  '0',  'b',  '0',  'A',  '1'},   // Läge 6
};

int brightnessLevels[NUM_BRIGHTNESS_LEVELS] = {20, 100, 255}; // Brightness levels

#endif
