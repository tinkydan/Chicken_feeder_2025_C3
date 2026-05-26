void updateTimeAndScreen() {
  /////////Internet Time Updating ////////////////////////////////
  WifiStatusScreen();
  updateRTC_NPT();
  updateTimeScreen();
}

void WifiStatusScreen() {
  if (((currentMillis - S_LAST > S_UPDATE_mS) && !FEED_NOW) || firstSU) {
    S_LAST = currentMillis;
    firstSU = LOW;
    //////////// /UpDate Screen ///////////////////////////////////////////////////////
    // display.backlight();
    if (WiFi.status() != WL_CONNECTED) {

      display.setCursor(0, 0);
      display.print("                     ");
      display.setCursor(18 * 5, 0);
      display.print("NC");

    } else {
      DEBUG_PRINTln("WiFi connected");
      DEBUG_PRINTln("IP address: ");
      DEBUG_PRINTln(WiFi.localIP());

      display.setCursor(0, 0);
      display.print("                     ");
      display.setCursor(19 * 5, 0);
      display.print("C");
    }



    // display.setCursor(0, 1 * 8);
    //  display.print(Last_f);
    // display.setCursor(0, 2 * 8);
    // display.print(Last_f2);
  }
}


void updateRTC_NPT() {
  /////////////////   Time Keeping //////////////////////////////////////
  if (((currentMillis - RTC_LAST > RTC_UPDATE_mS) && UPD_NTC) || firstRTC) {
    RTC_LAST = currentMillis;
    //DEBUG_PRINT("Year from timeClient: " + String(timeClient.getYear()));
    if ((timeClient.getHours() < 25) && (timeClient.getMinutes() < 61) && (timeClient.getSeconds() < 61)) {
      DEBUG_PRINT(daysOfTheWeek[timeClient.getDay()]);
      DEBUG_PRINT(", ");
      DEBUG_PRINT(timeClient.getHours());
      DEBUG_PRINT(":");
      DEBUG_PRINT(timeClient.getMinutes());
      DEBUG_PRINT(":");
      DEBUG_PRINTln(timeClient.getSeconds());
      DEBUG_PRINTln(timeClient.getFormattedTime());
      DEBUG_PRINTln("Epoch " + String(timeClient.getEpochTime()));
      setTime(timeClient.getEpochTime());
      // setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(), timeClient.getDay(), timeClient.getMonth(), timeClient.getYear());
      DEBUG_PRINTln("Year from defauls" + String(year()));
      Clock.setClockMode(false);  // set to 24h

      Clock.setYear(year() - 2000);
      DEBUG_PRINTln("Year from Clock " + String(Clock.getYear()));

      Clock.setMonth(month());
      Clock.setDate(day());
      Clock.setDoW(weekday());
      Clock.setHour(hour());
      Clock.setMinute(minute());
      Clock.setSecond(second());

      firstRTC = LOW;
    }
    // }
  }
}

void updateTimeScreen() {
  if ((currentMillis - TIME_LAST > TIME_UPDATE_mS) || firstRTC) {
    TIME_LAST = currentMillis;


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
    tm.Year = year_cur;  // - 2000;  // because Year is offset fro

    //temperature = Clock.getTemperature();

    if (allprint == 1) {
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

    //    sprintf(buf1, "%02d:%02d:%02d %02d/%02d/%02d", hour_cur, minute_cur, second_cur, date_cur, month_cur, year_cur);

    //  display.print(buf1);
    display.print(String(hour_cur) + ":" + String(minute_cur) + ":" + String(second_cur) + " " + String(year_cur) + "." + String(month_cur) + "." + String(date_cur));


    // Current weight and feed total
    display.setCursor(0, 23);

    //  sprintf(buf1, "%05.3flb  Feed:%05.3flb\nDay:%03d", scale_read * scale_calibration, day_feed / 2, days);

    // display.print(buf1);
    display.print(String(scale_read * scale_calibration, 2) + "lb  Feed:" + String(day_feed / 2, 2) + "lb\nDay:" + String(days) +" hw: "+ String(Hopper_Weight,1));

    if (door_status > 0) {

      //display.clearDisplay();
      display.setCursor(0, 23 + 2 * 8);
      if (door_status == 1) {
        display.println("Opening Door:");
      } else {
        display.println("Closing Door:");
      }
      drawProgressbar(5, 50, 120, 10, (1 - double(double(door_off_time - millis()) / double(door_timeout * 1000 * 255 / PWMDC))) * 100);
    }
    if (dumping == 1) {
      //display.clearDisplay();
      display.setCursor(0, 23 + 2 * 8);
      display.println("Empting " + String(weight2dump,2) + "lb Hopper:");
      drawProgressbar(5, 50, 120, 10, (double(millis() - HopperWheel_LAST )/ double(hop_pps * 1000 * weight2dump))*100);

    } else {
      display.setCursor(0, 23 + 2 * 8);
      display.print(Last_f);
      display.setCursor(0, 23 + 3 * 8);
      display.print(Last_f2);
    }
    display.display();
  }
  //DEBUG_PRINTln((makeTime(tm) - makeTime(tm_st)) / 3600 / 24);
}