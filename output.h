
//______________________________________________________________________________________
void light_switch() {

  if (fix_type >= 3) {

    if (arm) {
      ST = 3;
    } else {
      ST = 2;
    }
    if (failsafe) ST = 4;
  }
  else if (fix_type == 1 || fix_type == 2) {
    ST = 1;
  }
  else {
    ST = 0;
  }

  if (distanceToHome > 50) {
    LandingL = true;
  } else {
    LandingL = false;
  }

  
  switch (ST) {
    case 1: {                             // not armed / GPS not ready
        NAVlights = false;
        Beacon = false;
        Strobe = true;
        tail = false;
        cycle = true;
        static_light = true;
        break;
      }
    case 2: {                             // not armed / GPS ready
        NAVlights = true;
        Beacon = true;
        Strobe = false;
        tail = false;
        cycle = true;
        static_light = false;
        break;
      }
    case 3: {                             // armed
        NAVlights = true;
        Beacon = true;
        Strobe = true;
        tail = true;
        cycle = true;
        static_light = false;
        break;
      }
    case 4: {                             // Failsafe
        NAVlights = false;
        Beacon = false;
        Strobe = true;
        tail = true;
        cycle = true;
        static_light = true;
        break;
      }

    default: {                          // ALL OFF
        NAVlights = false;
        Beacon = false;
        Strobe = false;
        tail = false;
        cycle = false;
        static_light = false;

        digitalWrite(PIN_LandingL, LOW);
        digitalWrite(PIN_Beacon, LOW);
        digitalWrite(PIN_Tail, LOW);
        digitalWrite(PIN_Strobe, LOW);
        break;
      }
  }
}


//______________________________________________________________________________________
void light_cycle() {

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
}


//______________________________________________________________________________________
void light_output() {
  if (cycle_cnt == 0 || cycle_cnt == 2 || cycle_cnt == 4 || cycle_cnt == 6) {
    BeaconLED = false;
    StrobeLED = false;
  }
  if (cycle_cnt == 1) {
    if (Beacon == true) {
      BeaconLED = true;
    }
    StrobeLED = false;
  }
  if (cycle_cnt == 3 || cycle_cnt == 5) {
    BeaconLED = false;
    if (Strobe == true) {
      StrobeLED = true;
    }
  }
  if (cycle_cnt == 3) {
    if ( tail == true) {
      tailLED = true;
    }
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
}
