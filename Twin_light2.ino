#include <SoftwareSerial.h>
#include <FastLED.h>
#include <GCS_MAVLink.h>
#include "value.h"

CRGB leds[NUM_LEDS];
SoftwareSerial mavlinkSerial(12, 11); // RX, TX
#include "mav_link.h"
#include "output.h"
#include "debuging.h"

//______________________________________________________________________________________
void setup() {

  mavlinkSerial.begin(38400);
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
  Serial.print("Init done");
}


//______________________________________________________________________________________
void loop() {

  mavl_receive();
  set_flight_mode_flags();

  light_switch();
  light_cycle();
  light_output();

  if (int_cycle > 200) {
    if (set_home == 0) {
      GPS_calculateDistanceAndDirectionToHome();
     }
     int_cycle = 0;
#ifdef debug
    debug_print();
#endif
  }
  //set GPS home when 3D fix
  if (fix_type > 2 && set_home == 1 && gps_lat != 0 && gps_lon != 0 && numSat > 5) {
    gps_home_lat = gps_lat;
    gps_home_lon = gps_lon;
    gps_home_alt = gps_alt;
    GPS_calc_longitude_scaling(gps_home_lat);
    set_home = 0;
  }
  int_cycle++;
}

//______________________________________________________________________________________
//from here on code from Betaflight github https://github.com/betaflight/betaflight/blob/c8b5edb415c33916c91a7ccc8bd19c7276540cd1/src/main/io/gps.c

float GPS_scaleLonDown = 1.0f;  // this is used to offset the shrinking longitude as we go towards the poles

void GPS_calc_longitude_scaling(int32_t lat)
{
  float rads = (fabsf((float)lat) / 10000000.0f) * 0.0174532925f;
  GPS_scaleLonDown = cos_approx(rads);
}

#define DISTANCE_BETWEEN_TWO_LONGITUDE_POINTS_AT_EQUATOR_IN_HUNDREDS_OF_KILOMETERS 1.113195f
#define TAN_89_99_DEGREES 5729.57795f
// Get distance between two points in cm
// Get bearing from pos1 to pos2, returns an 1deg = 100 precision
void GPS_distance_cm_bearing(int32_t *currentLat1, int32_t *currentLon1, int32_t *destinationLat2, int32_t *destinationLon2, uint32_t *dist, int32_t *bearing)
{
  float dLat = *destinationLat2 - *currentLat1; // difference of latitude in 1/10 000 000 degrees
  float dLon = (float)(*destinationLon2 - *currentLon1) * GPS_scaleLonDown;
  *dist = sqrtf(sq(dLat) + sq(dLon)) * DISTANCE_BETWEEN_TWO_LONGITUDE_POINTS_AT_EQUATOR_IN_HUNDREDS_OF_KILOMETERS;

  *bearing = 9000.0f + atan2f(-dLat, dLon) * TAN_89_99_DEGREES;      // Convert the output radians to 100xdeg
  if (*bearing < 0)
    *bearing += 36000;
}

void GPS_calculateDistanceAndDirectionToHome(void)
{
  if (gps_home_lat != 0 && gps_home_lon != 0) {      // If we don't have home set, do not display anything
    uint32_t dist;
    int32_t dir;
    GPS_distance_cm_bearing(&gps_lat, &gps_lon, &gps_home_lat, &gps_home_lon, &dist, &dir);
    distanceToHome = dist / 100;
    directionToHome = dir / 100;
  } else {
    distanceToHome = 0;
    directionToHome = 0;
  }
}

#define M_PIf       3.14159265358979323846f
#define sinPolyCoef3 -1.666568107e-1f
#define sinPolyCoef5  8.312366210e-3f
#define sinPolyCoef7 -1.849218155e-4f
#define sinPolyCoef9  0

float sin_approx(float x)
{
  int32_t xint = x;
  if (xint < -32 || xint > 32) return 0.0f;                               // Stop here on error input (5 * 360 Deg)
  while (x >  M_PIf) x -= (2.0f * M_PIf);                                 // always wrap input angle to -PI..PI
  while (x < -M_PIf) x += (2.0f * M_PIf);
  if (x >  (0.5f * M_PIf)) x =  (0.5f * M_PIf) - (x - (0.5f * M_PIf));   // We just pick -90..+90 Degree
  else if (x < -(0.5f * M_PIf)) x = -(0.5f * M_PIf) - ((0.5f * M_PIf) + x);
  float x2 = x * x;
  return x + x * x2 * (sinPolyCoef3 + x2 * (sinPolyCoef5 + x2 * (sinPolyCoef7 + x2 * sinPolyCoef9)));
}

float cos_approx(float x)
{
  return sin_approx(x + (0.5f * M_PIf));
}
