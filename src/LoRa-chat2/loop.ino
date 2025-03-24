void loop() {
  digitalWrite(LED, !digitalRead(LED));                //migat`!

  if (digitalRead(TOUCH_IRQ) == 0) {                   //Touch IRQ
    //    rgb.clear();
    //    rgb.show();
    tft.getTouchRaw(&x_touch, &y_touch);
    if (tft.getTouchRawZ() > touchForce) {             //Touch threshold
      GetKeyPress(MyBuffer);
      delay(keyDelay);
    }
  }





  if (millis() % 1000 == 0) {   ////////////// Update CLOCK once per second. + other shit
    DS3231_get(&tiime);
    sprintf_P(s, PSTR("%02d.%02d.%02d  %02d:%02d:%02d"), tiime.mday, tiime.mon, tiime.year_s , tiime.hour, tiime.min, tiime.sec);
    sprintf_P(sdate, PSTR("%02d%02d%02d"), tiime.mday, tiime.mon, tiime.year);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setCursor(211, 231);
    tft.print(s);
    lightTimeout ++;
    if (lightTimeout >= lcd_timeout) {
      digitalWrite(TFT_LED, LOW), lightTimeout = lcd_timeout;                                //TFT LED off
      if (newM == true) {                                                                    //New message received. Unread message.
        rgb.setBrightness(random(255));                                                      //of 255
        rgb.setPixelColor(0, rgb.Color(random(255), random(255), random(255)));
        rgb.show();
        if (motorTime >= 10) {
          digitalWrite(MOTOR, HIGH);
          tone(PIEZO, 4000, 500);
          delay(100);
          digitalWrite(MOTOR, LOW);
          motorTime = 0;
        }
        motorTime++;
      }
    }
    tft.setCursor(1, 231);
    bat = analogRead(BAT_LVL) * (3.3 / 1023) * 1.75;                                       //1.75 = voltage divider resistors (100k and 75k). IDEAL equation: can be adj-ed for more accurate values
    if (bat < 3.6) {
      rgb.setBrightness(random(10) + 1);                                                   //of 255
      rgb.setPixelColor(0, rgb.Color(random(255), 0, 0));
      rgb.show();
      tft.setTextColor(ILI9341_PINK, ILI9341_BLACK);
    }
    if ((bat >= 3.6) and (light == false)) {
      rgb.setPixelColor(0, rgb.Color(0, 0, 0));
      rgb.show();
    }
    tft.print(bat);
    tft.print("v");


  }






  button1.Update();                                                   //n = number of clicks, -1 for long
  if (button1.clicks == 1) {                                          //1 click
    digitalWrite(TFT_LED, HIGH), lightTimeout = 0;                    //TFT light on
    if (light == false) {
      rgb.setBrightness(255);
      rgb.setPixelColor(0, rgb.Color(255, 255, 255));
      rgb.show();
      light = true;
      return;
    }
    if (light == true) {
      rgb.setBrightness(10);
      rgb.clear();
      rgb.show();
      light = false;
    }
  }
  if (button1.clicks == 2) tone(PIEZO, 2000, 75), delay(75), showMenu();          //double click to enter menu
  //if (button1.clicks == -1) {}                                                  //long click SOS wtf?






  if (Serial.available() > 0) {                                                   //handle incoming serial text, the first char is arrived!
    delay(4);                                                                     //wait for others to join the buffer. 2ms is enough for PUTTY.
    digitalWrite(TFT_LED, HIGH), lightTimeout = 0;                                //TFT light on
    String SerialRX;
    byte rText = 0;
    while (Serial.available()) {
      byte cbc = Serial.read();
      if ((cbc > 31) and (rText < textLimit) and (cbc < 127)) {
        SerialRX = SerialRX + (char)cbc;
        rText++;
      }
    }
    delay(4);
    while (Serial.available()) char t = Serial.read();                            //purge serial if any
    if (SerialRX.length() > 0) loraTx((char*)SerialRX.c_str());                   //send and log
  }











}
