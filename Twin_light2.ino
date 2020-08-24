#include <SoftwareSerial.h>
#include <FastLED.h>
#include <GCS_MAVLink.h>
#include "value.h"

CRGB leds[NUM_LEDS];
SoftwareSerial mavlinkSerial(11, 12); // RX, TX

#include "mav_link.h"
#include "output.h"
#include "debuging.h"

//______________________________________________________________________________________
void setup() {

  mavlinkSerial.begin(57600);
  FastLED.addLeds<WS2811, WS28_PIN, RGB>(leds, NUM_LEDS);                 // WS28 Init

#ifdef debug
  Serial.begin(115200);
#endif

  pinMode( PIN_Strobe, OUTPUT);
  pinMode( WS28_PIN, OUTPUT);
  pinMode( PIN_LandingL, OUTPUT);
  pinMode( PIN_Beacon, OUTPUT);
  pinMode( PIN_Tail, OUTPUT);

  for (int l = 0; l < NUM_LEDS + 1; l++) {
    leds[l] = CRGB(255, 255, 255);
  }
  FastLED.show();
  delay(300);

}


//______________________________________________________________________________________
void loop() {

  mavl_receive();
  set_flight_mode_flags();

#ifdef debug
  debug_print();
#endif

  light_switch();
  light_cycle();
  light_output();

}
