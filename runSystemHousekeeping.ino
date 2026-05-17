void runSystemHousekeeping() {
  timeClient.update();
  ArduinoOTA.handle();

  //esp_task_wdt_reset();
  /////////////////////////////////// WEB SEVER /////////////////////////////////
  server.handleClient();
  //timerWrite(watchdogTimer, 0);


  /////////////////   Check on WiFi //////////////////////////////////////
  if (currentMillis - Con_LAST > Con_UPDATE_mS) {
    Con_LAST = currentMillis;
    if (WiFi.status() != WL_CONNECTED) {
      DEBUG_PRINTln("wifi disconnected ");
      WIFI_Connect();
    }
  }

  /////////////// Thinkspeak resend if unsucsessful send
  if ((currentMillis - ThgS_LAST > ThgS_UPDATE_mS) && need2send) {
    if (client.connect(serverTS, 80)) {
      client.print(Link);
      DEBUG_PRINT(Link);
      need2send = LOW;
    }
  }

  if ((door_off_time > 0) && ((currentMillis > door_off_time))) {
    dooroff();
  }
}



unsigned long ota_progress_millis = 0;

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 0.25 second
  if (millis() - ota_progress_millis > 250) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}
