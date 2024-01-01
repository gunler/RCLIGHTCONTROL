#include "settings.h"
#include <FastLED.h>
#include "ledcolor.h"

#define LED1_PIN 5
#define LED2_PIN 4
#define LED3_PIN 6
#define LED4_PIN 7
#define LED_STRIP_PIN 9
#define RC_PIN 16
#define SWITCH_PIN 17

#define LEDC_CHANNEL_1 1
#define LEDC_CHANNEL_2 2
#define LEDC_CHANNEL_3 3
#define LEDC_CHANNEL_4 4
#define LEDC_RESOLUTION 8
#define LEDC_FREQUENCY 5000

#define NUM_LEDS 4 // Antal LED-lampor i LED-strip

CRGB leds[NUM_LEDS]; // Lägg till denna rad för att deklarera leds

volatile int state = 0;
unsigned long lastPressTime = 0;
const unsigned long debounceTime = 200;
const unsigned long pulseThreshold = 1500; // Ändra detta värde till lämpligt tröskelvärde
bool wasAboveThreshold = false;

bool effectDone[6] = {false, false, false, false, false, false}; // Track if the effect has been done
char lastStripState = '0'; // Track the last state of the LED strip

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  pinMode(RC_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT);

  // Konfigurera PWM-kanaler
  ledcSetup(LEDC_CHANNEL_1, LEDC_FREQUENCY, LEDC_RESOLUTION);
  ledcSetup(LEDC_CHANNEL_2, LEDC_FREQUENCY, LEDC_RESOLUTION);
  ledcSetup(LEDC_CHANNEL_3, LEDC_FREQUENCY, LEDC_RESOLUTION);
  ledcSetup(LEDC_CHANNEL_4, LEDC_FREQUENCY, LEDC_RESOLUTION);

  // Koppla PWM-kanaler till LED-pinnar
  ledcAttachPin(LED1_PIN, LEDC_CHANNEL_1);
  ledcAttachPin(LED2_PIN, LEDC_CHANNEL_2);
  ledcAttachPin(LED3_PIN, LEDC_CHANNEL_3);
  ledcAttachPin(LED4_PIN, LEDC_CHANNEL_4);

  // Konfigurera LED-strip
  FastLED.addLeds<WS2812B, LED_STRIP_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();  // clear all pixel data

  // Starta seriell kommunikation
  Serial.begin(9600);
}

void xenonEffect(int channel, int brightness) {
  for (int i = 0; i < 3; i++) {
    ledcWrite(channel, brightness);
    delay(25);
    ledcWrite(channel, 0);
    delay(55);
  }
  // Behåll ljusstyrkan efter effekten
  ledcWrite(channel, brightness);
}

void fadeEffect(int channel, int brightness) {
  for (int i = 0; i <= brightness; i += 5) {
    ledcWrite(channel, i);
    delay(20); // Dubbla tiden för fade-effekten
  }
  // Behåll ljusstyrkan efter effekten
  ledcWrite(channel, brightness);
}

void blinkEffect(int channel, int brightness) {
  if (cases[state][5] == 'd') {
    ledcWrite(channel, brightness);
    delay(50); // Snabbare blinkande effekt
    ledcWrite(channel, 0);
    delay(50); // Snabbare blinkande effekt
  } else if (cases[state][5] == '1') {
    ledcWrite(channel, brightnessLevels[2]); // Full brightness
    delay(50); // Snabbare blinkande effekt
    ledcWrite(channel, 0);
    delay(50); // Snabbare blinkande effekt
  } else if (cases[state][5] == '2') {
    ledcWrite(channel, brightnessLevels[1]); // Medium brightness
    delay(50); // Snabbare blinkande effekt
    ledcWrite(channel, 0);
    delay(50); // Snabbare blinkande effekt
  } else if (cases[state][5] == '3') {
    ledcWrite(channel, brightnessLevels[0]); // Low brightness
    delay(50); // Snabbare blinkande effekt
    ledcWrite(channel, 0);
    delay(50); // Snabbare blinkande effekt
  }
}

void loop() {
  unsigned long pulseWidth = pulseIn(RC_PIN, HIGH);
  unsigned long switchValue = pulseIn(SWITCH_PIN, HIGH);

  // Skriv ut pulsbredden till seriemonitorn
  Serial.println(pulseWidth);

  if (pulseWidth <= pulseThreshold && wasAboveThreshold && millis() - lastPressTime > debounceTime) {
    state = (state + 1) % NUM_CASES;
    lastPressTime = millis();
    wasAboveThreshold = false;
    for (int i = 0; i < 6; i++) {
      effectDone[i] = false; // Reset the effect done flags
    }
  } else if (pulseWidth > pulseThreshold) {
    wasAboveThreshold = true;
  }

  int brightnessIndex;
  if (switchValue < 1250) {
    brightnessIndex = 0; // Låg ljusstyrka
  } else if (switchValue < 1750) {
    brightnessIndex = 1; // Mellanljusstyrka
  } else {
    brightnessIndex = 2; // Hög ljusstyrka
  }

  for (int i = 0; i < 5; i++) {
    if (cases[state][i] == '1') {
      if (cases[state][5] == 'd') {
        ledcWrite(i + 1, brightnessLevels[brightnessIndex]);
      } else if (cases[state][5] == '1') {
        ledcWrite(i + 1, brightnessLevels[2]); // Full brightness
      } else if (cases[state][5] == '2') {
        ledcWrite(i + 1, brightnessLevels[1]); // Medium brightness
      } else if (cases[state][5] == '3') {
        ledcWrite(i + 1, brightnessLevels[0]); // Low brightness
      }
    } else if (cases[state][i] == 'x' && !effectDone[i]) {
      xenonEffect(i + 1, brightnessLevels[brightnessIndex]);
      effectDone[i] = true;
    } else if (cases[state][i] == 'f' && !effectDone[i]) {
      fadeEffect(i + 1, brightnessLevels[brightnessIndex]);
      effectDone[i] = true;
    } else if (cases[state][i] == 'b') {
      blinkEffect(i + 1, brightnessLevels[brightnessIndex]);
      // Inte sätta effectDone[i] till true här så att blinkande effekten fortsätter
    } else {
      ledcWrite(i + 1, 0);
    }
    // Uppdatera ljusstyrkan efter effekten
    if (effectDone[i]) {
      if (cases[state][5] == 'd') {
        ledcWrite(i + 1, brightnessLevels[brightnessIndex]);
      } else if (cases[state][5] == '1') {
        ledcWrite(i + 1, brightnessLevels[2]); // Full brightness
      } else if (cases[state][5] == '2') {
        ledcWrite(i + 1, brightnessLevels[1]); // Medium brightness
      } else if (cases[state][5] == '3') {
        ledcWrite(i + 1, brightnessLevels[0]); // Low brightness
      }
    }
  }

  // Hantera LED-strip
  if (cases[state][4] != lastStripState) {
    for (int i = 0; i < NUM_LEDS; i++) { // Loopa genom de första 4 LED-lamporna
      if (cases[state][4] == 'A') {
        leds[i] = ledColors[i][0]; // Färg för 'A'
      } else if (cases[state][4] == 'B') {
        leds[i] = ledColors[i][1]; // Färg för 'B'
      } else if (cases[state][4] == 'C') {
        leds[i] = ledColors[i][2]; // Färg för 'C'
      } else {
        leds[i] = CRGB::Black;
      }
    }
    FastLED.show();
    lastStripState = cases[state][5];
  }
}
