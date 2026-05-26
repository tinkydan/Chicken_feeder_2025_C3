//hw_timer_t *watchdogTimer = NULL;
int bootCount = 0;
long looptime = 0;
DS3231 Clock;
bool UPD_NTC = true;
boolean newData = false;
bool FEED_NOW = 0;
const byte numChars = 32;
char receivedChars[numChars];
bool Century = false;
bool h12;
bool PM;
int cur_on;
byte ADay, AHour, AMinute, ASecond, ABits, Ayear;
bool ADy, A12h, Apm;
//byte year, month, date, DoW, hour_cur, minute, second;
int second_cur, minute_cur, hour_cur, date_cur, month_cur, year_cur, temperature;
int allprint = 0;
String Link = "";
String Last_f = "";
String Last_f2 = "";
bool need2send = LOW;
float BAT_LEV = 0;
String HTML = "";
String HTML2 = "";
bool Dispensor = LOW;
int Dispensor_Cur = 0;
bool SLOPE = HIGH;
int PWM_MAX = 300;
float Hopper_Weight_Max = 6;//8; // should be 6
float Hopper_Weight = 0;
int open_ang = 80;

String myStatus = "";

// replace with your channel's thingspeak API key,
const char * apiKey = "3UL0GOPO09DLP047";
unsigned long myChannelNumber = 731828;


const char* ssid = "TheWhitneys";
const char* password = "icantremember";
byte mac[6];
const char* serverTS = "api.thingspeak.com";
const char* host = "api.thingspeak.com";

unsigned long RTC_UPDATE_mS = 30000;
unsigned long RTC_LAST = 0;

unsigned long Con_UPDATE_mS = 3600000;
unsigned long Con_LAST = 0;

unsigned long S_UPDATE_mS = 10000;
unsigned long S_LAST = 0;

unsigned long DISP_UPDATE_mS = 1;
unsigned long DISP_LAST = 0;

unsigned long TIME_UPDATE_mS = 1000;
unsigned long TIME_LAST = 0;
unsigned long currentMillis = 0;

unsigned long EEPROM_Upload_ms = 2000;  //Min time between EEPROM Updates
unsigned long EEPROM_Upload_LAST = 0;

unsigned long ThgS_UPDATE_mS = 100000; // Retry thingspeak 
unsigned long ThgS_LAST = 0;

//unsigned long HopperWheel_empty_mS = 60000; // Retry thingspeak replaced with     EEPROM.get(88, hop_pps);
unsigned long HopperWheel_LAST = 0;

unsigned long FEED_MILLIS = 0;

unsigned long pause_feed=0;

int dumping=0;
int mid_empty=0;
int door_status=0;
int hop_pps=35;

bool first = HIGH;
bool firstSU = HIGH;
bool firstRTC = HIGH;
bool opened = LOW;
bool Feeding = HIGH;
bool first_loop = HIGH;
float day_feed = 0;
double weight = 0;
bool full = LOW;
bool full_ish = LOW;
bool time_aval = LOW;
int sec = 0;
int mins = 0;
static const int servo_pin = 300;
int adj_pin=3;
double weight2dump=0;

int days = 0;
int LOADCELL_DOUT_PIN = 20; //20;
int LOADCELL_SCK_PIN = 21;//21;

// First desing used un covered NC pins on esp 32c3 resolting in bas scalke signals
const int BAT_PIN = 2;
const int SOLAR_PIN = 2;
int hour_fed = 0;

int Hoper_motor = 0;
int Dis_motor = 10;
unsigned long door_off_time=0;
int door_timeout=35*1000;  
int PWMDO=255;
int PWMDC=255;
int IN1=4;
int IN2=5;

double Weight_baseline = 0;



//LiquidCrystal_I2C lcd(0x27,20,4);
#define RTCMEMORYSTART 65
#define DEBUG_ESP_PORT Serial
Servo myservo;
#define uS_TO_S_FACTOR 1000000





tmElements_t tm;


tmElements_t tm_st;
// OLED Screen
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const long utcOffsetInSeconds = -18000;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds, 10000000);

//strDateTime dateTime;

// day_feed = num_chicks * (0.004095238095 * days + 0.04857142857); Freedom Raingers low end
// day_feed = num_chicks * (0.005395238095 * days + 0.04857142857); Freedom Raingers low end
//   day_feed = num_chicks * (0.00755952381 * days + 0.04130555556);
//NTPtime NTPch("ch.pool.ntp.org");
int pos;
int day_st = 21;
int month_st = 8;
int year_st = 2021;
int num_chicks = 64;
int HOUR_F_1 = 6;
int HOUR_F_2 = 15;
float sec_pp = 30;
float max_feed = 0.505;
double Slope = 0.00755952381;
double Intercept = 0.04130555556;
double scale_calibration = -0.0000101342658 * 0.090928253 * 10 * .52 / .56;

bool do_not_zero = 0;
//determine date and time

long scale_read = 0;
long zero = 0;
long reading = 0;
int iter = 0;

#define EEPROM_SIZE 128
HX711 scale;

//extern "C" {
//#include "user_interface.h"
//}

int boot_count;
int Feed_Hour;



WiFiClient client;

WebServer server(80);



