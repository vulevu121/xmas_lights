/*
  Copyright (c) 2014-2016 NicoHood
  See the readme for credit to other people.

  MSGEQ7 FastLED example
  Output via Led strip and FastLED library

  Reads MSGEQ7 IC with 7 different frequencies from range 0-255
  63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25KHz, 16kHz
*/

// FastLED
#include "FastLED.h"

#define LED_PINS    2 // DATA_PIN, or DATA_PIN, CLOCK_PIN
#define COLOR_ORDER GRB
#define CHIPSET     WS2811 // WS2811 LPD8806
#define NUM_LEDS    246

#define BRIGHTNESS  160  // reduce power consumption
#define LED_DITHER  255  // try 0 to disable flickering
#define CORRECTION  TypicalLEDStrip

CRGB leds[NUM_LEDS]; // Define the array of leds

// MSGEQ7
#include "MSGEQ7.h"
#define pinAnalogLeft A0
#define pinAnalogRight A1
#define pinReset 5
#define pinStrobe 4
#define MSGEQ7_INTERVAL ReadsPerSecond(50)
#define MSGEQ7_SMOOTH true

#define LEVEL1 100
#define LEVEL2 150
#define LEVEL3 200

CMSGEQ7<MSGEQ7_SMOOTH, pinReset, pinStrobe, pinAnalogLeft, pinAnalogRight> MSGEQ7;

CRGBPalette16 gPal;

void setup() {
  // FastLED setup
  FastLED.addLeds<CHIPSET, LED_PINS, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(CORRECTION);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.setDither(LED_DITHER);
  FastLED.show(); // needed to reset leds to zero

  // This will set the IC ready for reading
  MSGEQ7.begin();
}

void loop() {
  // Analyze without delay
  bool newReading = MSGEQ7.read(MSGEQ7_INTERVAL);

  int i;
  
  // Led strip output
  if (newReading) {
    // visualize the average bass of both channels
    uint8_t val = MSGEQ7.get(MSGEQ7_BASS);

    val = mapNoise(val);
    CRGB color = CRGB::Blue;
    color.nscale8_video(val);

    for(i = 0; i < LEVEL1 ; i++) {
      leds[i] = color;
    }

    val = MSGEQ7.get(MSGEQ7_3);
    val = mapNoise(val);
    color = CRGB::Purple;
    color.nscale8_video(val);

    for(i = LEVEL1; i < LEVEL2 ; i++) {
      leds[i] = color;
    }

    val = MSGEQ7.get(MSGEQ7_5);
    val = mapNoise(val);

    color = CRGB::Cyan;
    color.nscale8_video(val);

    for(i = LEVEL2; i < LEVEL3 ; i++) {
      leds[i] = color;
    }

    // Update Leds
    FastLED.show();
  }
}
