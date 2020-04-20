#include "FUTABA_SBUS.h"
#include <FastLED.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define DEBUG
//_______________________________________ SBUS Channel defeintion
#define CH_Licht 13
#define CH_GEAR 10
#define CH_FLAPS 9
#define CH_DIFFTH 11
#define CH_PAN 12
#define CH_Throttle 0
#define CH_Aileron 1
#define CH_Elevator 2
#define CH_Rudder 3
#define CH_AUX1 4
#define CH_AUX2 5
#define CH_AUX3 6
#define rssi 7
#define linkQ 8
#define notUsed1 14
#define notUsed2 15
//____________________________________________________________
#define CHANNELS 16
FUTABA_SBUS sBus;

Servo gear;
Servo flaps;
Servo pan;
Servo diff_throttle;
//_____________________________________Light Times
#define beacon_on 600
#define beacon_break 400
#define strobe_on 250
#define strobe_break 150
//____________________________________ LEDS
#define NUM_LEDS 24
CRGB leds[NUM_LEDS];
//                     LED in Zeile 347

//_______________________________________________________PINOUT
#define PIN_GEAR 8
#define PIN_FLAPS 9
#define PIN_PAN 10
#define PIN_DIFF_THR 11

#define PIN_Strobe 4
#define PIN_Tail 5
#define PIN_LandingL 6
#define PIN_Beacon 7
#define WS28_PIN 3
//______________________________________________________Variablen
int SBUS_CMD[CHANNELS];
int ST = 0;
char cycle_cnt = 0;
int LED_RT_Beacon = 0;
int LED_GN_Beacon = 0;
int LED_BL_Beacon = 0;
int LED_RT_Strobe = 0;
int LED_GN_Strobe = 0;
int LED_BL_Strobe = 0;
int LED_RT_NavR = 0;
int LED_GN_NavR = 0;
int LED_BL_NavR = 0;
int LED_RT_NavL = 0;
int LED_GN_NavL = 0;
int LED_BL_NavL = 0;
int LED_RT_NavRLand = 0;
int LED_GN_NavRLand = 0;
int LED_BL_NavRLand = 0;
int LED_RT_NavLLand = 0;
int LED_GN_NavLLand = 0;
int LED_BL_NavLLand = 0;

unsigned long currenttime;
unsigned long cycletime;
bool NAVlights;
bool Beacon;
bool Strobe;
bool LandingL;
bool cycle;
bool tail;
bool static_light;
bool tailLED;
bool BeaconLED;
bool StrobeLED;

SoftwareSerial mySerial(12, 13); // RX, TX
//____________________________________________________________________SETUP
void setup() {
  delay( 1000 );

  FastLED.addLeds<WS2811, WS28_PIN, RGB>(leds, NUM_LEDS);                 // WS28 Init

  pinMode(PIN_Strobe, OUTPUT);
  pinMode(WS28_PIN, OUTPUT);
  pinMode(PIN_LandingL, OUTPUT);
  pinMode(PIN_Beacon, OUTPUT);
  pinMode(PIN_Tail, OUTPUT);

  for (int l = 0; l < NUM_LEDS + 1; l++) {
    leds[l] = CRGB(255, 255, 255);
  }
  FastLED.show();
  delay(300);

  mySerial.begin(9600);
  sBus.begin();

  gear.attach(PIN_GEAR, 1000, 2000);
  flaps.attach(PIN_FLAPS, 1000, 2000);
  pan.attach(PIN_PAN, 1000, 2000);
  diff_throttle.attach(PIN_DIFF_THR, 1000, 2000);
}
//____________________________________________________________________LOOP
void loop() {

  //____________________________________________________________________SBUS lesen
  sBus.FeedLine();
  if (sBus.toChannels == 1) {
    sBus.UpdateServos();
    sBus.UpdateChannels();
    sBus.toChannels = 0;

    for (int i = 0; i < CHANNELS; i++) {
      SBUS_CMD[i] = map(sBus.channels[i], 0, 2016, 860, 2140);
    }
  }
  if (SBUS_CMD[0] == 0 && SBUS_CMD[1] == 0 && SBUS_CMD[2] == 0) {
    ST = 1;
    SBUS_CMD[CH_FLAPS] = 1500;
    SBUS_CMD[CH_GEAR] = 1500;
    SBUS_CMD[CH_DIFFTH] = 1000;
    SBUS_CMD[CH_PAN] = 1500;
  }

  if (SBUS_CMD[CH_Licht] <= 1249) ST = 1;
  else if (SBUS_CMD[CH_Licht] >= 1250 && SBUS_CMD[CH_Licht] <= 1699) ST = 2;
  else if (SBUS_CMD[CH_Licht] >= 1700 && SBUS_CMD[CH_Licht] <= 1999) ST = 3;
  if (SBUS_CMD[CH_Licht] >= 2000 || SBUS_CMD[CH_Licht] < 1000) ST = 4;
  if (SBUS_CMD[CH_Throttle] < 1050) ST = 1;

  if (sBus.failsafe_status == SBUS_SIGNAL_FAILSAFE) {
    ST = 1;
    SBUS_CMD[CH_FLAPS] = 1500;
    SBUS_CMD[CH_GEAR] = 1500;
    SBUS_CMD[CH_DIFFTH] = 1000;
    SBUS_CMD[CH_PAN] = 1500;
  }

  //____________________________________________________________________ DEBUG OUTPUT
#ifdef DEBUG
  for (int i = 0; i < CHANNELS; i++) {
    mySerial.print("inCh "); mySerial.print(i); mySerial.print(": ");
    mySerial.print(sBus.channels[i]);
    mySerial.print(", ");
  }
  mySerial.println();
  for (int i = 0; i < CHANNELS; i++) {
    mySerial.print("RxCh "); mySerial.print(i); mySerial.print(": ");
    mySerial.print(SBUS_CMD[i]);
    mySerial.print(", ");
  }
  mySerial.print(ST);
  mySerial.print(", ");
  mySerial.println();
#endif
  //____________________________________________________________________ AUSWERTUNG
  switch (ST) {
    case 4: {
        NAVlights = true;
        Beacon = false;
        Strobe = false;
        tail = false;
        LandingL = false;
        cycle = false;
        cycletime = 0;
        static_light = false;
        cycle_cnt = 0;

        digitalWrite(PIN_LandingL, LOW);
        digitalWrite(PIN_Beacon, LOW);
        digitalWrite(PIN_Tail, LOW);
        digitalWrite(PIN_Strobe, LOW);
        break;
      }
    case 2: {
        NAVlights = true;
        Beacon = true;
        Strobe = true;
        tail = true;
        LandingL = false;
        cycle = true;
        static_light = false;
        break;
      }
    case 3: {
        NAVlights = true;
        Beacon = true;
        Strobe = true;
        tail = true;
        LandingL = true;
        cycle = true;
        static_light = false;
        break;
      }
    case 1: {
        NAVlights = false;
        Beacon = false;
        Strobe = true;
        tail = false;
        LandingL = false;
        cycle = true;
        static_light = true;
        break;
      }
    default: {
        NAVlights = false;
        Beacon = false;
        Strobe = false;
        tail = false;
        LandingL = false;
        cycle = false;
        static_light = false;

        digitalWrite(PIN_LandingL, LOW);
        digitalWrite(PIN_Beacon, LOW);
        digitalWrite(PIN_Tail, LOW);
        digitalWrite(PIN_Strobe, LOW);
        break;
      }
  }

  currenttime = millis();

  if (cycletime == 0 && cycle == true) {
    cycletime = millis();
    cycle_cnt = 1;
  }

  if (cycle == true) {
    unsigned long st_time = currenttime - cycletime;
    if (cycle_cnt == 1 && st_time >= beacon_on) {
      cycle_cnt = 2;
    }
    if (cycle_cnt == 2 && st_time >= beacon_on + beacon_break) {
      cycle_cnt = 3;
    }
    if (cycle_cnt == 3 && st_time >= beacon_on + beacon_break + strobe_on) {
      cycle_cnt = 4;
    }
    if (cycle_cnt == 4 && st_time >= beacon_on + beacon_break + strobe_on + strobe_break) {
      cycle_cnt = 5;
    }
    if (cycle_cnt == 5 && st_time >= beacon_on + beacon_break + 2 * strobe_on + strobe_break) {
      cycle_cnt = 6;
    }
    if (cycle_cnt == 6 && st_time >= beacon_on + 2 * beacon_break + 2 * strobe_on + strobe_break) {
      cycle_cnt = 0;
      cycletime = 0;
      cycle = 0;
    }
  }

  if (cycle_cnt == 0 || cycle_cnt == 2 || cycle_cnt == 4 || cycle_cnt == 6) {
    BeaconLED = false;
    StrobeLED = false;
  }
  if (cycle_cnt == 1) {
    if (Beacon == true)BeaconLED = true;
    StrobeLED = false;
  }
  if (cycle_cnt == 3 || cycle_cnt == 5) {
    BeaconLED = false;
    if (Strobe == true)StrobeLED = true;
  }
  if (cycle_cnt == 5) {
    if ( tail == true) tailLED = true;
  } else {
    tailLED = false;
  }
  //____________________________________________________________________LED AUSGABE
  if (BeaconLED == true) {
    LED_RT_Beacon = 255;
    LED_GN_Beacon = 0;
    LED_BL_Beacon = 0;
    digitalWrite(PIN_Beacon, HIGH);
  } else {
    LED_RT_Beacon = 0;
    LED_GN_Beacon = 0;
    LED_BL_Beacon = 0;
    digitalWrite(PIN_Beacon, LOW);
  }

  if (NAVlights == true) {
    LED_RT_NavR = 0;
    LED_GN_NavR = 255;
    LED_BL_NavR = 0;

    LED_RT_NavL = 255;
    LED_GN_NavL = 0;
    LED_BL_NavL = 0;
  } else {
    LED_RT_NavR = 0;
    LED_GN_NavR = 0;
    LED_BL_NavR = 0;

    LED_RT_NavL = 0;
    LED_GN_NavL = 0;
    LED_BL_NavL = 0;
  }

  if (LandingL == true) {
    digitalWrite(PIN_LandingL, HIGH);
    LED_RT_NavRLand = 255;
    LED_GN_NavRLand = 255;
    LED_BL_NavRLand = 255;
    LED_RT_NavLLand = 255;
    LED_GN_NavLLand = 255;
    LED_BL_NavLLand = 255;
  } else {
    digitalWrite(PIN_LandingL, LOW);
    LED_RT_NavRLand = 0;
    LED_GN_NavRLand = 0;
    LED_BL_NavRLand = 0;
    LED_RT_NavLLand = 0;
    LED_GN_NavLLand = 0;
    LED_BL_NavLLand = 0;
  }
  if (tailLED == true) {
    digitalWrite(PIN_Tail, HIGH);
  } else {
    digitalWrite(PIN_Tail, LOW);
  }
  if (Strobe == true) {
    if (StrobeLED == true) {
      digitalWrite(PIN_Strobe, HIGH);
    } else {
      digitalWrite(PIN_Strobe, LOW);
    }
  }
  if (StrobeLED == true) {
    LED_RT_Strobe = 255;
    LED_GN_Strobe = 255;
    LED_BL_Strobe = 255;
  } else {
    LED_RT_Strobe = 0;
    LED_GN_Strobe = 0;
    LED_BL_Strobe = 0;
  }
  if (static_light == true) {
    LED_BL_Strobe = 255;
    LED_BL_NavR = 255;
    LED_BL_NavL = 255;
    LED_BL_Beacon = 255;
    LED_BL_NavRLand = 255;
    LED_BL_NavLLand = 255;

    LED_RT_Beacon = 0;
    LED_GN_Beacon = 0;
    LED_RT_NavR = 0;
    LED_GN_NavR = 0;
    LED_RT_NavL = 0;
    LED_GN_NavL = 0;
    LED_RT_Strobe = 0;
    LED_GN_Strobe = 0;
    LED_RT_NavRLand = 0;
    LED_GN_NavRLand = 0;
    LED_RT_NavLLand = 0;
    LED_GN_NavLLand = 0;
  }
  //____________________________________________________________________WS28  AUSGABE   !!!!  GRB   !!!!

  leds[0] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);        //Beacon1 unter Rumpf
  leds[1] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);
  leds[2] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);
  leds[3] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);
  leds[4] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);        //Beacon1 über Rumpf
  leds[5] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);
  leds[6] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);
  leds[7] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);

  leds[8] = CRGB(LED_GN_NavR, LED_RT_Strobe, LED_BL_Strobe);          // Wing R pos Strobe
  leds[9] = CRGB(LED_GN_NavR, LED_RT_Strobe, LED_BL_Strobe);
  leds[10] = CRGB(LED_GN_NavR, LED_RT_Strobe, LED_BL_Strobe);
  leds[11] = CRGB(LED_GN_NavR, LED_RT_NavR, LED_BL_NavR);              //Wing R pos
  leds[12] = CRGB(LED_GN_NavR, LED_RT_NavR, LED_BL_NavR);
  leds[13] = CRGB(LED_GN_NavR, LED_RT_NavR, LED_BL_NavR);
  leds[14] = CRGB(LED_GN_NavR, LED_RT_NavLLand, LED_BL_NavRLand);          //Wing R pos LAND
  leds[15] = CRGB(LED_GN_NavR, LED_RT_NavLLand, LED_BL_NavRLand);

  leds[16] = CRGB(LED_GN_Strobe, LED_RT_NavL, LED_BL_Strobe);          // Wing L pos Strobe
  leds[17] = CRGB(LED_GN_Strobe, LED_RT_NavL, LED_BL_Strobe);
  leds[18] = CRGB(LED_GN_Strobe, LED_RT_NavL, LED_BL_Strobe);
  leds[19] = CRGB(LED_GN_NavL, LED_RT_NavL, LED_BL_NavL);              //Wing L pos
  leds[20] = CRGB(LED_GN_NavL, LED_RT_NavL, LED_BL_NavL);
  leds[21] = CRGB(LED_GN_NavL, LED_RT_NavL, LED_BL_NavL);
  leds[22] = CRGB(LED_GN_NavL, LED_RT_NavLLand, LED_BL_NavLLand);          //Wing L pos LAND
  leds[23] = CRGB(LED_GN_NavL, LED_RT_NavLLand, LED_BL_NavLLand);

  FastLED.show();

  //____________________________________________________________________SERVO AUSGABE

  flaps.writeMicroseconds(SBUS_CMD[CH_FLAPS]);
  gear.writeMicroseconds(SBUS_CMD[CH_GEAR]);
  diff_throttle.writeMicroseconds(SBUS_CMD[CH_DIFFTH]);
  pan.writeMicroseconds(SBUS_CMD[CH_PAN]);

}
