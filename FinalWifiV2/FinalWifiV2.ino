#include "settings.h"
#include <FastLED.h>
#include "ledcolor.h"
#include <WiFi.h>        // For connecting ESP32 to WiFi
#include <ArduinoOTA.h>  // For enabling over-the-air updates
#include <ServoInput.h>  // Include the ServoInput library

const char* ssid = "Perma";  // Change to your WiFi Network name
const char* password = "pelleplutt";  // Change to your password

#define LED1_PIN 3
#define LED2_PIN 5
#define LED3_PIN 7
#define LED4_PIN 9
#define LED_STRIP_PIN 12
#define RC_PIN 16
#define SWITCH_PIN 18

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

ServoInputPin<RC_PIN> rcInput(1000, 2000); // Create a ServoInput object for the RC_PIN
ServoInputPin<SWITCH_PIN> switchInput(1000, 2000); // Create a ServoInput object for the SWITCH_PIN

void setup() {

  WiFi.begin(ssid, password);  // Connect to WiFi - defaults to WiFi Station mode

  // Ensure WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  ArduinoOTA.begin();  // Starts OTA
  
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
  Serial.begin(115200);
}

void xenonEffect(int channel, int brightness) {
  // Snabb blinkning
  for (int i = 0; i < 3; i++) {
    ledcWrite(channel, brightness);
    delay(35);
    ledcWrite(channel, 0);
    delay(25);
  }
  // Mjukare övergång till full ljusstyrka
  for (int i = 0; i <= brightness; i += 2) {
    ledcWrite(channel, i);
    delay(15);
  }
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
  if (ljusstyrka_matris[state][channel-1] == 'd') {
    ledcWrite(channel, brightness);
    delay(50); // Snabbare blinkande effekt
    ledcWrite(channel, 0);
    delay(50); // Snabbare blinkande effekt
  } else if (ljusstyrka_matris[state][channel-1] == '1') {
    ledcWrite(channel, brightnessLevels[2]); // Full brightness
    delay(50); // Snabbare blinkande effekt
    ledcWrite(channel, 0);
    delay(50); // Snabbare blinkande effekt
  } else if (ljusstyrka_matris[state][channel-1] == '2') {
    ledcWrite(channel, brightnessLevels[1]); // Medium brightness
    delay(50); // Snabbare blinkande effekt
    ledcWrite(channel, 0);
    delay(50); // Snabbare blinkande effekt
  } else if (ljusstyrka_matris[state][channel-1] == '3') {
    ledcWrite(channel, brightnessLevels[0]); // Low brightness
    delay(50); // Snabbare blinkande effekt
    ledcWrite(channel, 0);
    delay(50); // Snabbare blinkande effekt
  }
}

void loop() {


  ArduinoOTA.handle();  // Handles a code update request

  unsigned long pulseWidth = rcInput.getPulseRaw(); // Use ServoInput to read the RC_PIN
  unsigned long switchValue = switchInput.getPulseRaw(); // Use ServoInput to read the SWITCH_PIN

    // Skriv ut pulsbredden till seriemonitorn

  //Serial.print("CH3:");
  //Serial.print(pulseWidth);
  //Serial.print(",");
  //Serial.print("CH4:");
  //Serial.println(switchValue);

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
    if (case_matris[state][i] == '1') {
      if (ljusstyrka_matris[state][i] == 'd') {
        ledcWrite(i + 1, brightnessLevels[brightnessIndex]);
      } else if (ljusstyrka_matris[state][i] == '1') {
        ledcWrite(i + 1, brightnessLevels[2]); // Full brightness
      } else if (ljusstyrka_matris[state][i] == '2') {
        ledcWrite(i + 1, brightnessLevels[1]); // Medium brightness
      } else if (ljusstyrka_matris[state][i] == '3') {
        ledcWrite(i + 1, brightnessLevels[0]); // Low brightness
      }
    } else if (case_matris[state][i] == 'x' && !effectDone[i]) {
      xenonEffect(i + 1, brightnessLevels[brightnessIndex]);
      effectDone[i] = true;
    } else if (case_matris[state][i] == 'f' && !effectDone[i]) {
      fadeEffect(i + 1, brightnessLevels[brightnessIndex]);
      effectDone[i] = true;
    } else if (case_matris[state][i] == 'b') {
      blinkEffect(i + 1, brightnessLevels[brightnessIndex]);
      // Inte sätta effectDone[i] till true här så att blinkande effekten fortsätter
    } else {
      ledcWrite(i + 1, 0);
    }
    // Uppdatera ljusstyrkan efter effekten
    if (effectDone[i]) {
      if (ljusstyrka_matris[state][i] == 'd') {
        ledcWrite(i + 1, brightnessLevels[brightnessIndex]);
      } else if (ljusstyrka_matris[state][i] == '1') {
        ledcWrite(i + 1, brightnessLevels[2]); // Full brightness
      } else if (ljusstyrka_matris[state][i] == '2') {
        ledcWrite(i + 1, brightnessLevels[1]); // Medium brightness
      } else if (ljusstyrka_matris[state][i] == '3') {
        ledcWrite(i + 1, brightnessLevels[0]); // Low brightness
      }
    }
  }

  // Hantera LED-strip
  if (case_matris[state][4] != lastStripState) {
    for (int i = 0; i < NUM_LEDS; i++) { // Loopa genom de första 4 LED-lamporna
      if (case_matris[state][4] == 'A') {
        leds[i] = ledColors[i][0]; // Färg för 'A'
      } else if (case_matris[state][4] == 'B') {
        leds[i] = ledColors[i][1]; // Färg för 'B'
      } else if (case_matris[state][4] == 'C') {
        leds[i] = ledColors[i][2]; // Färg för 'C'
      } else {
        leds[i] = CRGB::Black;
      }
    }
    FastLED.show();
    lastStripState = case_matris[state][4];
  }
}
