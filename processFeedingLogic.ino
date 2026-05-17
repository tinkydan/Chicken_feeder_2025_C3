void processFeedingLogic() {


  //  DEBUG_PRINTln("If scale is ready: " + String(scale.is_ready()) );
  if (scale.is_ready()) {
    if (makeTime(tm) > 0) {
      days = (makeTime(tm) - makeTime(tm_st)) / 3600 / 24;
    }
    DEBUG_PRINTln("Scale is ready: " + String(scale.is_ready()));
    day_feed = num_chicks * (Slope * days + Intercept);
    if (day_feed > (max_feed * num_chicks)) {
      day_feed = max_feed * num_chicks;
    }
    Serial.println(scale.read());
    DEBUG_PRINT(day_feed);
    DEBUG_PRINT(',');
    reading = scale.read();
    //if (reading>0){
    scale_read = reading;
    // }


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

        servoClose();
        display.println("Weight Hopper Closed");
        dooropen();
        delay(door_timeout * 255 / PWMDO);
        dooroff();
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
          servoOpen();
          delay(20000);
          zero = zero - weight / scale_calibration;

          servoClose();
          display.println("Weight Hopper Closed");
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


          servoOpen();
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
          doorclose();

          FEED_NOW = 0;
          do_not_zero = 0;
          Dispensor = LOW;
          digitalWrite(Dis_motor, LOW);
          Dispensor_Cur = Dispensor_Cur - 1;
          servoClose();
 
        }
      }
      DEBUG_PRINT_WAIT("FeedingLOOPEdn");
    } else {
      Feeding = HIGH;
      first_loop = HIGH;
      myservo.write(open_ang);
      digitalWrite(Hoper_motor, LOW);
      if (opened) {
      //servoClose();}

      //   myservoB.write(180);
    }
  }
}