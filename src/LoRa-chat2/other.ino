void onReceive(int packetSize) {                               //Receiving a packet

  if (packetSize) {
    String loraRX;
    newM = true;
    digitalWrite(TFT_LED, HIGH), lightTimeout = 0;             //TFT LED on
    loraRX = LoRa.readString();
    String v1stRX = F("00000000");                             //[8][8][1][1][xIDx][0123456789012345678901234567890123456789012345678]
    String v2ndRX = F("11111111");                             //zzzzzzzz888888885D1FXY7777777777777777777777777777777777777777777777777
    String ttlRX = ("x");
    String contrRX = ("y");                                    //0123456701234567XYvasy!77777777777777777777777777777777777777777777777!
    String idRX = F("none");                                   //zzzzzzzz88888888XY5D1F7777777777777777777777777777777777777777777777777
    String textRX = loraRX;

    if (packetSize > 22) {                                     //23 or more. standard LoRa messenger packet, suppose to be...fucxothers
      v1stRX = loraRX.substring(0, 8);                         //[8][8][1][1][xIDx][0123456789012345678901234567890123456789012345678]
      v2ndRX = loraRX.substring(8, 16);                        //zzzzzzzz888888885D1FXY7777777777777777777777777777777777777777777777777
      ttlRX = loraRX.substring(16, 17);
      contrRX = loraRX.substring(17, 18);                      //0123456701234567XYvasy!77777777777777777777777777777777777777777777777!
      idRX = loraRX.substring(18, 22);                         //zzzzzzzz88888888XY5D1F7777777777777777777777777777777777777777777777777
      textRX = loraRX.substring(22, 71);
    }

    //log:
    appendLogLine(String(s) + F("  <<< ") + v1stRX + "-" + v2ndRX + "-" + ttlRX + "-" + contrRX + "-" + idRX + ":" + textRX + ":RSSI=" + LoRa.packetRssi() + ":SNR=" + LoRa.packetSnr() + ":FE=" + LoRa.packetFrequencyError());

    moveUP();
    vseLinii2[9].v1st = v1stRX;
    vseLinii2[9].v2nd = v2ndRX;
    vseLinii2[9].ttl = ttlRX;
    vseLinii2[9].contr = contrRX;
    vseLinii2[9].id = idRX;
    vseLinii2[9].text = textRX;

    if (inMenu == false) printMsgs();


  }



}







void loraTx(char * loraText) {                              //packet forming and sending and logging
  String v1stTX = "00000000";                               //dummy, no use for now, must be eight!
  String v2ndTX = "11111111";                               //dummy, no use for now, must be eight!
  String ttlTX = "x";                                       //dummy, no use for now, must be one!
  String contrTX = "y";                                     //dummy, no use for now, must be one!

  TXbuffer = v1stTX + v2ndTX + ttlTX + contrTX + myID + loraText;

  //log:
  appendLogLine(String(s) + F("  >>> ") + v1stTX + "-" + v2ndTX + "-" + ttlTX + "-" + contrTX + "-" + myID + ":" + loraText + ":");

  //send:
  if (TRANSMIT) {                                   //do a real TX
    LoRa.beginPacket();                             //------------------------------------
    LoRa.print(TXbuffer);
    LoRa.endPacket();
    LoRa.receive();                                 //put the radio back into receive mode
  }

  moveUP();

  vseLinii2[9].v1st = v1stTX;
  vseLinii2[9].v2nd = v2ndTX;
  vseLinii2[9].ttl = ttlTX;
  vseLinii2[9].contr = contrTX;
  vseLinii2[9].id = myID;
  vseLinii2[9].text = loraText;

  printMsgs();

}








void moveUP() {
  for (byte q = 0; q < 9; q++) {                   //each up!!!
    vseLinii2[q].v1st = vseLinii2[q + 1].v1st;
    vseLinii2[q].v2nd = vseLinii2[q + 1].v2nd;
    vseLinii2[q].ttl = vseLinii2[q + 1].ttl;
    vseLinii2[q].contr = vseLinii2[q + 1].contr;
    vseLinii2[q].id = vseLinii2[q + 1].id;
    vseLinii2[q].text = vseLinii2[q + 1].text;
  }
}









void printMsgs() {                                                           //TFT printing and parsing all 10 msgs
  tft.fillRect(0, 0, 320, 83, ILI9341_BLUE);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  byte printLine = 1;
  for (byte q = 0; q < 10; q++) {
    tft.setCursor(1, printLine);
    if (vseLinii2[q].text.length() > 0) {
      if (myID == vseLinii2[q].id) {                                         //treat my own message
        tft.setTextColor(ILI9341_PINK, ILI9341_BLUE);                        //ILI9341_PINK
      }
      else {
        tft.setTextColor(ILI9341_RED, ILI9341_BLUE);
      }
      tft.print(vseLinii2[q].id);

      if (myID == vseLinii2[q].id) {                                         //my own message
        tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
      }
      else {
        tft.setTextColor(ILI9341_CYAN, ILI9341_BLUE);
      }
      tft.print(vseLinii2[q].text);
    }

    printLine = printLine + 8;
  }

  //tft.setTextSize(2);
  //touchWait();
}









void MakeKB_Button(const char type[][13]) {
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE, 0xf800);
  for (int y = 0; y < 3; y++)
  {
    int ShiftRight = 15 * pgm_read_byte(&(type[y][0]));
    for (int x = 3; x < 13; x++)
    {
      if (x >= pgm_read_byte(&(type[y][1]))) break;

      drawButton(15 + (30 * (x - 3)) + ShiftRight, 100 + (30 * y), 20, 25);
      tft.setCursor(20 + (30 * (x - 3)) + ShiftRight, 105 + (30 * y));
      tft.print(char(pgm_read_byte(&(type[y][x]))));
    }
  }
  //ShiftKey
  drawButton(15, 160, 35, 25);
  tft.setCursor(27, 168);
  tft.print('^');

  //Special Characters
  drawButton(15, 190, 35, 25);
  tft.setCursor(21, 195);
  tft.print(F("SY"));

  //BackSpace
  drawButton(270, 160, 35, 25);
  tft.setCursor(276, 165);
  tft.print(F("<-"));

  //Return
  drawButton(270, 190, 35, 25);
  tft.setCursor(276, 195);
  tft.setTextColor(ILI9341_BLACK, ILI9341_RED);
  tft.print(F("Ok"));
  tft.setTextColor(ILI9341_WHITE, ILI9341_RED);

  //Spacebar
  drawButton(60, 190, 200, 25);
  tft.setCursor(105, 195);
  tft.print(F("SPACE BAR"));

  tft.fillRect(1, 83, 318, 12, 0x03EF);                       //text input field 0x3186

  tft.setTextSize(1);

  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(3, 85);                                       //print the text buffer
  tft.print(MyBuffer);
  

  tft.setTextColor(ILI9341_GREENYELLOW, ILI9341_BLACK);
  tft.setCursor(305, 85);
  tft.print(textLimit);
  printMsgs();

}








void loraApply() {
  LoRa.end();
  if (LoRa.begin(Lora_Freq)) tone(PIEZO, 1200, 30), delay(30), tone(PIEZO, 1800, 75);  //LoRa.setFrequency(Lora_Freq);//<- need to send something to get started somewhy(((
  LoRa.setTxPower(Lora_TxPower);                     //1-20
  LoRa.setSpreadingFactor(Lora_SpreadingFactor);     //6-12
  LoRa.setSignalBandwidth(Lora_SignalBandwidth);     //7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3
  LoRa.setCodingRate4(Lora_CodingRate4);             //Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8.
  LoRa.setSyncWord(Lora_SyncWord);                   //def 0x12=18dec hz: (0xF3);  // ranges from 0-0xFF, default 0x34, see API
  if (Lora_Crc == true) LoRa.enableCrc();            //LoRa CRC
  if (Lora_Crc == false) LoRa.disableCrc();

  LoRa.onReceive(onReceive);
  LoRa.receive();

}








void drawButton(int x, int y, int w, int h) {
  tft.fillRoundRect(x - 3, y + 3, w, h, 3, 0x8888);                  //button shading
  tft.fillRoundRect(x, y, w, h, 3, 0xffff);                          //outter button color
  tft.fillRoundRect(x + 1, y + 1, w - 1 * 2, h - 1 * 2, 3, 0xf800);  //inner button color
}








byte TouchButton(int x, int y, int w, int h) {
  int X, Y;
  if (tft.getTouchRawZ() > touchForce) {                              //touch theshold again
    tft.getTouchRaw(&x_touch, &y_touch);

    Y = map(x_touch, TS_MINY, TS_MAXY, 240, 0);                       //TS_MINY, TS_MAXY, 0, 240 //need to mirror?
    X = map(y_touch, TS_MINX, TS_MAXX, 0, 320);                       //TS_MINX, TS_MAXX, 0, 320
    if (showTouchDebug) tft.drawPixel( X, Y, ILI9341_YELLOW);         //////////////////// for touching debug or calibrating
    digitalWrite(TFT_LED, HIGH), lightTimeout = 0;                    //TFT light on
    newM = false;
    return (IsWithin(X, x, x + w) & IsWithin(Y, y, y + h));
  }
  //control reaches end of non-void function... must return something, todo later
}








void touchWait() {
  while (digitalRead(TOUCH_IRQ) == HIGH) {            //Just wait for touch
    digitalWrite(LED, !digitalRead(LED));             //migat`!
    delay (15);
  }
}







void calcAnt(int fr) {                                                 //return float antennaL
  float lambda = (float)299792458 / (float)fr;
  antennaL = ((lambda / (float)4)) * 100;                              //meters to cm
}
