// FastLED
#include <FastLED.h>

#define LED_PINS    2 // DATA_PIN, or DATA_PIN, CLOCK_PIN
#define COLOR_ORDER RGB
#define CHIPSET     WS2811 // WS2811 LPD8806
#define NUM_LEDS    150

#define BRIGHTNESS  160  // reduce power consumption
#define LED_DITHER  255  // try 0 to disable flickering
#define CORRECTION  TypicalLEDStrip

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

CRGB leds[NUM_LEDS]; // Define the array of leds
CRGBPalette16 gPal;

#define LEVEL1 100
#define LEVEL2 150
#define LEVEL3 200

// MSGEQ7

#include <MSGEQ7.h>
#define pinAnalogLeft A0
#define pinAnalogRight A1
#define pinReset 5
#define pinStrobe 4
#define MSGEQ7_INTERVAL ReadsPerSecond(50)
#define MSGEQ7_SMOOTH true

CMSGEQ7<MSGEQ7_SMOOTH, pinReset, pinStrobe, pinAnalogLeft, pinAnalogRight> MSGEQ7;

#define NUM_RELAYS 8
#define RELAY1 6
#define RELAY2 7
#define RELAY3 8
#define RELAY4 9
#define RELAY5 10
#define RELAY6 11
#define RELAY7 12
#define RELAY8 13
#define RELAYONTIME 40

unsigned long RelayOnTime[8];
int relay[] = {6, 7, 8, 9, 10, 11, 12, 13};

void setup()
{
  Serial.begin(115200);
  MSGEQ7.begin();

  FastLED.addLeds<CHIPSET, LED_PINS, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(CORRECTION);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.setDither(LED_DITHER);
  FastLED.show(); // needed to reset leds to zero

  for(int i = 0; i < NUM_RELAYS; i++) {
    pinMode(relay[i], OUTPUT);
    digitalWrite(relay[i], HIGH);
  }
  
}

void loop() {
  // Analyze without delay
  bool newReading = MSGEQ7.read(MSGEQ7_INTERVAL);

  int i;
  uint8_t val;
  CRGB color;

  if (newReading) {

    // ==== LOW ====
    val = MSGEQ7.get(MSGEQ7_0);
    val = mapNoise(val);

//    Serial.print(val);
//    Serial.print(" ");
    if(val > 20) {
      RelayOn(1-1);
      RelayOn(5-1);
    }

    color = CRGB::Blue;
    color.nscale8_video(val);

    for(i = 0; i < LEVEL1 ; i++) {
      leds[i] = color;
    }

    // ==== MID ====

    val = MSGEQ7.get(MSGEQ7_3);
    val = mapNoise(val);

//    Serial.print(val);
//    Serial.print(" ");
    if(val > 20) {
      RelayOn(2-1);
      RelayOn(4-1);
      RelayOn(6-1);
    }

    color = CRGB::Purple;
    color.nscale8_video(val);

    for(i = LEVEL1; i < LEVEL2 ; i++) {
      leds[i] = color;
    }

    // ==== HIGH ====

    val = MSGEQ7.get(MSGEQ7_5);
    val = mapNoise(val);

//    Serial.println(val);
    if(val > 20) {
      RelayOn(3-1);
//      RelayOn(2-1);
    }

    color = CRGB::Cyan;
    color.nscale8_video(val);

    for(i = LEVEL2; i < NUM_LEDS ; i++) {
      leds[i] = color;
    }

    FastLED.show();
    RelayOff();
  }

//  sinelon();
//  EVERY_N_MILLISECONDS( 20 )] { gHue++; }
   
}

void RelayOn(int i) {
  digitalWrite(relay[i], LOW);
  RelayOnTime[i] = millis();
}

void RelayOff() {
  for(int i = 0; i < NUM_RELAYS; i++) {
    if( (millis() - RelayOnTime[i]) > RELAYONTIME ) {
      digitalWrite(relay[i], HIGH);
    }
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
//  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

