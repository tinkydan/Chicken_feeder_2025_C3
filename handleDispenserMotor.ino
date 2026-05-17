void handleDispenserMotor () {
 
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
}









