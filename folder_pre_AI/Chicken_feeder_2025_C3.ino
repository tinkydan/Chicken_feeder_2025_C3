#include "HX711.h"
#include <TimeLib.h>
#include "NTPClient.h"
#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#ifdef ESP8266
#include <Servo.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ESP32)
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#endif

//
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
//LiquidCrystal_I2C lcd(0x3F, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

const unsigned char COOP[] PROGMEM = {
  // 'PCB_pcb copy_2021-09-24 (2), 91x64px
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x0f, 0x00, 0x00, 0x00,
  0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x20, 0x00, 0x7f, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xfe, 0x50, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0xfc, 0x40, 0x22, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x0f, 0xf8, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x1f, 0xfe, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,
  0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x88, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xc4, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0xff, 0xff, 0xe2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff,
  0xff, 0xe2, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xf1, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff,
  0xff, 0xfe, 0x10, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0xdc, 0xff, 0xff, 0xff, 0x8c, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x03, 0x38, 0xff, 0xff, 0xff, 0xce, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x43, 0xff, 0xff, 0xff, 0xe1, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe,
  0x00, 0x0f, 0xf0, 0xc0, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x80, 0x07, 0xf8, 0x41,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0xe4, 0x1f, 0xf8, 0x1f, 0xf0, 0x40, 0x00, 0x00,
  0x00, 0x00, 0x0f, 0xfe, 0x24, 0x1f, 0xf8, 0x00, 0x30, 0x40, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfc,
  0x00, 0x07, 0xf8, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x07, 0xf8, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x9f, 0xfc, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x40, 0x00, 0x00,
  0x00, 0x00, 0x9f, 0xfe, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x9f, 0xfe,
  0x00, 0x07, 0xf8, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0xbf, 0xfe, 0x00, 0x07, 0xf8, 0x00,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0xbf, 0xfe, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x40, 0x00, 0x00,
  0x00, 0x00, 0x9f, 0xfe, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0xfe,
  0x00, 0x07, 0xf8, 0x01, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0xfe, 0x00, 0x07, 0xf8, 0x81,
  0xc0, 0x00, 0xb0, 0x00, 0x00, 0x00, 0x9f, 0xfe, 0x00, 0x07, 0xf8, 0x01, 0xc0, 0x01, 0x20, 0x00,
  0x00, 0x00, 0xff, 0xfe, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfe,
  0x00, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x07, 0xfb, 0x07,
  0xdc, 0x10, 0x08, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x07, 0xfb, 0x7f, 0xff, 0x1c, 0x08, 0x00,
  0x00, 0x00, 0xff, 0xfe, 0x00, 0x07, 0xfa, 0x7d, 0xff, 0x9c, 0x1c, 0x00, 0x00, 0x00, 0xff, 0xfe,
  0x00, 0x07, 0xf8, 0x00, 0x4b, 0x1c, 0x38, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x0f, 0xf8, 0x00,
  0x08, 0x10, 0x10, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x0f, 0xff, 0xf8, 0x00, 0x00, 0x10, 0x10, 0x00,
  0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xfd, 0xc0, 0x80, 0x10, 0x10, 0x00, 0x00, 0x00, 0x1f, 0xff,
  0xff, 0xf7, 0xdd, 0xff, 0xc1, 0x93, 0x30, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xb0, 0x61, 0xf8, 0xf8,
  0x00, 0x13, 0xfc, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x40, 0xf8, 0x78, 0x00, 0x00, 0x70, 0x00,
  0x00, 0x00, 0x06, 0x00, 0x04, 0x40, 0xf8, 0x78, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x06, 0x00,
  0x04, 0x60, 0xf8, 0x00, 0x00, 0x10, 0xfc, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x60, 0xf8, 0x00,
  0x00, 0x38, 0x60, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x78, 0x30, 0x00, 0x00, 0x3f, 0xc0, 0x00,
  0x00, 0x80, 0x07, 0x00, 0x00, 0x78, 0x30, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x60, 0xb0, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0xb0, 0x00,
  0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


//LiquidCrystal_I2C lcd(0x27,20,4);
#define RTCMEMORYSTART 65
#define DEBUG_ESP_PORT Serial
Servo myservo;
#define uS_TO_S_FACTOR 1000000
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
int Hopper_Weight_Max = 12;
int Hopper_Weight = 0;
int open_ang = 150;
#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTln(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTln(x)
#endif





#define DEBUG_WAIT


#ifdef DEBUG_WAIT
#define DEBUG_PRINT_WAIT(x) \
  Serial.println(x); \
  delay(250);
#else
#define DEBUG_PRINT_WAIT(x)
#endif


// replace with your channel's thingspeak API key,
String apiKey = "3UL0GOPO09DLP047";
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

unsigned long FEED_MILLIS = 0;

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
static const int servo_pin = 3;


int days = 0;
int LOADCELL_DOUT_PIN = 20;
int LOADCELL_SCK_PIN = 21;
const int BAT_PIN = 2;
const int SOLAR_PIN = 2;
int hour_fed = 0;

int Hoper_motor = 0;
int Dis_motor = 10;


double Weight_baseline = 0;


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

void setup() {
  pinMode(Hoper_motor, OUTPUT);
  digitalWrite(Hoper_motor, LOW);
  pinMode(Dis_motor, OUTPUT);
  digitalWrite(Dis_motor, LOW);


  Wire.begin(6, 7);
  Serial.begin(115200);
  DEBUG_PRINTln("Loading Scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  delay(500);
  Serial.println(scale.read_average(20) * scale_calibration);
  //DEBUG_PRINT("Reading ");
  //readFromRTCMemory();
  DEBUG_PRINT("Writing ");
  //writeToRTCMemory();
  //esp_task_wdt_init(120, true);
  //esp_task_wdt_add(NULL);
  while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64

    DEBUG_PRINTln(F("SSD1306 allocation failed"));
    delay(500);  // Don't proceed, loop forever
  }

  display.clearDisplay();  //for Clearing the display
  display.setCursor(0, 0);
  display.println("Auto");
  display.setCursor(0, 7);
  display.println("Feeder");
  display.setCursor(0, 15);
  display.println("V1.1");

  display.drawBitmap(35, 0, COOP, 91, 64, WHITE);  // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)

  display.display();

  delay(2500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Wifi Connect WiFi:");
  display.println(ssid);
  // put your setup code here, to run once:
  display.display();
  ////Serial.begin(115200);

  DEBUG_PRINTln("Starting EEPROM");
  EEPROM.begin(EEPROM_SIZE);

  EEPROM.get(0, day_st);
  if (day_st == -1) {
    DEBUG_PRINTln("Setting EEPROM");
    EEPROM.put(0, day_st);
    EEPROM.put(4, month_st);
    EEPROM.put(8, year_st);
    EEPROM.put(12, num_chicks);
    EEPROM.put(16, HOUR_F_1);
    EEPROM.put(20, HOUR_F_2);
    EEPROM.put(24, sec_pp);
    EEPROM.put(28, max_feed);
    EEPROM.put(32, Slope);
    EEPROM.put(40, Intercept);
    EEPROM.put(48, scale_calibration);
    EEPROM.put(56, PWM_MAX);
 EEPROM.put(64, hour_fed);
  EEPROM.put(72, Weight_baseline);

    EEPROM.commit();
  }
  //| LATT | LONG | TIMZ | ARIS | ASET | UpLm | DnLm
  DEBUG_PRINTln("Reading EEPROM");

  EEPROM.get(0, day_st);
  EEPROM.get(4, month_st);
  EEPROM.get(8, year_st);
  EEPROM.get(12, num_chicks);
  EEPROM.get(16, HOUR_F_1);
  EEPROM.get(20, HOUR_F_2);
  EEPROM.get(24, sec_pp);
  EEPROM.get(28, max_feed);
  EEPROM.get(32, Slope);
  EEPROM.get(40, Intercept);
  EEPROM.get(48, scale_calibration);
  EEPROM.get(56, PWM_MAX);
  EEPROM.get(64, hour_fed);
  EEPROM.get(72, Weight_baseline);


  DEBUG_PRINT("READING EEPROM || Day Start: ");
  DEBUG_PRINTln(day_st);
  DEBUG_PRINT("READING  EEPROM || Month Start: ");
  DEBUG_PRINTln(month_st);
  DEBUG_PRINT("READING  EEPROM || Year Start: ");
  DEBUG_PRINTln(year_st);
  DEBUG_PRINT("READING  EEPROM || Number of Chicks: ");
  DEBUG_PRINTln(num_chicks);
  DEBUG_PRINT("READING  EEPROM || Hour Feed 1: ");
  DEBUG_PRINTln(HOUR_F_1);
  DEBUG_PRINT("READING  EEPROM || Hour Feed 2: ");
  DEBUG_PRINTln(HOUR_F_2);
  DEBUG_PRINT("READING  EEPROM || Seconds per Pound of Feed: ");
  DEBUG_PRINTln(sec_pp);
  Serial.println("Testing");
  Serial.println(Slope);
  DEBUG_PRINT("READING  EEPROM || Max Days feed per Chicken: ");
  DEBUG_PRINTln(max_feed);
  DEBUG_PRINT("READING  EEPROM || Feed Slope: ");
  DEBUG_PRINTln(Slope);
  DEBUG_PRINT("READING  EEPROM || Feed Intercept: ");
  DEBUG_PRINTln(Intercept);
  DEBUG_PRINT("READING EEPROM || Scale Calibration: ");
  DEBUG_PRINTln(scale_calibration);
  DEBUG_PRINT("READING EEPROM || Weight_baseline: ");
  DEBUG_PRINTln(Weight_baseline);










  //  display.setCursor(0,8);
  //  display.print("by EasyIoT");

  DEBUG_PRINTln("Initializing Time");

  tm_st.Hour = 0;
  tm_st.Minute = 0;
  tm_st.Second = 0;
  tm_st.Day = day_st;
  tm_st.Month = month_st;
  tm_st.Year = year_st - 2000;  // because Year is offset fro

  DEBUG_PRINTln("Pin Setup");

  pinMode(Hoper_motor, OUTPUT);
  digitalWrite(Hoper_motor, LOW);
  pinMode(Dis_motor, OUTPUT);
  digitalWrite(Dis_motor, LOW);

  //pinMode(16, INPUT);


  DEBUG_PRINTln("Screen Setup");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Wifi Connect WiFi:");
  display.println(ssid);
  // put your setup code here, to run once:
  display.display();

  DEBUG_PRINTln("Screen Setup");
  delay(1000);
  //pinMode(0, INPUT);
  //pinMode(0, INPUT_PULLUP);
  // WiFi.mode(WIFI_AP_STA);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  DEBUG_PRINTln("Connecting");


  //   display.begin(20, 4);                      // initialize the lcd
  // Print a message to the  display.
  //  display.backlight();



  DEBUG_PRINTln();
  while ((WiFi.status() != WL_CONNECTED) && (iter < 20)) {
    delay(500);


    DEBUG_PRINT(".");
    iter = iter + 1;
  }


  if (WiFi.status() != WL_CONNECTED) {



  } else {
    DEBUG_PRINTln("WiFi connected");
    DEBUG_PRINTln("IP address: ");
    DEBUG_PRINTln(WiFi.localIP());
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  }


  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //if (hour_fed == 0) {
  //  hour_fed = Clock.getHour(h12, PM);
  //}
  //myservo.attach(26);
  reading = scale.read();
  scale_read = reading;
  DEBUG_PRINTln("Initial Scale Weight Reading " + String(scale_read * scale_calibration));

  if (Weight_baseline <= 0 || ((scale_read * scale_calibration) < (Weight_baseline + 0.75))) {

    myservo.attach(servo_pin);
    //
    display.clearDisplay();

    display.setCursor(0, 0);
    display.println("Testing Servo");
    // put your setup code here, to run once:
    display.display();
    digitalWrite(Dis_motor, HIGH);
    DEBUG_PRINTln("detach test");

    for (pos = 0; pos <= open_ang / 2; pos += 1) {  // goes from 180 degrees to 0 degrees
      myservo.write(pos);                           // tell servo to go to position in variable 'pos'
      delay(5);                                     // waits 15ms for the servo to reach the position
    }



    display.println("Set halfway");
    display.display();

    delay(2000);
    for (pos = open_ang / 2; pos <= open_ang; pos += 1) {  // goes from 180 degrees to 0 degrees
      myservo.write(pos);                                  // tell servo to go to position in variable 'pos'
      delay(5);                                            // waits 15ms for the servo to reach the position
    }

    display.println("Set Open");
    display.display();
    delay(2000);
    for (pos = open_ang; pos >= 0; pos -= 1) {  // goes from 180 degrees to 0 degrees
      myservo.write(pos);                       // tell servo to go to position in variable 'pos'
      delay(5);                                 // waits 15ms for the servo to reach the position
    }
    display.println("Set Closed");
    display.display();
    digitalWrite(Dis_motor, LOW);

    if (Weight_baseline > 0) {

      reading = scale.read();
      scale_read = reading;

      DEBUG_PRINTln("Weight Baseline (Previously " + String(Weight_baseline) + "lb");
      Weight_baseline = scale_read * scale_calibration * 1 / 10 + Weight_baseline * 9 / 10;
      EEPROM.put(72, Weight_baseline);
      EEPROM.commit();
      DEBUG_PRINTln("Updating the baseline zero point to " + String(Weight_baseline) + "lb");
    }

  } else if ((scale_read * scale_calibration) > (Weight_baseline + 0.75)) {
    do_not_zero = 1;
  }
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("AutoFeed");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    DEBUG_PRINTln("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    DEBUG_PRINTln("\nEnd");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Rebooting...");
    display.display();
    ESP.restart();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //   DEBUG_PRINTf("Progress: %u%%\r", (progress / (total / 100)));

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Updating...");

    drawProgressbar(5, 20, 120, 10, (progress * 100 / total));
    display.display();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    // DEBUG_PRINTf("Error[%u]: ", /////error);
    if (error == OTA_AUTH_ERROR) {
      DEBUG_PRINTln("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      DEBUG_PRINTln("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      DEBUG_PRINTln("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      DEBUG_PRINTln("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      DEBUG_PRINTln("End Failed");
    }
  });
  ArduinoOTA.begin();
  DEBUG_PRINTln("Ready");
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTln(WiFi.localIP());

  DEBUG_PRINTln("detach servo");
  myservo.detach();



  DEBUG_PRINTln("seting up server");
  server.on("/", handleRoot);             //Which routine to handle at root location
  server.on("/action_page", handleForm);  //form action is handled here
  server.on("/SETTINGS", SETTINGS);
  server.on("/DISPON", DISPON);
  server.on("/DISPOFF", DISPOFF);
  server.on("/SWEEP", Servo_Sweep);
  server.on("/FEED_NOW", Feed);
  server.begin();
  DEBUG_PRINTln("HTTP server started");

  timeClient.begin();

  // watchdogTimer = timerBegin(0, 80, true); //timer 0 divisor 80
  // timerAlarmWrite(watchdogTimer, 40000000, false); // set time in uS must be fed within this time or reboot
  // timerAttachInterrupt(watchdogTimer, & interruptReboot, true);
  // timerAlarmEnable(watchdogTimer);  // enable interrupt
}

void loop() {
    DEBUG_PRINT_WAIT("Loop Begin");
    delay(2000);
  timeClient.update();
  ArduinoOTA.handle();
  //esp_task_wdt_reset();
  /////////////////////////////////// WEB SEVER /////////////////////////////////
  server.handleClient();
  //timerWrite(watchdogTimer, 0);
  if (!FEED_NOW) {  //!digitalRead(0)
    FEED_NOW = 1;

    DEBUG_PRINTln("Feeding NOW");
  }
  DEBUG_PRINT_WAIT("LOOPStart");
  ////Disp
  currentMillis = millis();
  if (Dispensor_Cur > PWM_MAX) {
    Dispensor_Cur = PWM_MAX;
  }
  if (Dispensor_Cur < 0) {
    Dispensor_Cur = 0;
  }


  if ((currentMillis - DISP_LAST > DISP_UPDATE_mS) && SLOPE) {
    DISP_LAST = currentMillis;
    if (Dispensor_Cur < (PWM_MAX - 1) && Dispensor_Cur > 0) {
      if (Dispensor == HIGH) {
        Dispensor_Cur = Dispensor_Cur + 1;
        //digitalWrite(Dis_motor, HIGH);//?inverted for relay
      } else if (Dispensor == LOW) {
        Dispensor_Cur = Dispensor_Cur - 1;
        //digitalWrite(Dis_motor, LOW);
        // digitalWrite(Dis_motor, LOW); //?inverted for relay
      }
    }
    cur_on = abs(Dispensor_Cur - 1023);
    //analogWrite(Dis_motor, cur_on); Disabled due to relay addition
  }





  // put your main code here, to run repeatedly:
  if (allprint == 1) {

    DEBUG_PRINTln(scale_read * scale_calibration);
  }

  /////////Internet Time Updating ////////////////////////////////




  if (((currentMillis - S_LAST > S_UPDATE_mS) && !FEED_NOW) || firstSU) {
    S_LAST = currentMillis;
    firstSU = LOW;
    //////////// /UpDate Screen ///////////////////////////////////////////////////////

    // display.backlight();

    if (WiFi.status() != WL_CONNECTED) {

      //display.setCursor(0, 0);
      //display.print("                     ");
      //display.setCursor(18 * 5, 0);
      //display.print("NC");

    } else {
      DEBUG_PRINTln("WiFi connected");
      DEBUG_PRINTln("IP address: ");
      DEBUG_PRINTln(WiFi.localIP());

      //display.setCursor(0, 0);
      //display.print("                     ");
      //display.setCursor(19 * 5, 0);
      //display.print("C");
    }



    // display.setCursor(0, 1 * 8);
    //  display.print(Last_f);
    // display.setCursor(0, 2 * 8);
    // display.print(Last_f2);
  }

  /////////////////   Check on WiFi //////////////////////////////////////
  if (currentMillis - Con_LAST > Con_UPDATE_mS) {
    Con_LAST = currentMillis;
    if (WiFi.status() != WL_CONNECTED) {
      DEBUG_PRINTln("wifi disconnected ");
      WIFI_Connect();
    }
  }
  /////////////////   Time Keeping //////////////////////////////////////

  if (((currentMillis - RTC_LAST > RTC_UPDATE_mS) && UPD_NTC) || firstRTC) {
    RTC_LAST = currentMillis;




    // if (timeClient.update()) {
    if ((timeClient.getHours() < 25) && (timeClient.getMinutes() < 61) && (timeClient.getSeconds() < 61)) {
      DEBUG_PRINT(daysOfTheWeek[timeClient.getDay()]);
      DEBUG_PRINT(", ");
      DEBUG_PRINT(timeClient.getHours());
      DEBUG_PRINT(":");
      DEBUG_PRINT(timeClient.getMinutes());
      DEBUG_PRINT(":");
      DEBUG_PRINTln(timeClient.getSeconds());
      DEBUG_PRINTln(timeClient.getFormattedTime());


      setTime(timeClient.getEpochTime());
      // setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(), timeClient.getDay(), timeClient.getMonth(), timeClient.getYear());

      Clock.setClockMode(false);  // set to 24h

      Clock.setYear(year());
      Clock.setMonth(month());
      Clock.setDate(day());
      Clock.setDoW(weekday());
      Clock.setHour(hour());
      Clock.setMinute(minute());
      Clock.setSecond(second());


      // Clock.setYear(timeClient.getYear());
      //  Clock.setMonth(timeClient.getMonth());
      // Clock.setDate(timeClient.getDay());
      // Clock.setDoW(timeClient.getDay());
      //  Clock.setHour(timeClient.getHours());
      //  Clock.setMinute(timeClient.getMinutes());
      //  Clock.setSecond(timeClient.getSeconds());
      firstRTC = LOW;
    }
    // }
  }

  if ((currentMillis - TIME_LAST > TIME_UPDATE_mS) || firstRTC) {
    TIME_LAST = currentMillis;



    // DateTime now = RTC.now();
    // second_cur = Clock.getSecond();
    // minute_cur = Clock.getMinute();
    // hour_cur = Clock.getHour(h12, PM);
    // date_cur = Clock.getDay();
    // month_cur = Clock.getMonth(Century);
    // year_cur = Clock.getYear();

    second_cur = second();
    minute_cur = minute();
    hour_cur = hour();
    date_cur = day();
    month_cur = month();
    year_cur = year();

    tm.Hour = hour_cur;
    tm.Minute = minute_cur;
    tm.Second = second_cur;
    tm.Day = date_cur;
    tm.Month = month_cur;
    tm.Year = year_cur - 2000;  // because Year is offset fro

    temperature = Clock.getTemperature();

    if (allprint == 0) {
      //NTPch.printDateTime(dateTime);
      DEBUG_PRINTln(days);
      DEBUG_PRINTln(day_feed);
      DEBUG_PRINTln(makeTime(tm));
      DEBUG_PRINT(hour_cur);

      DEBUG_PRINTln("hr");
      DEBUG_PRINTln(makeTime(tm_st));

      DEBUG_PRINT(weight);
      DEBUG_PRINTln("lb");
      DEBUG_PRINT(scale_read);
      DEBUG_PRINTln("Scale");

      DEBUG_PRINT(scale_read * scale_calibration);
      DEBUG_PRINTln("\nScale");
      DEBUG_PRINT(hour_fed);

      DEBUG_PRINTln("hr last feed");
      DEBUG_PRINT(Clock.getMonth(Century));
      DEBUG_PRINT(' ');
      // then the date
      DEBUG_PRINT(Clock.getDate());
      DEBUG_PRINT(' ');
      // and the day of the week
      DEBUG_PRINT(Clock.getDoW());
      DEBUG_PRINT(' ');
      // Finally the hour, minute, and second
      DEBUG_PRINT(Clock.getHour(h12, PM));
      DEBUG_PRINT(' ');
      DEBUG_PRINT(Clock.getMinute());
      DEBUG_PRINT(' ');
      DEBUG_PRINTln(Clock.getSecond());

      display.display();
    }
    display.clearDisplay();
    if (Clock.getHour(h12, PM) <= 24) {
      second_cur = Clock.getSecond();
      minute_cur = Clock.getMinute();
      hour_cur = Clock.getHour(h12, PM);
      date_cur = Clock.getDate();
      month_cur = Clock.getMonth(Century);
      year_cur = Clock.getYear();
    }
    display.setCursor(0, 7);
    display.print(WiFi.localIP());
    display.setCursor(0, 15);
    char buf1[20];

    sprintf(buf1, "%02d:%02d:%02d %02d/%02d/%02d", hour_cur, minute_cur, second_cur, date_cur, month_cur, year_cur);

    display.print(buf1);



    // Current weight and feed total
    display.setCursor(0, 23);

    sprintf(buf1, "%05.3flb  Feed:%05.3flb\nDay:%03d", scale_read * scale_calibration, day_feed / 2, days);

    display.print(buf1);



    display.setCursor(0, 23 + 2 * 8);
    display.print(Last_f);
    display.setCursor(0, 23 + 3 * 8);
    display.print(Last_f2);
    display.display();
  }



  ////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////  Control Loop ////////////////////////////////////////////////////////////

  DEBUG_PRINTln((makeTime(tm) - makeTime(tm_st)) / 3600 / 24);
  //  DEBUG_PRINTln( num_chicks * (0.004095238095 * days + 0.04857142857));
  scale.power_up();
   DEBUG_PRINTln("If scale is ready: " + String(scale.is_ready()) );
  if (scale.is_ready()) {
    if (makeTime(tm) > 0) {
      days = (makeTime(tm) - makeTime(tm_st)) / 3600 / 24;
    }

    day_feed = num_chicks * (Slope * days + Intercept);
    if (day_feed > (max_feed * num_chicks)) {
      day_feed = max_feed * num_chicks;
    }
    DEBUG_PRINT(day_feed);
    DEBUG_PRINT(',');
    reading = scale.read();
    scale_read = reading;
    DEBUG_PRINTln(scale_read * scale_calibration);



    if (FEED_NOW || ((hour_fed + int(100 * days) != hour_cur + int(100 * days)) && ((hour_cur == HOUR_F_1) || (hour_cur == HOUR_F_2)))) {
      if (first_loop) {

        if (do_not_zero == 0) {
          zero = reading;
        } else {
          DEBUG_PRINTln("Scale reading high feeding has likely been interupted, restarting");
          zero = long(Weight_baseline / scale_calibration);
        }



        full = LOW;
        full_ish = LOW;
        first_loop = LOW;
        Hopper_Weight = Hopper_Weight_Max;

        /*

                display.setCursor(0, 1 * 8);
                display.print("F                   ");
                display.setCursor(2 * 5, 1 * 8);
                display.print("TO:");
                display.setCursor(5 * 5, 1 * 8);
                display.print((sec_pp * day_feed / 2));
                display.setCursor(10 * 5, 1 * 8);
                display.print("CT:       ");
                display.setCursor(13 * 5, 1 * 8);
                display.print((currentMillis - FEED_MILLIS) / 1000);
                display.setCursor(0, 2 * 8);
                display.print(Last_f);
        */
        Last_f2 = Last_f;
        Last_f = "";



        FEED_MILLIS = currentMillis;
        myservo.attach(servo_pin);
        for (pos = open_ang; pos >= 0; pos -= 1) {  // goes from 180 degrees to 0 degrees
          myservo.write(pos);                       // tell servo to go to position in variable 'pos'
          delay(5);                                 // waits 15ms for the servo to reach the position
        }
        display.println("Weight Hopper Closed");
      }

      DEBUG_PRINT_WAIT("Checking Feed Needs");

      if ((currentMillis - FEED_MILLIS) / 1000 > ((sec_pp * day_feed / 2))) {
        full = 1;
        digitalWrite(Hoper_motor, LOW);

        DEBUG_PRINTln("Time Overrun");
      }

      if (Feeding) {
        scale_read = reading - zero;
        weight = scale_read * scale_calibration;

        if (Hopper_Weight < weight) {
          // The hopper needs to be emptied ever 12lb to prevent over filling
          Hopper_Weight = Hopper_Weight + Hopper_Weight_Max;

          digitalWrite(Hoper_motor, LOW);
          delay(1000);
          //  Weight_hold=weight;
          for (int posDegrees = 0; posDegrees <= open_ang; posDegrees++) {
            myservo.write(posDegrees);
            DEBUG_PRINTln(posDegrees);
            delay(5);
          }
          delay(20000);
          zero = zero - weight / scale_calibration;

          for (pos = open_ang; pos >= 0; pos -= 1) {  // goes from 180 degrees to 0 degrees
            myservo.write(pos);                       // tell servo to go to position in variable 'pos'
            delay(5);                                 // waits 15ms for the servo to reach the position
          }
          display.println("Weight Hopper Closed");
          myservo.write(0);
          digitalWrite(Hoper_motor, HIGH);

          FEED_MILLIS = FEED_MILLIS + 21120;
        }
        if (!full && (weight < (day_feed / 2))) {
          digitalWrite(Hoper_motor, HIGH);
          if (Dispensor == 0) {
            Dispensor = HIGH;
            digitalWrite(Dis_motor, HIGH);
            Dispensor_Cur = Dispensor_Cur + 1;
          }

          full_ish = LOW;
          DEBUG_PRINT("Filling NOW  ");
          DEBUG_PRINT(weight);
          DEBUG_PRINT("lb  TIME:");


          DEBUG_PRINT((currentMillis - FEED_MILLIS) / 1000);
          DEBUG_PRINT("Sec  TIME Cutoff:");


          DEBUG_PRINT((sec_pp * day_feed / 2));
          DEBUG_PRINTln("Sec");
          DEBUG_PRINT_WAIT("FeedingLOOP");


        } else if (!full && !full_ish && (weight > (day_feed / 2))) {
          full_ish = HIGH;
          digitalWrite(Hoper_motor, LOW);


          delay(3000);
        } else if (!full & full_ish) {
          full = HIGH;
          digitalWrite(Hoper_motor, LOW);


          delay(3000);
        } else if (full) {


          for (int posDegrees = 0; posDegrees <= open_ang; posDegrees++) {
            myservo.write(posDegrees);
            DEBUG_PRINTln(posDegrees);
            delay(5);
          }
          opened = HIGH;
          // myservo.write(30);
          // myservoB.write(150);
          Feeding = LOW;
          hour_fed = hour_cur + 100 * days;
          EEPROM.put(64, hour_fed);
          EEPROM.commit();




          Last_f = "P";
          if (month_cur > 9) {
            Last_f = Last_f + String(month_cur);
          } else {
            Last_f = Last_f + "0";
            Last_f = Last_f + String(month_cur);
          }
          Last_f = Last_f + "/";
          if (date_cur > 9) {
            Last_f = Last_f + String(date_cur);
          } else {
            Last_f = Last_f + "0";
            Last_f = Last_f + String(date_cur);
          }
          Last_f = Last_f + " ";
          if (hour_cur > 9) {
            Last_f = Last_f + String(hour_cur);
          } else {
            Last_f = Last_f + "0";
            Last_f = Last_f + String(hour_cur);
          }
          Last_f = Last_f + ":";
          if (minute_cur > 9) {
            Last_f = Last_f + String(minute_cur);
          } else {
            Last_f = Last_f + "0";
            Last_f = Last_f + String(minute_cur);
          }
          Last_f = Last_f + " ";
          Last_f = Last_f + String(weight);
          Last_f = Last_f + "lb";






          Link = "";

          Link = "GET /update?api_key=" + apiKey + "&field5=";  //Requeste webpage
          Link = Link + String(weight);
          Link = Link + "&field6=";
          Link = Link + String((currentMillis - FEED_MILLIS) / 1000);
          Link = Link + "&field7=";
          Link = Link + String(day_feed / 2);
          Link = Link + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
          if (client.connect(serverTS, 80)) {

            client.print(Link);
            DEBUG_PRINT(Link);
            need2send = LOW;

          } else {
            need2send == HIGH;
            WIFI_Connect();
          }
          client.stop();

          delay(60000);
          FEED_NOW = 0;
          do_not_zero = 0;
          Dispensor = LOW;
          digitalWrite(Dis_motor, LOW);
          Dispensor_Cur = Dispensor_Cur - 1;

          myservo.detach();
        }
      }
      DEBUG_PRINT_WAIT("FeedingLOOPEdn");
    } else {
      Feeding = HIGH;
      first_loop = HIGH;
      myservo.write(open_ang);
      digitalWrite(Hoper_motor, LOW);


      if (opened) {

        for (int posDegrees = open_ang; posDegrees >= 0; posDegrees--) {
          myservo.write(posDegrees);
          DEBUG_PRINTln(posDegrees);
          delay(5);
        }
        opened = LOW;
      }
      //   myservoB.write(180);
    }
    DEBUG_PRINT_WAIT("Feeding If Branch Complete");
          delay(7000);
      DEBUG_PRINT_WAIT("5sec hold complete");
   delay(1000);
 DEBUG_PRINT_WAIT("Loop pre");
  } 

  DEBUG_PRINT_WAIT("Loop");
}



void handleRoot() {
  DEBUG_PRINTln("Loading main page");
  // Read the first line of the request

  HTML += "\r\n<h3> Chicken Feed Dispenser</h3>\r\n";
  HTML += Last_f + "\r\n";
  HTML += "\r\n<br> ";
  HTML += Last_f2 + "\r\n";


  HTML += HTML2;

  // print time to web page
  HTML += "\r\n<br>Time: ";
  HTML += String(hour_cur);
  HTML += ":";
  HTML += String(minute_cur);
  HTML += ":";
  HTML += String(second_cur);
  HTML += "\r\n<br>DATE: ";
  HTML += String(month_cur);
  HTML += "/";
  HTML += String(date_cur);
  HTML += "/";
  HTML += String(year_cur);

  HTML += "\r\n<br>Battery Voltage: ";
  BAT_LEV = 3.3 * (910 + 320) / 320 * 12.09 / 6.25 * 12.11 / 12.34 * double(analogRead(BAT_PIN)) / 4096;
  HTML += String(BAT_LEV);
  HTML += "\r\n<br>SOLAR Voltage: ";
  BAT_LEV = 3.3 * (910 + 200) / 200 * 12.09 / 1.22 * 12.11 / 12.05 * double(analogRead(SOLAR_PIN)) / 4096;
  HTML += String(BAT_LEV);

  HTML += "\r\n<br>Current Weight: ";
  HTML += String(scale_read * scale_calibration, 4);
  HTML += "\r\n<br>Current RAW: ";
  HTML += String(scale.read_average(5));
  HTML += " | Target Weight: ";
  HTML += String(day_feed / 2, 4);
  HTML += " | Chicks are ";
  HTML += String(days);
  HTML += " days old";
  HTML += "\r\n<br>Web Time: ";
  HTML += String(timeClient.getHours());
  HTML += "Web Year: ";
  HTML += String(year());
  HTML += "\r\n";
  HTML += "Tm Year: ";
  HTML += String(tm.Year);
  HTML += "Tm Month: ";
  HTML += String(tm.Month);
  HTML += "Tm Date: ";
  HTML += String(tm.Day);
  HTML += "Tm Hour: ";
  HTML += String(tm.Hour);
  HTML += "Tm Min: ";
  HTML += String(tm.Minute);
  HTML += "Tm Second: ";
  HTML += String(tm.Second);
  HTML += "\r\n <br>";
  Serial.println(scale.read_average(20) * scale_calibration);

  HTML += "Tm Year: ";
  HTML += String(tm_st.Year);
  HTML += "Tm Month: ";
  HTML += String(tm_st.Month);
  HTML += "Tm Date: ";
  HTML += String(tm_st.Day);
  HTML += "Tm Hour: ";
  HTML += String(tm_st.Hour);
  HTML += "Tm Min: ";
  HTML += String(tm_st.Minute);
  HTML += "Tm Second: ";
  HTML += String(tm_st.Second);
  HTML += "\nHourFeed: ";
  HTML += String(hour_fed);
  HTML += "\nDISP: ";
  HTML += String(Dispensor);
  HTML += "DISP_CUR: ";
  HTML += String(Dispensor_Cur);
  HTML += "CUR_ ON: ";
  HTML += String(cur_on);


  HTML += "<br><br>\r\n<a href=\"/DISPON\"\"><button>Soft On</button></a><a href=\"/DISPOFF\"\"><button>Soft Off</button></a>\r\n";
  HTML += "<br><br>\r\n<a href=\"/SWEEP\"\"><button>Servo Sweep</button></a><a href=\"/FEED_NOW\"\"><button>Feed Now</button></a>\r\n";
  HTML += "<br><br>\r\n<a href=\"/SETTINGS\"\"><button>Settings</button>\r\n";
  HTML += "</html>";

  //  server.send(HTML);
  server.send(200, "text/html", HTML);

  HTML = "";
  HTML2 = "";


  delay(1);
  DEBUG_PRINTln("Client disonnected");
  DEBUG_PRINTln("");
}




void SETTINGS() {
  DEBUG_PRINTln("Loading Settings Page");




  HTML2 += "<h3> FEEDER SEETINGS</h3>";
  HTML2 += "<form action=\"/action_page\">";
  HTML2 += "Chick Hatch Date (day|month|year) :<br>";
  HTML2 += "<input type=\"text\" name=\"DAY\" value=";
  HTML2 += String(day_st);
  HTML2 += ">";

  HTML2 += "<input type=\"text\" name=\"MONTH\" value=";
  HTML2 += String(month_st);
  HTML2 += ">";

  HTML2 += "<input type=\"text\" name=\"YEAR\" value=";
  HTML2 += String(year_st);
  HTML2 += "><br>";

  HTML2 += "Number of Chickens:<br>";
  HTML2 += "<input type=\"text\" name=\"NCIK\" value=";
  HTML2 += String(num_chicks);
  HTML2 += "><br>";

  HTML2 += "Hour for fist feeding:<br>";
  HTML2 += "<input type=\"text\" name=\"HF1\" value=";
  HTML2 += String(HOUR_F_1);
  HTML2 += "><br>";

  HTML2 += "Hour for second feeding:<br>";
  HTML2 += "<input type=\"text\" name=\"HF2\" value=";
  HTML2 += String(HOUR_F_2);
  HTML2 += "><br>";

  HTML2 += "Upper limit number of seconds to despense a pound of food:<br>";
  HTML2 += "<input type=\"text\" name=\"SECPP\" value=";
  HTML2 += String(sec_pp);
  HTML2 += "><br>";

  HTML2 += "Maximun Per Chicken Feed Dispensed per Day:<br>";
  HTML2 += "<input type=\"text\" name=\"MXF\" value=";
  HTML2 += String(max_feed, 4);
  HTML2 += "><br>";

  HTML2 += "Feeding Weight Slope:<br>";
  HTML2 += "<input type=\"text\" name=\"SLP\" value=";
  HTML2 += String(Slope, 16);
  HTML2 += "><br>";

  HTML2 += "Feeding Weight Intercept:<br>";
  HTML2 += "<input type=\"text\" name=\"INT\" value=";
  HTML2 += String(Intercept, 16);
  HTML2 += "><br>";

  HTML2 += "Scale Calibration:<br>";
  HTML2 += "<input type=\"text\" name=\"CAL\" value=";
  HTML2 += String(scale_calibration, 16);
  HTML2 += "><br>";


  HTML2 += "Max PWM:<br>";
  HTML2 += "<input type=\"text\" name=\"PWM\" value=";
  HTML2 += String(PWM_MAX);
  HTML2 += "><br>";


  HTML2 += "Weight_baseline:<br>";
  HTML2 += "<input type=\"text\" name=\"WBL\" value=";
  HTML2 += String(Weight_baseline, 16);
  HTML2 += "><br>";

  HTML2 += "<br><br>";
  HTML2 += "<input type=\"submit\" value=\"Submit\">";
  HTML2 += "</form> ";
  handleRoot();
}

//===============================================================
// This routine is executed when you press submit on settings page
//===============================================================
void handleForm() {

  if (currentMillis - EEPROM_Upload_LAST >= EEPROM_Upload_ms) {
    EEPROM_Upload_LAST = currentMillis;


    String DAY_S = server.arg("DAY");
    String MONTH_S = server.arg("MONTH");
    String YEAR_S = server.arg("YEAR");
    String NCIK_S = server.arg("NCIK");
    String HF1_S = server.arg("HF1");
    String HF2_S = server.arg("HF2");
    String SECPP_S = server.arg("SECPP");
    String MXF_S = server.arg("MXF");
    String SLP_S = server.arg("SLP");
    String INT_S = server.arg("INT");
    String CAL_S = server.arg("CAL");
    String PWM_S = server.arg("PWM");
    String WBL_S = server.arg("WBL");

    day_st = DAY_S.toInt();
    month_st = MONTH_S.toInt();
    year_st = YEAR_S.toInt();
    num_chicks = NCIK_S.toInt();
    HOUR_F_1 = HF1_S.toInt();
    HOUR_F_2 = HF2_S.toInt();
    sec_pp = SECPP_S.toFloat();
    max_feed = MXF_S.toFloat();
    Slope = SLP_S.toDouble();
    Intercept = INT_S.toDouble();
    scale_calibration = CAL_S.toDouble();
    PWM_MAX = PWM_S.toDouble();
    Weight_baseline = WBL_S.toDouble();


    EEPROM.put(0, day_st);
    EEPROM.put(4, month_st);
    EEPROM.put(8, year_st);
    EEPROM.put(12, num_chicks);
    EEPROM.put(16, HOUR_F_1);
    EEPROM.put(20, HOUR_F_2);
    EEPROM.put(24, sec_pp);
    EEPROM.put(28, max_feed);
    EEPROM.put(32, Slope);
    EEPROM.put(40, Intercept);
    EEPROM.put(48, scale_calibration);
    EEPROM.put(56, PWM_MAX);
    EEPROM.put(72, Weight_baseline);




    EEPROM.commit();


    DEBUG_PRINT("SETTING EEPROM || Day Start: ");
    DEBUG_PRINTln(day_st);
    DEBUG_PRINT("SETTING EEPROM || Month Start: ");
    DEBUG_PRINTln(month_st);
    DEBUG_PRINT("SETTING EEPROM || Year Start: ");
    DEBUG_PRINTln(year_st);
    DEBUG_PRINT("SETTING EEPROM || Number of Chicks: ");
    DEBUG_PRINTln(num_chicks);
    DEBUG_PRINT("SETTING EEPROM || Hour Feed 1: ");
    DEBUG_PRINTln(HOUR_F_1);
    DEBUG_PRINT("SETTING EEPROM || Hour Feed 2: ");
    DEBUG_PRINTln(HOUR_F_2);
    DEBUG_PRINT("SETTING EEPROM || Max Days feed per Chicken: ");
    DEBUG_PRINTln(sec_pp);
    DEBUG_PRINT("SETTING EEPROM || Seconds per Pound of Feed: ");
    DEBUG_PRINTln(max_feed);
    DEBUG_PRINT("SETTING EEPROM || Feed Slope: ");
    DEBUG_PRINTln(Slope);
    DEBUG_PRINT("SETTING EEPROM || Feed Intercept: ");
    DEBUG_PRINTln(Intercept);
    DEBUG_PRINT("SETTING EEPROM || Scale Calibration: ");
    DEBUG_PRINTln(scale_calibration);
    DEBUG_PRINT("SETTING EEPROM || Weight_baseline: ");
    DEBUG_PRINTln(Weight_baseline);
    handleRoot();
    tm_st.Day = day_st;
    tm_st.Month = month_st;
    tm_st.Year = year_st - 2000;  // because Year is offset fro
  }
}

void DISPON() {
  Dispensor = HIGH;
  Dispensor_Cur = Dispensor_Cur + 1;
  HTML2 = "SOFT ON REQUESTED";
  handleRoot();
  SLOPE = HIGH;
}

void DISPOFF() {
  Dispensor = LOW;
  Dispensor_Cur = Dispensor_Cur - 1;
  HTML2 = "SOFT OFF REQUESTED";
  handleRoot();
  SLOPE = HIGH;
}


void WIFI_Connect() {
  WiFi.disconnect();
  DEBUG_PRINTln("Connecting to WiFi...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  for (int i = 0; i < 60; i++) {
    if (WiFi.status() != WL_CONNECTED) {
      delay(250);
      DEBUG_PRINT(".");
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_PRINTln("");
    DEBUG_PRINTln("WiFi Connected");
    DEBUG_PRINTln("IP address: ");
    DEBUG_PRINTln(WiFi.localIP());
  }
}

/*

  void readFromRTCMemory() {
  system_rtc_mem_read(RTCMEMORYSTART, &rtcMem, sizeof(rtcMem));

  DEBUG_PRINT("count = ");
  DEBUG_PRINTln(rtcMem.count);
  yield();
  }

  void writeToRTCMemory() {
  if (rtcMem.count <= MAXHOUR) {
    rtcMem.count++;
  } else {
    rtcMem.count = 0;
  }

  system_rtc_mem_write(RTCMEMORYSTART, &rtcMem, 4);

  DEBUG_PRINT("count = ");
  DEBUG_PRINTln(rtcMem.count);
  yield();
  }
*/

void Feed() {
  FEED_NOW = 1;
  DEBUG_PRINTln("Feeding NOW from HTTP command");
}
void Servo_Sweep() {
  handleRoot();
  myservo.attach(servo_pin);
  int pos;
  //
  for (pos = 0; pos <= 180; pos += 1) {  // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);  // tell servo to go to position in variable 'pos'
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Sweeping Servo");

    drawProgressbar(5, 20, 120, 10, (double(pos) * 100 / 180));
    display.display();
    delay(5);  // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) {  // goes from 180 degrees to 0 degrees
    myservo.write(pos);                  // tell servo to go to position in variable 'pos'
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Sweeping Servo");

    drawProgressbar(5, 20, 120, 10, (double(pos) * 100 / 180));
    display.display();
    delay(5);  // waits 15ms for the servo to reach the position
  }

  myservo.detach();
}
void drawProgressbar(int x, int y, int width, int height, int progress) {
  progress = progress > 100 ? 100 : progress;  // set the progress value to 100
  progress = progress < 0 ? 0 : progress;      // start the counting to 0-100
  float bar = ((float)(width - 1) / 100) * progress;
  display.drawRect(x, y, width, height, WHITE);
  display.fillRect(x + 2, y + 2, bar, height - 4, WHITE);  // initailize the graphics fillRect(int x, int y, int width, int height)
}
