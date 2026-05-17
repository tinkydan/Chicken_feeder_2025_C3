#include "HX711.h"
#include <TimeLib.h>
#include "NTPClient.h"
#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <NetworkUdp.h>
//LiquidCrystal_I2C lcd(0x3F, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
// Bring in our logo bitmap to keep things clean
#include "coop_logo.h"
#include "debug.h"
#include "variables.h"


void setup() {
  pinMode(Hoper_motor, OUTPUT);
  digitalWrite(Hoper_motor, LOW);
  pinMode(Dis_motor, OUTPUT);
  digitalWrite(Dis_motor, LOW);
  pinMode(IN1, OUTPUT);
  digitalWrite(IN1, LOW);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN1, LOW);

  scale.power_up();

  Wire.begin(6, 7);
  Serial.begin(115200);
  DEBUG_PRINTln("Loading Scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  delay(500);
  DEBUG_PRINTln(scale.read_average(20) * scale_calibration);
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
    EEPROM.put(80, PWMDO);
    EEPROM.put(84, PWMDC);

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
  EEPROM.get(80, PWMDO);
  EEPROM.get(84, PWMDC);


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
  //Serial.println("Testing");
  //Serial.println(Slope);
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
  tm_st.Year = year_st;  //- 2000;  // because Year is offset fro

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
  while ((WiFi.status() != WL_CONNECTED) && (iter < 100)) {

    DEBUG_PRINT(".");
    drawProgressbar(5, 20, 120, 10, (double(iter)));
    display.display();
    delay(100);
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



  if (!MDNS.begin("feeder")) {  // http://esp32.local
    MDNS.addService("http", "tcp", 80);
    DEBUG_PRINTln("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }


  DEBUG_PRINTln("seting up server");
  server.on("/", handleRoot);             //Which routine to handle at root location
  server.on("/action_page", handleForm);  //form action is handled here
  server.on("/SETTINGS", SETTINGS);
  server.on("/DISPON", DISPON);
  server.on("/DISPOFF", DISPOFF);
  server.on("/SWEEP", Servo_Sweep);
  server.on("/FEED_NOW", Feed);
  server.on("/OPEN", dooropen);
  server.on("/CLOSE", doorclose);
  server.on("/DOOROFF", dooroff);
  server.on("/DEBUG", Debugpage);
  server.on("/SERVOOPEN", servoOpen);
  server.on("/SERVOCLOSE", servoClose);
  server.begin();
  DEBUG_PRINTln("HTTP server started");



  timeClient.begin();

  // watchdogTimer = timerBegin(0, 80, true); //timer 0 divisor 80
  // timerAlarmWrite(watchdogTimer, 40000000, false); // set time in uS must be fed within this time or reboot
  // timerAttachInterrupt(watchdogTimer, & interruptReboot, true);
  // timerAlarmEnable(watchdogTimer);  // enable interrupt
}


void loop() {
  //DEBUG_PRINT_WAIT("Loop Begin");

  currentMillis = millis();
  runSystemHousekeeping();  // Handle OTA, Web Server, NTP, and WiFi
  handleDispenserMotor();   // Manage the dispenser motor PWM
  updateTimeAndScreen();    // Sync RTC and update the OLED
  processFeedingLogic();    // The main control loop for the scale and servos


  //if (!digitalRead(8)) {
  // FEED_NOW = 1;
  // DEBUG_PRINTln("Feeding NOW");
  // }
  //DEBUG_PRINT_WAIT("LOOPStart");

  // put your main code here, to run repeatedly:
  if (allprint == 1) {
    DEBUG_PRINTln(scale_read * scale_calibration);
  }
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
  HTML += String(scale_read);  //scale.read_average(5));
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
  //Serial.println(scale.read_average(20) * scale_calibration);

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
  HTML += "<br><br>\r\n<a href=\"/DEBUG\"\"><button>DEBUG page</button>\r\n";
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

  HTML2 += "Door Open PWM (0-255):<br>";
  HTML2 += "<input type=\"text\" name=\"PWMDO\" value=";
  HTML2 += String(PWMDO);
  HTML2 += "><br>";

  HTML2 += "Door Close PWM (0-255):<br>";
  HTML2 += "<input type=\"text\" name=\"PWMDC\" value=";
  HTML2 += String(PWMDC);
  HTML2 += "><br>";

  HTML2 += "<br><br>";
  HTML2 += "<input type=\"submit\" value=\"Submit\">";
  HTML2 += "</form> ";
  handleRoot();
}




void Debugpage() {
  DEBUG_PRINTln("Loading DEBUG Page");

  HTML2 += "<h3> FEEDER Manual Options</h3>";
  HTML2 += "<br><br>\r\n<a href=\"/FEED_NOW\"\"><button>Run Feeding Cycle</button></a>\r\n";
  HTML2 += "<br><br>\r\n<a href=\"/DISPON\"\"><button>Dispensor ON</button></a>    <a href=\"/DISPOFF\"\"><button>Dispensor OFF</button></a>\r\n";
  HTML2 += "<br><br>\r\n<a href=\"/SWEEP\"\"><button>Servo Sweep</button></a>\r\n";
  HTML2 += "<br><br>\r\nDispensor DOOR: <a href=\"/OPEN\"\"><button>OPEN</button></a> <a href=\"/CLOSE\"\"><button>CLOSE</button></a> <a href=\"/DOOROFF\"\"><button>OFF</button></a>\r\n";
  HTML2 += "<br><br>\r\n<a href=\"/SERVOOPEN\"\"><button>Hopper Flap Open</button></a>    <a href=\"/SERVOCLOSE\"\"><button>Hopper Flap Close</button></a>\r\n";
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
    String PWM_DO = server.arg("PWMDO");
    String PWM_DC = server.arg("PWMDC");

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
    PWMDO = PWM_DO.toInt();
    PWMDC = PWM_DC.toInt();


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
    EEPROM.put(80, PWMDO);
    EEPROM.put(84, PWMDC);




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
    DEBUG_PRINT("SETTING EEPROM || Door Open PWM: ");
    DEBUG_PRINTln(PWMDO);
    DEBUG_PRINT("SETTING EEPROM || Door Close PWM: ");
    DEBUG_PRINTln(PWMDC);
    handleRoot();
    tm_st.Day = day_st;
    tm_st.Month = month_st;
    tm_st.Year = year_st;  // because Year is offset fro
  }
}


void dooropen() {
  if ((PWMDC == 255) && (PWMDO == 255)) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    analogWrite(IN1, PWMDO);
    analogWrite(IN2, 0);
  }
  door_off_time = millis() + door_timeout * 255 / PWMDO;
}

void doorclose() {
  if ((PWMDC == 255) && (PWMDO == 255)) {
    digitalWrite(IN2, HIGH);
    digitalWrite(IN1, LOW);
  } else {
    analogWrite(IN2, PWMDC);
    analogWrite(IN1, 0);
  }
  door_off_time = millis() + door_timeout * 255 / PWMDC;
}

void dooroff() {
  if ((PWMDC == 255) && (PWMDO == 255)) {
    digitalWrite(IN2, LOW);
    digitalWrite(IN1, LOW);
  } else {
    analogWrite(IN2, 0);
    analogWrite(IN1, 0);
  }
  door_off_time = 0;
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

void servoOpen() {
  myservo.attach(servo_pin);
  for (int posDegrees = 0; posDegrees <= open_ang; posDegrees++) {
    myservo.write(posDegrees);
    DEBUG_PRINTln(posDegrees);
    delay(5);
  }
  opened = HIGH;
  myservo.detach();
}
void servoClose() {

  myservo.attach(servo_pin);
  //

  for (int posDegrees = open_ang; posDegrees >= 0; posDegrees--) {
    myservo.write(posDegrees);
    DEBUG_PRINTln(posDegrees);
    delay(5);
  }
  opened = LOW;
  myservo.detach();
  // }
}
void drawProgressbar(int x, int y, int width, int height, int progress) {
  progress = progress > 100 ? 100 : progress;  // set the progress value to 100
  progress = progress < 0 ? 0 : progress;      // start the counting to 0-100
  float bar = ((float)(width - 1) / 100) * progress;
  display.drawRect(x, y, width, height, WHITE);
  display.fillRect(x + 2, y + 2, bar, height - 4, WHITE);  // initailize the graphics fillRect(int x, int y, int width, int height)
}
