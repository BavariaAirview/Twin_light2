#include <Servo.h>
#include <FastLED.h>
#include <sbus.h>
#include "value.h"

// #define debug 1

#define NUM_LEDS 28
CRGB leds[NUM_LEDS];
SBUS sbus;

Servo servo6;
Servo servo7;

void setup() {

  servo6.attach(PIN_SERVO6);
  servo7.attach(PIN_SERVO7);

  delay( 500 );
  sbus.begin(SBUS_PIN, sbusNonBlocking);
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

  for (int j; j <= max_lowpass; j++) {
    CH6[j] = 1500;
    CH7[j] = 1500;
  }
}

void loop() {
  if (!sbus.waitFrame()) {               //IF YOU HAVE NO CONNECTION TO THE SBUS ALL LIGHTS WILL BE OFF
    Serial.print("  NO SBUS  ");
  } else {
    SBUSread_filter_Channels();       //I READ THE SBUS CHANNELS 16 TIMES AND FILTER THE SIGNAL BECOUSE SOMETIMES I GOT AN IVALID VALUE
  }

  if (channel_6_result < 900 || channel_6_result > 2100 ) channel_6_result = servo6_center;
  if (channel_7_result < 900 || channel_6_result > 2100 ) channel_7_result = servo7_center;

  CH6[i_f] = channel_6_result ;
  CH7[i_f] = channel_7_result ;
  i_f++;
  if (i_f > max_lowpass) i_f = 1;

  for (int j; j <= max_lowpass; j++) {
    lowpass_filter6 = lowpass_filter6 + CH6[j];
    lowpass_filter7 = lowpass_filter7 + CH7[j];
  }

  channel_6_result = lowpass_filter6 / max_lowpass;
  channel_7_result = lowpass_filter7 / max_lowpass;
  lowpass_filter6 = 0;
  lowpass_filter7 = 0;

#ifdef debug
  Serial.print(channel_6_result);
  Serial.print(" ");
  Serial.print(channel_7_result);
  Serial.print(" ");
  Serial.print(channel_8_result);
  Serial.print(" ");
#endif

  RC_CMD = channel_8_result;

  if (RC_CMD <= 1399) ST = 1;
  else if (RC_CMD >= 1400 && RC_CMD <= 1799) ST = 2;
  else if (RC_CMD >= 1800 && RC_CMD <= 2099) ST = 3;
  if (RC_CMD >= 2100 || RC_CMD < 1200) ST = 4;
  if (RC_CMD < 900) ST = 1;

#ifdef debug
  Serial.print(ST);
  Serial.print(" ");
#endif

  Servo_cmd();

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
  if (cycle_cnt == 3) {
    if ( tail == true) tailLED = true;
  } else {
    tailLED = false;
  }

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

  leds[0] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);        //Beacon1 unter Rumpf
  leds[1] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);
  leds[2] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);
  leds[3] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);        //Beacon1 über Rumpf
  leds[4] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);
  leds[5] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);
  leds[6] = CRGB(LED_GN_Beacon, LED_RT_Beacon, LED_BL_Beacon);

  leds[7] = CRGB(LED_GN_NavLLand, LED_RT_NavL, LED_BL_NavLLand);          // Wing L pos Strobe
  leds[8] = CRGB(LED_GN_NavLLand, LED_RT_NavL, LED_BL_NavLLand);
  leds[9] = CRGB(LED_GN_NavLLand, LED_RT_NavL, LED_BL_NavLLand);
  leds[10] = CRGB(LED_GN_NavL, LED_RT_NavL, LED_BL_NavL);              //Wing L pos
  leds[11] = CRGB(LED_GN_NavL, LED_RT_NavL, LED_BL_NavL);
  leds[12] = CRGB(LED_GN_NavL, LED_RT_NavL, LED_BL_NavL);
  leds[13] = CRGB(LED_GN_Strobe, LED_RT_NavL, LED_BL_Strobe);          //Wing L pos LAND
  leds[14] = CRGB(LED_GN_Strobe, LED_RT_NavL, LED_BL_Strobe);
  leds[15] = CRGB(LED_GN_Strobe, LED_RT_NavL, LED_BL_Strobe);          //Wing L pos LAND
  leds[16] = CRGB(LED_GN_Strobe, LED_RT_NavL, LED_BL_Strobe);

  leds[17] = CRGB(LED_GN_NavR, LED_RT_Strobe, LED_BL_Strobe);          // Wing R pos Strobe
  leds[18] = CRGB(LED_GN_NavR, LED_RT_Strobe, LED_BL_Strobe);
  leds[19] = CRGB(LED_GN_NavR, LED_RT_Strobe, LED_BL_Strobe);
  leds[20] = CRGB(LED_GN_NavR, LED_RT_Strobe, LED_BL_Strobe);              //Wing R pos
  leds[21] = CRGB(LED_GN_NavR, LED_RT_NavR, LED_BL_NavR);
  leds[22] = CRGB(LED_GN_NavR, LED_RT_NavR, LED_BL_NavR);
  leds[23] = CRGB(LED_GN_NavR, LED_RT_NavRLand, LED_BL_NavRLand);          //Wing R pos LAND
  leds[24] = CRGB(LED_GN_NavR, LED_RT_NavRLand, LED_BL_NavRLand);
  leds[25] = CRGB(LED_GN_NavR, LED_RT_NavRLand, LED_BL_NavRLand);
  leds[26] = CRGB(LED_GN_NavR, LED_RT_NavRLand, LED_BL_NavRLand);
  leds[27] = CRGB(LED_GN_NavR, LED_RT_NavRLand, LED_BL_NavRLand);

  FastLED.show();
#ifdef debug
  Serial.println(" ");
#endif
}

//FILTERING THE SBUS CHANNELS OVER 16 READINGS BECOUSE OF SOME INTERRUPTS
void SBUSread_filter_Channels() {
  if (channel_6_count <= 15) {
    channel_6 += sbus.getChannel(SBUSchannel_6);
    channel_6_count++;
  } else {
    channel_6_count = 0;
    channel_6_result = channel_6 / 16;
    channel_6 = 0;
  }
  if (channel_7_count <= 15) {
    channel_7 += sbus.getChannel(SBUSchannel_7);
    channel_7_count++;
  } else {
    channel_7_count = 0;
    channel_7_result = channel_7 / 16;
    channel_7 = 0;
  }
  if (channel_8_count <= 15) {
    channel_8 += sbus.getChannel(SBUSchannel_8);
    channel_8_count++;
  } else {
    channel_8_count = 0;
    channel_8_result = channel_8 / 16;
    channel_8 = 0;
  }
}

void Servo_cmd() {

  if (channel_6_result < 2200 && channel_6_result > 800) {
    servo6.writeMicroseconds(channel_6_result);
  } else {
    servo6.writeMicroseconds(servo6_center);
  }
  if (channel_7_result < 2200 && channel_7_result > 800) {
    servo7.writeMicroseconds(channel_7_result);
  } else {
    servo7.writeMicroseconds(servo7_center);
  }
}
