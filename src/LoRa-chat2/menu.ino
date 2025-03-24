void showMenu() {
  inMenu = true;
  menuStay = true;
  tone(PIEZO, 1000, 3);                            //Tick
  digitalWrite(MOTOR, HIGH);
  delay(40);
  digitalWrite(MOTOR, LOW);
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextSize(2);
  digitalWrite(TFT_LED, HIGH);                     //TFT light on

  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(100, 3);
  tft.print(F("Main menu"));
  tft.setTextColor(ILI9341_WHITE, 0xf000);        //draw menu buttons
  drawButton(294, 1, 25, 25);                     //exit button
  tft.setCursor(302, 6);
  tft.print('X');
  drawButton(20, 30, 107, 25);                    //set time button
  tft.setCursor(25, 36);
  tft.print(F("Set time"));
  drawButton(20, 60, 107, 25);                    //set LORA button
  tft.setCursor(25, 66);
  tft.print(F("Set LoRa"));
  drawButton(20, 90, 107, 25);                    //touch debug button
  tft.setCursor(25, 96);
  if (showTouchDebug) tft.setTextColor(ILI9341_YELLOW, 0xf000);
  tft.print(F("TouchDBG"));
  tft.setTextColor(ILI9341_WHITE, 0xf000);
  drawButton(20, 120, 107, 25);                   //info button
  tft.setCursor(52, 126);
  tft.print(F("Info"));
  drawButton(20, 150, 107, 25);                   //reset button
  tft.setCursor(41, 156);
  tft.print(F("RESET!"));


  while (menuStay == true) {                             //menu loop
    if (digitalRead(TOUCH_IRQ) == 0) {                   //touch IRQ
      tft.getTouchRaw(&x_touch, &y_touch);
      if (tft.getTouchRawZ() > touchForce) {             //touch threshold
        tone(PIEZO, 1000, 3);                            //tick
        digitalWrite(MOTOR, HIGH);
        delay(40);
        digitalWrite(MOTOR, LOW);


        if (TouchButton(20, 60, 107, 25)) {              //lora set button
          tone(PIEZO, 2000, 75);
          delay(75);
          menuLora();
          return;
        }
        if (TouchButton(20, 30, 107, 25)) {              //time set button
          tone(PIEZO, 2000, 75);
          delay(75);
          menuTime();
          return;
        }
        if (TouchButton(20, 150, 107, 25)) {             //reset button
          tone(PIEZO, 2000, 75);
          reFormat();
        }
        if (TouchButton(294, 1, 25, 25)) {               //exit button
          menuStay = false;
          tone(PIEZO, 2000, 75);
          inMenu = false;
          lightTimeout = 0;
          break;
        }
        if (TouchButton(20, 90, 107, 25)) {              //colored touch debug button
          showTouchDebug = !showTouchDebug;
          tone(PIEZO, 2000, 75);
          tft.setCursor(25, 96);
          if (showTouchDebug) tft.setTextColor(ILI9341_YELLOW, 0xf000);
          if (!showTouchDebug) tft.setTextColor(ILI9341_WHITE, 0xf000);
          tft.print(F("TouchDBG"));
        }
        if (TouchButton(20, 120, 107, 25)) {              //info button
          tone(PIEZO, 2000, 75);
          tft.fillScreen(ILI9341_BLUE);
          tft.setTextColor(ILI9341_WHITE, 0xf000);
          drawButton(294, 1, 25, 25);                     //exit button
          tft.setCursor(302, 6);
          tft.print('X');
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
          tft.setCursor(130, 3);
          tft.println(F("Info"));
          tft.setTextSize(1);
          tft.setTextColor(ILI9341_GREEN, ILI9341_BLUE);
          tft.println();
          tft.print(F(" LoRa messenger by "));
          tft.println(welc);
          tft.println();
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
          tft.println(F(" url: https://github.com/Airrr17/LoRa_messenger"));
          tft.println();
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
          tft.print(F(" Current LoRa settings:"));
          tft.print(F("         Frequency: ")), tft.println(Lora_Freq);
          tft.setCursor(198, 67), tft.print(F("Tx power: ")), tft.println(Lora_TxPower);
          tft.setCursor(150, 76), tft.print(F("Spreading Factor: ")), tft.println(Lora_SpreadingFactor);
          tft.setCursor(150, 85), tft.print(F("Signal Bandwidth: ")), tft.println(Lora_SignalBandwidth);
          tft.setCursor(186, 94), tft.print(F("CodingRate: 4/")), tft.println(Lora_CodingRate4);
          tft.setCursor(198, 103), tft.print(F("SyncWord: ")), tft.println(Lora_SyncWord);
          tft.setCursor(228, 112), tft.print(F("CRC: ")), tft.println(Lora_Crc);
          tft.drawRect(148, 57, 165, 64, ILI9341_WHITE);                               //around lora
          tft.drawRect(148, 122, 165, 8, ILI9341_WHITE);                               //around rssi
          tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);

          tft.setTextSize(2);
          tft.setCursor(12, 94), tft.print (F("SD ID: ")), tft.println(myID);
          tft.setTextSize(1);
          tft.drawFastHLine(0, 120, 148, ILI9341_WHITE);
          tft.setCursor(0, 150);
          tft.println (F("Frequency - this sw version is for 410-525MHz module."));
          tft.println (F("For 1/4 wave antenna length see calculator @[SetLoRa]"));
          tft.println (F("Antenna is very important part!"));
          tft.println (F("Spread Factor: high = longer range, less datarate."));
          tft.println (F("Bandwidth: low number = longer range, less datarate."));

          while (!TouchButton(294, 1, 25, 25)) {                                           //exit button
            DS3231_get(&tiime);
            tft.setCursor(0, 76), tft.print(F(" Date: ")), tft.print(tiime.mday), tft.print("."), tft.print(tiime.mon), tft.print("."), tft.print(tiime.year), tft.print(", "), tft.print(DAYS[tiime.wday - 1]), tft.println("   ");
            tft.print(F(" Time: ")), tft.print(tiime.hour), tft.print(":"), tft.print(tiime.min), tft.print(":"), tft.print(tiime.sec), tft.println("   ");
            bat = analogRead(BAT_LVL) * (3.3 / 1023) * 1.75;
            tft.setCursor(0, 111), tft.print (F(" Battery: ")), tft.print(bat), tft.print("v  ");
            tft.setCursor(0, 123);
            tft.print(F(" Current RSSI: "));
            int currentRssi = LoRa.rssi();
            tft.print(currentRssi);                                                        //min as in datasheet -141, max let be 0
            tft.println(F("dBm  "));
            tft.print(F(" Temperature : "));
            tft.print(DS3231_get_treg());
            tft.print((char)247);                                                          //glcdfont.c -> replace 0x06, 0x0F, 0x09, 0x0F, 0x06 with 0x06, 0x09, 0x09, 0x09, 0x06
            tft.print("C");

            if (currentRssi >= 0) currentRssi = 0;
            if (currentRssi <= -141) currentRssi = -141;
            byte w = map(abs(currentRssi), 0, 141, 1, 161);
            if ((currentRssi > -120) and (currentRssi < -50)) tft.fillRect(150, 124, w, 4, ILI9341_YELLOW);                 //rssi - 161 pix length!
            if (currentRssi <= -120) tft.fillRect(150, 124, w, 4, ILI9341_RED);
            if (currentRssi >= -50) tft.fillRect(150, 124, w, 4, ILI9341_GREEN);
            tft.fillRect(150 + w, 124, 161 - w, 4, ILI9341_BLUE);                          //zater
            delay(250);
            digitalWrite(LED, !digitalRead(LED));

          }
          tone(PIEZO, 2000, 75);
          delay(75);
          showMenu();
          return;
        }



        delay(keyDelay);                                 //Keys done

      }      //getTouchRawZ
    }        //TOUCH_IRQ


  }          //menu loop


  MakeKB_Button(Mobile_KB);                               //return
}









void menuTime() {                                         //rtc
  timeMenuStay = true;
  tone(PIEZO, 1000, 3);                                   //tick
  digitalWrite(MOTOR, HIGH);
  delay(40);
  digitalWrite(MOTOR, LOW);
  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(100, 3);
  tft.print(F("Time menu"));
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
  tft.setCursor(27,  35), tft.print(F("Min:"));
  tft.setCursor(15,  65), tft.print(F("Hour:"));
  tft.setCursor(15,  95), tft.print(F("Week:"));
  tft.setCursor(27, 125), tft.print(F("Day:"));
  tft.setCursor(3, 155), tft.print(F("Month:"));
  tft.setCursor(15, 185), tft.print(F("Year:"));
  byte t_m = tiime.min;
  byte t_h = tiime.hour;
  byte t_w = tiime.wday;
  byte t_d = tiime.mday;
  byte t_M = tiime.mon;
  word t_y = tiime.year;
  tft.setTextDatum(MC_DATUM);
  tft.drawString(String(t_m), 136, 43);
  tft.drawString(String(t_h), 136, 73);
  tft.drawString(String(DAYS[t_w - 1]), 136, 103);
  tft.drawString(String(t_d), 136, 133);
  tft.drawString(String(t_M), 136, 163);
  tft.drawString(String(t_y), 136, 193);
  tft.setTextDatum(TL_DATUM);

  tft.setTextColor(ILI9341_WHITE, 0xf000);
  for (byte i = 28; i < 180; i = i + 30) {  //    [+]&[-] buttons
    drawButton(80, i, 20, 25);
    tft.setCursor(85, i + 5);
    tft.print('-');
    drawButton(170, i, 20, 25);
    tft.setCursor(175, i + 5);
    tft.print('+');
  }

  drawButton(249, 209, 68, 25);                   //time apply button
  tft.setCursor(254, 214);
  tft.print(F("Apply"));

  drawButton(294, 1, 25, 25);                     //exit button
  tft.setCursor(302, 6);
  tft.print('X');

  while (timeMenuStay == true) {
    if (digitalRead(TOUCH_IRQ) == 0) {            //touch IRQ
      tft.getTouchRaw(&x_touch, &y_touch);
      if (tft.getTouchRawZ() > touchForce) {      //touch threshold
        tone(PIEZO, 1000, 3);                     //tick
        digitalWrite(MOTOR, HIGH);
        delay(40);
        digitalWrite(MOTOR, LOW);
        bool changed = false;

        if (TouchButton(249, 209, 68, 25)) {      //apply button
          timeMenuStay = false;
          tone(PIEZO, 2000, 75);
          delay(75);
          struct ts t;
          t.sec = 0;
          t.min = t_m;
          t.hour = t_h;
          t.wday = t_w;
          t.mday = t_d;
          t.mon = t_M;
          t.year = t_y;
          DS3231_set(t);
          delay(100);
          DS3231_get(&tiime);
          sprintf_P(sdate, PSTR("%02d%02d%02d"), tiime.mday, tiime.mon, tiime.year);
          sprintf_P(s, PSTR("%02d.%02d.%02d  %02d:%02d:%02d"), tiime.mday, tiime.mon, tiime.year_s , tiime.hour, tiime.min, tiime.sec);

          tft.setCursor(3, 214);
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          tft.print(s);

          delay(2000);
          showMenu();
          return;                                                        //Apply
        }

        if (TouchButton(294, 1, 25, 25)) {                               //exit button
          timeMenuStay = false;
          tone(PIEZO, 2000, 75);
          delay(75);
          showMenu();
          return;
        }
        if (TouchButton(80, 28, 20, 25)) {                               //-minutes button
          if (t_m == 0) changed = true, t_m = 59;
          if ((t_m > 0) and (changed == false)) changed = true, t_m--;
          if (t_m == 9) tft.setCursor(124, 35), tft.print("  ");
        }
        if (TouchButton(170, 28, 20, 25)) {                              //+minutes button
          if (t_m == 59) changed = true, t_m = 0;
          if ((t_m < 59) and (changed == false)) changed = true, t_m++;
          if (t_m == 0) tft.setCursor(124, 35), tft.print("  ");
        }

        if (TouchButton(80, 58, 20, 25)) {                               //-hours button
          if (t_h == 0) changed = true, t_h = 23;
          if ((t_h > 0) and (changed == false)) changed = true, t_h--;
          if (t_h == 9) tft.setCursor(124, 65), tft.print("  ");
        }
        if (TouchButton(170, 58, 20, 25)) {                              //+hours button
          if (t_h == 23) changed = true, t_h = 0;
          if ((t_h < 23) and (changed == false)) changed = true, t_h++;
          if (t_h == 0) tft.setCursor(124, 65), tft.print("  ");
        }

        if (TouchButton(80, 88, 20, 25)) {                               //-weeks button
          if (t_w == 1) changed = true, t_w = 7;
          if ((t_w > 1) and (changed == false)) changed = true, t_w--;
        }
        if (TouchButton(170, 88, 20, 25)) {                              //+weeks button
          if (t_w == 7) changed = true, t_w = 1;
          if ((t_w < 7) and (changed == false)) changed = true, t_w++;
        }

        if (TouchButton(80, 118, 20, 25)) {                               //-days button
          if (t_d == 1) changed = true, t_d = 31;
          if ((t_d > 1) and (changed == false)) changed = true, t_d--;
          if (t_d == 9) tft.setCursor(124, 125), tft.print("  ");
        }
        if (TouchButton(170, 118, 20, 25)) {                              //+days button
          if (t_d == 31) changed = true, t_d = 1;
          if ((t_d < 31) and (changed == false)) changed = true, t_d++;
          if (t_d == 1) tft.setCursor(124, 125), tft.print("  ");
        }

        if (TouchButton(80, 148, 20, 25)) {                               //-month button
          if (t_M == 1) changed = true, t_M = 12;
          if ((t_M > 1) and (changed == false)) changed = true, t_M--;
          if (t_M == 9) tft.setCursor(125, 155), tft.print("  ");
        }
        if (TouchButton(170, 148, 20, 25)) {                              //+month button
          if (t_M == 12) changed = true, t_M = 1;
          if ((t_M < 12) and (changed == false)) changed = true, t_M++;
          if (t_M == 1) tft.setCursor(125, 155), tft.print("  ");
        }

        if (TouchButton(80, 178, 20, 25)) {                               //-year button
          if (t_y > 2025) t_y--;
        }
        if (TouchButton(170, 178, 20, 25)) {                              //+year button
          if (t_y < 2222) t_y++;                                          //doomsday
        }

        tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(String(t_m), 136, 43);
        tft.drawString(String(t_h), 136, 73);
        tft.drawString(String(DAYS[t_w - 1]), 136, 103);
        tft.drawString(String(t_d), 136, 133);
        tft.drawString(String(t_M), 136, 163);
        tft.drawString(String(t_y), 136, 193);
        tft.setTextDatum(TL_DATUM);                      //default


        delay(keyDelay);                                 //keys done
      }

      delay(keyDelay / 4);

      //tft.setTextSize(1);


    }


  }


}








void menuLora() {
  int Lora_Freq_temp = Lora_Freq;
  int Lora_TxPower_temp = Lora_TxPower;
  int Lora_SpreadingFactor_temp = Lora_SpreadingFactor;
  int Lora_SignalBandwidth_temp = Lora_SignalBandwidth;
  int Lora_CodingRate4_temp = Lora_CodingRate4;
  int Lora_SyncWord_temp = Lora_SyncWord;
  bool Lora_Crc_temp = Lora_Crc;

  loraMenuStay = true;
  tone(PIEZO, 1000, 3);                                   //Tick
  digitalWrite(MOTOR, HIGH);
  delay(40);
  digitalWrite(MOTOR, LOW);
  tft.setTextSize(2);
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(100, 3);
  tft.print(F("LoRa menu"));

  tft.setTextColor(ILI9341_WHITE, 0xf000);

  char freqBuffer[9];                                //divide to bytes
  byte freqs[9];
  itoa(Lora_Freq_temp, freqBuffer, 10);              //first to char; int, buffer, base
  for (byte j = 0; j < 9; j++) {                     //then to individual bytes array (0 to 8)
    freqs[j] = freqBuffer[j] - 48;
  }

  drawButton(249, 209, 68, 25);                      //time apply button
  tft.setCursor(254, 214);
  tft.print(F("Apply"));

  drawButton(294, 1, 25, 25);                        //exit button
  tft.setCursor(302, 6);
  tft.print('X');

  for (byte i = 113; i < 230; i = i + 32) {          //[+]&[-] buttons
    drawButton(79, i, 20, 20);
    tft.setCursor(84, i + 3);
    tft.print('-');
    drawButton(145, i, 20, 20);
    tft.setCursor(150, i + 3);
    tft.print('+');
  }
  drawButton(229, 113, 20, 20), tft.setCursor(234, 116), tft.print('-');         //sync word
  drawButton(295, 113, 20, 20), tft.setCursor(300, 116), tft.print('+');
  drawButton(229, 145, 20, 20), tft.setCursor(234, 148), tft.print('-');         //crc
  drawButton(295, 145, 20, 20), tft.setCursor(300, 148), tft.print('+');

  byte b = 0;
  for (word i = 7; i < 300; i = i + 36) {            //[+]&[-] buttons freq
    tft.setTextColor(ILI9341_WHITE, 0xf000);
    drawButton(i, 35, 20, 20);
    tft.setCursor(i + 5, 38);
    tft.print('+');
    drawButton(i, 80, 20, 20);
    tft.setCursor(i + 5, 83);
    tft.print('-');
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
    tft.setCursor(i + 4, 61);
    tft.print(freqs[b]);
    b++;
  }
  tft.setCursor(100, 61), tft.print("."), tft.setCursor(208, 61), tft.print(".");

  tft.drawFastHLine(0, 107, 320, ILI9341_WHITE);
  tft.drawFastVLine(168, 107, 133, ILI9341_WHITE);
  tft.drawFastHLine(168, 173, 152, ILI9341_WHITE);

  tft.setTextSize(1);
  tft.setCursor(3, 120), tft.print(F("TXpower dBm:"));
  tft.setCursor(3, 152), tft.print(F("Spreading F:"));
  tft.setCursor(3, 184), tft.print(F("S.bandwidth:"));
  tft.setCursor(3, 216), tft.print(F("Coding Rate:"));
  tft.setCursor(171, 120), tft.print(F("SyncWord:"));
  tft.setCursor(201, 152), tft.print(F("CRC:"));
  tft.setCursor(171, 176), tft.print(F("QW. Ant. length: "));
  calcAnt(Lora_Freq_temp);
  tft.print((float)antennaL);
  tft.print(F("cm"));
  tft.setCursor(171, 185), tft.print(F("Data rate: "));
  tft.print(Lora_SpreadingFactor_temp * (Lora_SignalBandwidth_temp / (float)(2 << (Lora_SpreadingFactor_temp - 1))) * (4.0 / Lora_CodingRate4_temp));
  tft.print(F("bps"));


  tft.setTextDatum(MC_DATUM);
  tft.drawString(String(Lora_TxPower), 121, 124);
  tft.drawString(String(Lora_SpreadingFactor), 121, 156);
  tft.drawString(String(Lora_SignalBandwidth), 121, 188);
  tft.drawString("4/" + String(Lora_CodingRate4), 121, 220);
  tft.drawString(String(Lora_SyncWord), 272, 124);
  tft.drawString(String(Lora_Crc), 272, 156);
  tft.setTextDatum(TL_DATUM);



  byte vot = 0;
  for (byte v = 0; v < 10; v++) {
    if (Lora_SignalBandwidth_temp == Band[v]) vot = v;                                            //vot
  }


  while (loraMenuStay == true) {                  //ooooooooghhhhhhhhhhhhhhhhhhh my buttons
    if (digitalRead(TOUCH_IRQ) == 0) {            //touch IRQ
      tft.getTouchRaw(&x_touch, &y_touch);
      if (tft.getTouchRawZ() > touchForce) {      //touch threshold
        tone(PIEZO, 1000, 3);                     //tick
        digitalWrite(MOTOR, HIGH);
        delay(40);
        digitalWrite(MOTOR, LOW);
        bool changed = false;
        bool changed2 = false;
        bool freq_changed = false;

        if (TouchButton(294, 1, 25, 25)) {                                                       //exit button
          loraMenuStay = false;
          tone(PIEZO, 2000, 75);
          delay(75);
          showMenu();
          return;
        }

        if (TouchButton(295, 113, 20, 20)) {                                                      //sync - button
          if (Lora_SyncWord_temp == 255) changed = true, Lora_SyncWord_temp = 0;
          if ((Lora_SyncWord_temp < 255) and (changed == false)) Lora_SyncWord_temp++;
          tft.setCursor(263, 120), tft.print("   ");
        }
        if (TouchButton(229, 113, 20, 20)) {                                                      //sync + button
          if (Lora_SyncWord_temp == 0) changed = true, Lora_SyncWord_temp = 255;
          if ((Lora_SyncWord_temp > 0) and (changed == false)) Lora_SyncWord_temp--;
          tft.setCursor(263, 120), tft.print("   ");
        }
        if (TouchButton(295, 145, 20, 20) or TouchButton(229, 145, 20, 20)) Lora_Crc_temp = !Lora_Crc_temp;//crc +- button

        if (TouchButton(145, 113, 20, 20)) {                                                     //TXpwr - button
          if (Lora_TxPower_temp == 20) changed = true, Lora_TxPower_temp = 1;
          if ((Lora_TxPower_temp < 20) and (changed == false)) Lora_TxPower_temp++;
          if (Lora_TxPower_temp == 1) tft.setCursor(115, 120), tft.print("  ");
        }
        if (TouchButton(79, 113, 20, 20)) {                                                      //TXpwr + button
          if (Lora_TxPower_temp == 1) changed = true, Lora_TxPower_temp = 20;
          if ((Lora_TxPower_temp > 1) and (changed == false)) Lora_TxPower_temp--;
          if (Lora_TxPower_temp == 9) tft.setCursor(115, 120), tft.print("  ");
        }
        if (TouchButton(145, 145, 20, 20)) {                                                     //spreading factor - button    +32pix
          if (Lora_SpreadingFactor_temp < 12) Lora_SpreadingFactor_temp++;
        }
        if (TouchButton(79, 145, 20, 20)) {                                                      //spreading factor + button
          if (Lora_SpreadingFactor_temp > 6) Lora_SpreadingFactor_temp--;
          if (Lora_SpreadingFactor_temp == 9) tft.setCursor(115, 152), tft.print("  ");
        }
        if (TouchButton(145, 209, 20, 20)) {                                                     //codingRate - button
          if (Lora_CodingRate4_temp < 8) Lora_CodingRate4_temp++;
        }
        if (TouchButton(79, 209, 20, 20)) {                                                      //codingRate + button
          if (Lora_CodingRate4_temp > 5) Lora_CodingRate4_temp--;
        }

        if (TouchButton(145, 177, 20, 20)) {                                                     //signal bandwidth - button
          if (vot < 9) vot++;
          tft.setCursor(102, 184), tft.print(F("      "));
          Lora_SignalBandwidth_temp = Band[vot];
        }
        if (TouchButton(79, 177, 20, 20)) {                                                      //signal bandwidth + button
          if (vot > 0) vot--;
          tft.setCursor(102, 184), tft.print(F("      "));
          Lora_SignalBandwidth_temp = Band[vot];
        }


        word g = 7;                                                                              //freq buttons -> byte freqs[9];
        for (byte h = 0; h < 9; h++) {
          if (TouchButton(g, 35, 20, 20)) {                                                      //0 + and go on   +36pix left
            if (freqs[h] == 9) freq_changed = true, freqs[h] = 0;
            if ((freqs[h] < 9) and (freq_changed == false)) freqs[h]++;
            changed2 = true;                                                                     //to calculate F
          }
          if (TouchButton(g, 80, 20, 20)) {                                                      //0 - and go on   +36pix
            if (freqs[h] == 0) freq_changed = true, freqs[h] = 9;
            if ((freqs[h] > 0) and (freq_changed == false)) freqs[h]--;
            changed2 = true;
          }
          g = g + 36;
        }



        if (TouchButton(249, 209, 68, 25)) {                                                     //apply button
          Lora_Freq = Lora_Freq_temp;
          Lora_TxPower = Lora_TxPower_temp;
          Lora_SpreadingFactor = Lora_SpreadingFactor_temp;
          Lora_SignalBandwidth = Lora_SignalBandwidth_temp;
          Lora_CodingRate4 = Lora_CodingRate4_temp;
          Lora_SyncWord = Lora_SyncWord_temp;
          Lora_Crc = Lora_Crc_temp;
          loraApply();
          save_config();
          delay(50);
          tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
          tft.setTextSize(2);
          tft.setCursor(172, 214);
          tft.print (F("  OK! "));


          tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
          tft.setTextSize(1);
          delay(2000);
          showMenu();
          return;





        }



        tft.setTextDatum(MC_DATUM);
        tft.drawString(String(Lora_TxPower_temp), 121, 124);
        tft.drawString(String(Lora_SpreadingFactor_temp), 121, 156);
        tft.drawString(String(Lora_SignalBandwidth_temp), 121, 188);
        tft.drawString("4/" + String(Lora_CodingRate4_temp), 121, 220);
        tft.drawString(String(Lora_SyncWord_temp), 272, 124);
        tft.drawString(String(Lora_Crc_temp), 272, 156);
        tft.setTextDatum(TL_DATUM);


        tft.setCursor(237, 185);
        tft.print(Lora_SpreadingFactor_temp * (Lora_SignalBandwidth_temp / (float)(2 << (Lora_SpreadingFactor_temp - 1))) * (4.0 / Lora_CodingRate4_temp));
        tft.print(F("bps  "));



        if (changed2) {                                       //freq changed, calculator and printer

          tft.setTextSize(2);
          Lora_Freq_temp = 0;
          int d = 100000000;
          for (byte v = 0; v < 9; v++) {                      //to int!
            Lora_Freq_temp = Lora_Freq_temp + freqs[v] * d;
            d = d / 10;
          }

          if (Lora_Freq_temp > maxF) Lora_Freq_temp = maxF;  //correct if any
          if (Lora_Freq_temp < minF) Lora_Freq_temp = minF;

          word f = 11;
          itoa(Lora_Freq_temp, freqBuffer, 10);              //first to char; int, buffer, base
          for (byte j = 0; j < 9; j++) {                     //then to individual bytes array (0 to 8)
            freqs[j] = freqBuffer[j] - 48;
            tft.setCursor(f, 61), tft.print(freqs[j]);
            f = f + 36;
          }
          tft.setTextSize(1);

          calcAnt(Lora_Freq_temp);
          tft.setCursor(273, 176);
          tft.print((float)antennaL);
          tft.print(F("cm"));

          changed2 = false;
        }









        delay(keyDelay);                                 //keys done
      }    //if touch?


    }     //if touched


  }       //while







}
