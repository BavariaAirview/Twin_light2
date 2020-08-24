#define debug 1

#define beacon_on 600
#define beacon_break 400
#define strobe_on 200
#define strobe_break 150

#define PIN_Strobe 4
#define PIN_Tail 5
#define PIN_LandingL 6
#define PIN_Beacon 7
#define WS28_PIN 3

#define NUM_LEDS 28

#define max_lowpass 20

//_______________________________________________________________________________
uint8_t int_cycle =0;

uint8_t vbat = 0;
uint8_t fix_type = 0;           // < 0-1: no fix, 2: 2D fix, 3: 3D fix

uint8_t base_mode = MAV_MODE_PREFLIGHT;
uint8_t system_status = MAV_STATE_UNINIT;

bool arm = false;
bool failsafe = false;

#ifdef debug

uint8_t debugtimeout = 250;
unsigned long debugtime = 0;
bool debugsend = false;

#endif

uint8_t set_home = 1;
uint8_t numSat = 0;
int32_t gps_lon = 0;
int32_t gps_lat = 0;
int32_t gps_alt = 0;
int32_t gps_home_lon = 0;
int32_t gps_home_lat = 0;
int32_t gps_home_alt = 0;
uint32_t distanceToHome = 0;    // distance to home in meters
int32_t relative_alt = 0;  
int16_t directionToHome = 0;   // direction to home in degrees

int i = 0;
unsigned long RC_CMD = 0;
char ST = 0;
char cycle_cnt = 0;

bool IN = false;
bool Sig = false;

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

uint8_t i_f = 1;

unsigned long currenttime;
unsigned long cycletime;
unsigned long an;
unsigned long aus;

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
