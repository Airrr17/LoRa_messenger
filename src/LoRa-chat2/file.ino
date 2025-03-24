void save_config() {                                       //Save config!
  if (myFile.isOpen()) myFile.flush(), myFile.close();
  if (SD.exists(configFile)) SD.remove(configFile);
  delay(100);
  myFile = SD.open(configFile, FILE_WRITE);
  myFile.println(TS_MINX);
  myFile.println(TS_MINY);
  myFile.println(TS_MAXX);
  myFile.println(TS_MAXY);
  myFile.println(Lora_Freq);
  myFile.println(Lora_TxPower);
  myFile.println(Lora_SpreadingFactor);
  myFile.println(Lora_SignalBandwidth);
  myFile.println(Lora_CodingRate4);
  myFile.println(Lora_SyncWord);
  myFile.println(Lora_Crc);
  myFile.flush(), myFile.close();
  delay(100);
}





void appendLogLine(String toLog) {                                        //vrode ne slozhno
  if (myFile.isOpen()) myFile.close();                                    //close any file
  delay(10);
  myFile = SD.open(sdate, FILE_WRITE);                                    //logfile! open
  myFile.println(toLog);
  myFile.flush(), myFile.close();
  Serial.println(toLog);

}





void reFormat() {
  rgb.setBrightness(50);
  rgb.setPixelColor(0, rgb.Color(100, 0, 0));
  rgb.show();
  int minimalX = 1500;
  int minimalY = 1500;
  int maximalX = 1500;
  int maximalY = 1500;
  Lora_Freq = default_Lora_Freq;                           //Defaults
  Lora_TxPower = default_Lora_TxPower;
  Lora_SpreadingFactor = default_Lora_SpreadingFactor;
  Lora_SignalBandwidth = default_Lora_SignalBandwidth;
  Lora_CodingRate4 = default_Lora_CodingRate4;
  Lora_SyncWord = default_Lora_SyncWord;
  Lora_Crc = default_Lora_Crc;
  //tft.setTextSize(1);
  tft.fillScreen(ILI9341_NAVY);
  tft.setCursor(0, 3);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_RED, ILI9341_NAVY);
  tft.println(F("...Formatting device..."));
  tft.println();
  tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);
  tft.println(F("Move the stylus around all corners of the screen,"));
  tft.println(F("Adjusting four MIN/MAX values."));
  tft.print(F("Repeat until touch counter ("));
  tft.print(toTouch);
  tft.println(F(") lowered to 0."));
  tft.println();
  tft.println(F("Touch to start."));

  tft.setTextColor(ILI9341_GREEN, ILI9341_NAVY);
  while (toTouch > 0) {
    if (tft.getTouchRawZ() > touchForce) {                //Touch threshold
      tft.getTouchRaw(&x_touch, &y_touch);
      toTouch--;
      tft.setCursor(302, 232);
      tft.print("    ");
      tft.setCursor(302, 232);
      tft.print(toTouch);

      tft.setCursor(100, 60);
      tft.print(F("Current X =     "));
      tft.setCursor(172, 60);
      tft.print(x_touch);
      tft.setCursor(100, 70);
      tft.print(F("Current Y =     "));
      tft.setCursor(172, 70);
      tft.print(y_touch);

      if (minimalX > x_touch) minimalX = x_touch;
      if (minimalY > y_touch) minimalY = y_touch;
      if (maximalX < x_touch) maximalX = x_touch;
      if (maximalY < y_touch) maximalY = y_touch;

      tft.setCursor(100, 80);
      tft.print(F("TS_minX   =     "));
      tft.setCursor(172, 80);
      tft.print(minimalX);
      tft.setCursor(100, 90);
      tft.print(F("TS_minY   =     "));
      tft.setCursor(172, 90);
      tft.print(minimalY);
      tft.setCursor(100, 100);
      tft.print(F("TS_maxX   =     "));
      tft.setCursor(172, 100);
      tft.print(maximalX);
      tft.setCursor(100, 110);
      tft.print(F("TS_maxY   =     "));
      tft.setCursor(172, 110);
      tft.print(maximalY);
      delay (20);
    }
  }

  tft.setTextColor(ILI9341_YELLOW, ILI9341_NAVY);
  tft.setCursor(302, 232), tft.print("    ");                                        //steret' 0
  TS_MINX = minimalX, TS_MINY = minimalY, TS_MAXX = maximalX, TS_MAXY = maximalY;

  tft.setCursor(0, 130);
  tft.println(F("Format OK, applying default settings."));
  if (fastBoot == false) delay(500);

  save_config();

  tft.print(F("Saved '"));
  tft.print(configFile);
  tft.println(F("' file with all default settings."));
  tft.println();
  tft.print(F("OK! "));
  tft.setTextColor(ILI9341_RED, ILI9341_NAVY);
  tft.println(F("  ... Press reset ..."));
  delay(500);
  tone(PIEZO, 2500, 2000);
  rgb.setBrightness(50);                             //255
  while (1) {
    rgb.setPixelColor(0, rgb.Color(0, 0, random(255)));
    rgb.show();
    delay(10);
  }






}
