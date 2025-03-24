void setup() {
  Serial.begin(115200);
  delay(2000);                                  //USB serial init time.
  Serial.println(F("---------------------------------------------"));
  Serial.println(welc);                         //Serial out welcome msg. firstly.

  pinMode(LORA_RESET, OUTPUT);                  //LoRa Reset pin
  pinMode(LORA_IRQ, INPUT_PULLUP);              //LoRa Interrupt pin
  pinMode(TOUCH_SS, OUTPUT);                    //Touch slave select
  pinMode(TOUCH_IRQ, INPUT_PULLUP);             //Touch IRQ
  pinMode(TFT_SS, OUTPUT);                      //TFT slave select
  pinMode(TFT_DC, OUTPUT);                      //TFT DC
  pinMode(TFT_LED, OUTPUT);                     //TFT LED
  pinMode(LED, OUTPUT);                         //OnBoard LED
  pinMode(SD_SS, OUTPUT);                       //SD card slave select
  pinMode(BAT_LVL, INPUT);                      //BAT.LVL
  pinMode(PIEZO, OUTPUT);                       //Piezo
  pinMode(MOTOR, OUTPUT);                       //Motor
  pinMode(BUTTON, INPUT_PULLUP);                //Button

  delay(60);                                    //bzzz a little
  digitalWrite(MOTOR, LOW);                     //Motor off

  rgb.begin();                                  //RGB led start
  if (fastBoot == false) {
    for (byte o = 0; o < 150; o++) {
      rgb.setBrightness(o);                     //255
      rgb.setPixelColor(0, rgb.Color(0, o, o)); //cyan
      rgb.show();
      delay(8);
    }
    tone(PIEZO, 1000, 300), delay(300), tone(PIEZO, 800, 350), delay(350), tone(PIEZO, 500, 350);  //Startup sound
  }
  else {
    tone(PIEZO, 1000, 300);
    rgb.setBrightness(10);                          //255
    rgb.setPixelColor(0, rgb.Color(0, 100, 100));   //cyan
    rgb.show();
  }

  bat = analogRead(BAT_LVL) * (3.3 / 1023) * 1.75;  //1.75 = for voltage divider resistors (100k and 75k)
  Serial.print(F("Battery voltage: "));
  Serial.println(bat);

  digitalWrite(TOUCH_SS, HIGH);                     //set all slave select pins high
  digitalWrite(TFT_SS, HIGH);
  digitalWrite(SD_SS, HIGH);
  digitalWrite(TFT_LED, HIGH);                      //TFT LED on

  Wire.begin();                                     //For RTC & optional BMP280. SCL-PB6, SDA-PB7
  Serial.println(F("TFT init..."));
  tft.begin();
  tft.fillScreen(ILI9341_NAVY);
  tft.setRotation(3);

  delay (50);
  tft.setTextSize(2);                               //WELCOME BIG
  tft.setCursor(92, 0);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.print(welc);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_GREEN, ILI9341_NAVY);
  if (fastBoot == false) delay(500);
  tft.setCursor(0, 30);
  tft.print(F("SD card init"));
  for (byte o = 0; o < 20; o++) {
    tft.print(".");
    delay(25);
  }
  if (fastBoot == false) delay(500);

  while (!sdini) {
    String cardmsg = F("Insert FAT16/32 formatted card.");
    if (!SD.begin(SD_SS)) {                                   //(!SD.begin(SD_SS, SPI_FULL_SPEED));
      tft.setCursor(198, 30);
      tft.setTextColor(ILI9341_GREEN, ILI9341_RED);
      tft.println(F("Failed!"));
      tft.setTextColor(ILI9341_GREEN, ILI9341_NAVY);
      tft.println(cardmsg);
      Serial.println(cardmsg);
      digitalWrite(LED, !digitalRead(LED));                   //migat`!
      tone(PIEZO, 1000, 5);                                   //i pikat'!
      rgb.setPixelColor(0, rgb.Color(255, 0, 0));             //i vashe!
      rgb.show();
      delay(250);
      rgb.clear();
      rgb.show();
    }
    else {
      sdini = 1;
    }
  }

  if (fastBoot == false) delay(500);
  rgb.clear();
  rgb.show();
  rgb.setBrightness(10);

  tft.setCursor(198, 30);
  tft.println(F("OK!    "));
  if (fastBoot == false) delay(500);
  tft.print(F("SD ID"));                                    //Get sdcard UID
  for (byte o = 0; o < 27; o++) {
    tft.print(".");
    delay(25);
  }
  tft.print(" ");
  if (fastBoot == false) delay(500);
  cid_t cid;
  if (!SD.card()->readCID(&cid)) {
    tft.print(F("ERROR, "));
    myID = "0000";
  } else {
    myID = String((byte)(cid.psn >> 24) ^ (byte)(cid.psn >> 16), HEX) + String((byte)(cid.psn >> 8) ^ (byte)cid.psn, HEX);
    if (myID.length() > 4) myID = String((byte)(cid.psn >> 24) ^ (byte)(cid.psn >> 16), HEX);
    if (myID.length() == 2) myID = "00" + myID;
    if (myID.length() == 3) myID = "0" + myID;
    myID.toUpperCase();
  }
  if (fastBoot == false) delay(500);
  tft.println(myID);

  tft.print(F("DS3231 RTC"));
  for (byte o = 0; o < 21; o++) {
    tft.print(".");
    delay(25);
  }
  DS3231_init(DS3231_INTCN);                     //register the ds3231 (DS3231_INTCN is the default address of ds3231, this is set by macro for no performance loss)
  DS3231_get(&tiime);
  sprintf_P(s, PSTR("%02d.%02d.%02d  %02d:%02d:%02d"), tiime.mday, tiime.mon, tiime.year_s , tiime.hour, tiime.min, tiime.sec);
  tft.print(". ");
  tft.println(s);
  if (fastBoot == false) delay(200);

  tft.print(F("Log filename"));
  for (byte o = 0; o < 20; o++) {
    tft.print(".");
    delay(25);
  }
  tft.print(" ");
  sprintf_P(sdate, PSTR("%02d%02d%02d"), tiime.mday, tiime.mon, tiime.year);
  tft.println(sdate);
  if (fastBoot == false) delay(200);


  appendLogLine(String(s) + "  Hello " + myID + ". Starting LoRa messenger " + welc + ", Battery= " + bat + "v, t= " + DS3231_get_treg() + "C.");


  //////////////////////////////////////////////////////////////////////////////////configfile start/////////////////////////////////////////////////////////
  tft.print(F("Loading config file"));
  String calmsg = F(" Touch to format.");
  for (byte o = 0; o < 13; o++) {
    tft.print(".");
    delay(25);
  }
  tft.print(" ");
  tft.println(configFile);
  if (fastBoot == false) delay(500);

  if (SD.exists(configFile)) {                                                      //config present. applying.
    tft.println(F("Parsing configuration:"));                                       //parse
    if (fastBoot == false) delay(500);
    myFile = SD.open(configFile, FILE_READ);

    //tft.setTextSize(1);
    tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);
    int n;
    char lineT[11];
    const String errmsg = F("ERROR");

    tft.print(F(".........TS_MINX = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) TS_MINX = String(lineT).toInt();
    if ((TS_MINX < 1) or (TS_MINX > 1000)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);      //config.txt error flag ON
    else tft.println(TS_MINX);
    if (fastBoot == false) delay(200);

    tft.print(F(".........TS_MINY = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) TS_MINY = String(lineT).toInt();
    if ((TS_MINY < 1) or (TS_MINY > 1000)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);      //config.txt error flag ON
    else tft.println(TS_MINY);
    if (fastBoot == false) delay(200);

    tft.print(F(".........TS_MAXX = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) TS_MAXX = String(lineT).toInt();
    if ((TS_MAXX < 1000) or (TS_MAXX > 5000)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);   //config.txt error flag ON
    else tft.println(TS_MAXX);
    if (fastBoot == false) delay(200);

    tft.print(F(".........TS_MAXY = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) TS_MAXY = String(lineT).toInt();
    if ((TS_MAXY < 1000) or (TS_MAXY > 5000)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);   //config.txt error flag ON
    else tft.println(TS_MAXY);
    if (fastBoot == false) delay(200);

    tft.setTextColor(ILI9341_WHITE, ILI9341_NAVY);

    tft.print(F("........LoraFreq = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) Lora_Freq = String(lineT).toInt();
    if ((Lora_Freq < 410000000) or (Lora_Freq > 525000000)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_WHITE, ILI9341_NAVY);                     //config.txt error flag ON
    else tft.println(Lora_Freq);
    if (fastBoot == false) delay(200);

    tft.print(F(".........TxPower = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) Lora_TxPower = String(lineT).toInt();
    if ((Lora_TxPower < 1) or (Lora_TxPower > 20)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_WHITE, ILI9341_NAVY);                              //config.txt error flag ON
    else tft.println(Lora_TxPower);
    if (fastBoot == false) delay(200);

    tft.print(F(".SpreadingFactor = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) Lora_SpreadingFactor = String(lineT).toInt();
    if ((Lora_SpreadingFactor < 6) or (Lora_SpreadingFactor > 12)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_WHITE, ILI9341_NAVY);              //config.txt error flag ON
    else tft.println(Lora_SpreadingFactor);
    if (fastBoot == false) delay(200);

    tft.print(F(".SignalBandwidth = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) Lora_SignalBandwidth = String(lineT).toInt();
    if ((Lora_SignalBandwidth < 7800) or (Lora_SignalBandwidth > 500000)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_WHITE, ILI9341_NAVY);       //config.txt error flag ON
    else tft.println(Lora_SignalBandwidth);
    if (fastBoot == false) delay(200);

    tft.print(F(".....CodingRate4 = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) Lora_CodingRate4 = String(lineT).toInt();
    if ((Lora_CodingRate4 < 5) or (Lora_CodingRate4 > 8)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_WHITE, ILI9341_NAVY);                       //config.txt error flag ON
    else tft.print("4/"), tft.println(Lora_CodingRate4);
    if (fastBoot == false) delay(200);

    tft.print(F("........SyncWord = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) Lora_SyncWord = String(lineT).toInt();
    if ((Lora_SyncWord < 1) or (Lora_SyncWord > 245)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_WHITE, ILI9341_NAVY);                           //config.txt error flag ON
    else tft.println(Lora_SyncWord);
    if (fastBoot == false) delay(200);

    tft.print(F(".............CRC = "));
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) Lora_Crc = String(lineT).toInt();
    if ((Lora_Crc != 0) and (Lora_Crc != 1)) tft.setTextColor(ILI9341_GREEN, ILI9341_RED), tft.println(errmsg), configtxt = 1, tft.setTextColor(ILI9341_WHITE, ILI9341_NAVY);                                     //config.txt error flag ON
    else tft.println(Lora_Crc);
    if (fastBoot == false) delay(500);

    if (configtxt == 1) {
      tft.println();
      tft.print(F("Error in setup data :-E"));
      tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);
      tft.println(calmsg);                                     //"Touch to format."
      touchWait();
      reFormat();
    }
    myFile.close();                                            //close the file.
  } else {                                                     //esli net fila.
    tft.setTextColor(ILI9341_YELLOW, ILI9341_RED);
    tft.println();
    tft.print(F("Error opening config file."));
    tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);
    tft.println(calmsg);                                       //"Touch to format."
    touchWait();
    reFormat();
  }
  ////////////////////////////////////////////////////////////////////////////////configfile end/////////////////////////////////////////////////////////

  tft.setTextColor(ILI9341_GREEN, ILI9341_NAVY);

  tft.print(F("LoRa"));
  for (byte o = 0; o < 28; o++) {
    tft.print(".");
    delay(25);
  }

  LoRa.setPins(LORA_SS, LORA_RESET, LORA_IRQ);            //(ss, reset, dio0-interrupt)  //LoRa.setSPIFrequency(50000000);
  if (!LoRa.begin(Lora_Freq)) {                           //433e6
    tft.setTextColor(ILI9341_YELLOW, ILI9341_RED);
    tft.println(F("ERROR!"));
    delay(500);
    Serial.println(F("LoRa connection failed!"));
    while (1);
  }
  delay(100);
  tft.println(" OK!");

  tft.print(F("Battery voltage"));
  for (byte o = 0; o < 17; o++) {
    tft.print(".");
    delay(25);
  }
  tft.print(" ");
  tft.print(bat);
  tft.println("v");
  if (fastBoot == false) delay(200);



  tft.print(F("Applying LoRa settings"));
  for (byte o = 0; o < 10; o++) {
    tft.print(".");
    delay(25);
  }

  loraApply();

  tft.println(F(" OK!"));
  tft.println();

  tft.setTextColor(ILI9341_CYAN, ILI9341_NAVY);
  tft.setCursor(1, 231);
  tft.print(F("Touch to pause: "));
  bool torm = false;
  for (byte o = 5; o > 0; o--) {
    tft.print(o);
    tft.print(".");
    if (fastBoot == false) delay(1000);
    else delay(333);
    if (digitalRead(TOUCH_IRQ) == LOW) torm = true;
  }
  tft.print("0");
  delay(500);
  if (torm == true) tft.print(F(", Paused.")), touchWait();

  tft.setTextColor(ILI9341_GREEN, ILI9341_NAVY);
  tft.fillScreen(ILI9341_BLUE);
  LoRa.onReceive(onReceive);                                       //register the receive callback
  LoRa.receive();                                                  //put the radio into receive mode

  lightTimeout = 0;

  vseLinii2[0].id = "LoRa";  //predef text
  vseLinii2[0].text = " messenger:                    working...";
  vseLinii2[1].text = "     ENTER empty line [OK] to show menu.";
  vseLinii2[2].v1st = "abcdefgh";
  vseLinii2[2].v2nd = "12345678";
  vseLinii2[2].ttl = "z";
  vseLinii2[2].contr = "x";
  vseLinii2[2].id = "----";
  vseLinii2[2].text = "+***********************************************+";




  MakeKB_Button(Mobile_KB);                                        //finaly draw KB and all 10 msgs

}
