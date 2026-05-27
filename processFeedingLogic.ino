void processFeedingLogic() {
  if (dumping==1){
    if ((millis() - HopperWheel_LAST > (hop_pps * 1000 * weight2dump))) {
    dumping = 2;
    display.clearDisplay();
    display.setCursor(0, 23 + 3 * 8);
    display.print("Hopper has emptied");
    display.display();
    delay(1000);
    DEBUG_PRINTln("Hopper has emptied");
    digitalWrite(adj_pin, LOW);
    }
  }
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
      if (first_loop) {  // Setup feeder for feeding
        if (do_not_zero == 0) {
          zero = reading;
        } else {
          display.setCursor(0, 0);
          display.println("Scale reading high feeding has likely been interupted, restarting");
          display.display();
          DEBUG_PRINTln("Scale reading high feeding has likely been interupted, restarting");
          zero = long(Weight_baseline / scale_calibration);
        }

        full = LOW;
        full_ish = LOW;
        first_loop = LOW;
        Hopper_Weight = Hopper_Weight_Max;


        FEED_MILLIS = currentMillis;

        servoClose(0);
        display.println("Weight Hopper Closed");
        display.display();
        dooropen();
        display.println("Opening Feed Door");
        display.display();
        //delay(door_timeout * 255 / PWMDO);
        //dooroff();
      }



      DEBUG_PRINT_WAIT("Checking Feed Needs");

      if (Feeding&&(!mid_empty && ((currentMillis - FEED_MILLIS) / 1000 > ((sec_pp * day_feed / 2))))) {
        full = 1;
        digitalWrite(Hoper_motor, LOW);
        DEBUG_PRINTln("Time Overrun");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Time Overrun");
        display.display();
        delay(500);
      }

      if (Feeding && (door_status==0)) {
        scale_read = reading - zero;
        weight = scale_read * scale_calibration;

        if ((Hopper_Weight < weight) || mid_empty) {
          if (mid_empty == 0) {
            // The hopper needs to be emptied ever 12lb to prevent over filling
            digitalWrite(Hoper_motor, LOW);
            //delay(1000);
            //  Weight_hold=weight;
            
            Hopper_Weight = Hopper_Weight + Hopper_Weight_Max;
            weight2dump=(weight - (Hopper_Weight - 2*Hopper_Weight_Max));
            zero = zero - weight2dump / scale_calibration ;

            //  weight2dump=(weight - (Hopper_Weight - Hopper_Weight_Max));
            servoOpen();
            pause_feed = millis();
            display.setCursor(0, 0);
            display.println("To prevent hopper overfill, dumping");
            display.display();
            
            //servoClose(20000);
            
            mid_empty = 1;
          } else {
            if (dumping == 2) {
              dumping = 0;
              mid_empty = 0;
              FEED_MILLIS = FEED_MILLIS + millis() - pause_feed;
              display.setCursor(0, 0);
              display.println("Weight Hopper Closed");
              display.display();
              digitalWrite(Hoper_motor, HIGH);
            }
          }
        } else if (!full && (weight < (day_feed / 2))) {
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
        } else if (full && (dumping == 0)) {  // dumping=0  - dump not triggered //  dumping = 1 - actively dumpping   //  dumping = 2 - recently completed dumping
          //weight=
          weight_fed=weight;
          zero = reading;
          digitalWrite(Hoper_motor, LOW);  //Timmer overrun trigged a full call during the dumping cycle witch triggered the servo open rutine since the dump flag was set to 1 instead of 0
          servoOpen();  //
        } else if (full && (dumping == 2)) {
          Feeding = LOW;
          hour_fed = hour_cur + 100 * days;
          EEPROM.put(64, hour_fed);
          EEPROM.commit();
          create_fed_status_string();
          writeTS();
          //delay(60000);
          FEED_NOW = 0;
          do_not_zero = 0;
          Dispensor = LOW;
          digitalWrite(Dis_motor, LOW);
          //Dispensor_Cur = Dispensor_Cur - 1;
          doorclose();
          servoClose(60000);
          dumping = 0;
        }
      }
      DEBUG_PRINT_WAIT("FeedingLOOPEdn");
    } else {
      Feeding = HIGH;
      first_loop = HIGH;
      myservo.write(open_ang);
      digitalWrite(Hoper_motor, LOW);
      if (opened) {
        servoClose(0);
      }

      //   myservoB.write(180);
    }
  }
}


void create_fed_status_string() {
  Last_f2 = Last_f;
  Last_f = "";


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
  Last_f = Last_f + String(weight_fed);
  Last_f = Last_f + "lb";
}
