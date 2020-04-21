#define beacon_on 600
#define beacon_break 400
#define strobe_on 200
#define strobe_break 150

#define servo6_center 1500
#define servo7_center 1500

#define PIN_Strobe 4
#define PIN_Tail 5
#define PIN_LandingL 6
#define PIN_Beacon 7
#define WS28_PIN 3
#define PIN_SERVO6 11
#define PIN_SERVO7 10
#define SBUS_PIN 12

#define SBUSchannel_6 6   //Gear
#define SBUSchannel_7 7   //Pan
#define SBUSchannel_8 8   //Light

#define max_lowpass 20

int CH6[max_lowpass];
int CH7[max_lowpass];
int lowpass_filter6 = 0;
int lowpass_filter7 = 0;

uint16_t channel_6;
uint16_t channel_7;
uint16_t channel_8;

uint16_t channel_6_result = 0;
uint16_t channel_7_result = 0;
uint16_t channel_8_result = 0;

int channel_6_count = 0;
int channel_7_count = 0;
int channel_8_count = 0;

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

uint8_t i_f =1;

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
