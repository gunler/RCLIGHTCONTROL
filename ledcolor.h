// Definiera en 2D-matris för LED-färger
// Antalet rader i matrisen skall följa antalet "num_LEDS" i huvudskissen CRGB ledColors[6][3] = {
CRGB ledColors[4][3] = {
  // A              B             C               // Färg för A, B och C
  {CRGB::Red,     CRGB::Green,    CRGB::Blue},    // Färger för LED 1
  {CRGB::Red,     CRGB::Purple,   CRGB::Cyan},    // Färger för LED 2
  {CRGB::White,   CRGB::Orange,   CRGB::Pink},    // Färger för LED 3
  {CRGB::White,   CRGB::Gray,     CRGB::Brown}    // Färger för LED 4
//  {CRGB::White,   CRGB::Orange,   CRGB::Pink},  // Färger för LED 5
//  {CRGB::White,   CRGB::Gray,     CRGB::Brown}  // Färger för LED 6
};
