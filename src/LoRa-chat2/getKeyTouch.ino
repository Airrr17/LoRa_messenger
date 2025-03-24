void GetKeyPress(char * textBuffer) {
  char key = 0;
  static bool shift = false, special = false, back = false, lastSp = false, lastSh = false;
  static char bufIndex = 0;

  if (TouchButton(15, 160, 35, 25)) {   //ShiftKey
    shift = !shift;
  }

  if (TouchButton(15, 190, 35, 25)) {   //Special Characters
    special = !special;
  }

  if (special != lastSp || shift != lastSh) {
    if (special) {
      if (shift) {
        tft.fillScreen(ILI9341_BLUE);
        MakeKB_Button(Mobile_SymKeys);
      }
      else {
        tft.fillScreen(ILI9341_BLUE);
        MakeKB_Button(Mobile_NumKeys);
      }
    }
    else {
      tft.fillScreen(ILI9341_BLUE);
      MakeKB_Button(Mobile_KB);
      tft.setTextColor(0xffff, 0xf800);
    }

    if (special)
      tft.setTextColor(ILI9341_MAGENTA, 0xf800);
    else
      tft.setTextColor(0xFFFF, 0xf800);

    tft.setCursor(21, 195);
    tft.setTextSize(2);
    tft.print(F("SY"));

    if (shift)
      tft.setTextColor(ILI9341_MAGENTA, 0xf800);
    else
      tft.setTextColor(0xffff, 0xf800);

    tft.setCursor(27, 168);
    tft.print('^');
    tft.setTextSize(1);
    lastSp = special;
    lastSh = shift;
  }

  for (int y = 0; y < 3; y++) {
    int ShiftRight;
    if (special) {
      if (shift) ShiftRight = 15 * pgm_read_byte(&(Mobile_SymKeys[y][0]));
      else ShiftRight = 15 * pgm_read_byte(&(Mobile_NumKeys[y][0]));
    }
    else ShiftRight = 15 * pgm_read_byte(&(Mobile_KB[y][0]));

    for (int x = 3; x < 13; x++) {
      if (x >=  (special ? (shift ? pgm_read_byte(&(Mobile_SymKeys[y][1])) : pgm_read_byte(&(Mobile_NumKeys[y][1]))) : pgm_read_byte(&(Mobile_KB[y][1])) )) break;

      if (TouchButton(15 + (30 * (x - 3)) + ShiftRight, 100 + (30 * y), 20, 25)) {
        if (bufIndex < (textLimit)) {
          delay(100);
          if (special) {
            if (shift) textBuffer[bufIndex] = pgm_read_byte(&(Mobile_SymKeys[y][x]));
            else textBuffer[bufIndex] = pgm_read_byte(&(Mobile_NumKeys[y][x]));
          }
          else textBuffer[bufIndex] = (pgm_read_byte(&(Mobile_KB[y][x])) + (shift ? 0 : ('a' - 'A')));

          bufIndex++;
        }
        break;
      }
    }
  }

  //Spacebar
  if (TouchButton(60, 190, 200, 25)) {
    if (bufIndex < (textLimit)) textBuffer[bufIndex++] = ' ';
  }

  //BackSpace
  if (TouchButton(270, 160, 35, 25)) {
    if ((bufIndex) > 0)
      bufIndex--;
    textBuffer[bufIndex] = 0;
    tft.fillRect(1, 83, 318, 12, 0x03EF);                                                      //text input field 0x3186
    //tft.setTextSize(2); //def
  }

  //---------------------------------------------------------------------------------------------ENTER KEY------
  if (TouchButton(270, 190, 35, 25)) {
    if (bufIndex == 0) tone(PIEZO, 2000, 75), delay(75), showMenu();

    if (bufIndex > 0) {                                       //est' tam che vashe?

      loraTx(textBuffer);                                     //send and log

      while (bufIndex > 0) {                                  //erase buffer
        bufIndex--;
        textBuffer[bufIndex] = 0;
      }

      tft.fillRect(1, 83, 318, 12, 0x03EF);                   //text input field 0x3186
      //tft.setTextSize(2);
    }
  }

  tft.fillRect(1, 83, 318, 12, 0x03EF);                       //text input field 0x3186

  tft.setTextColor(ILI9341_GREENYELLOW, ILI9341_BLACK);
  tft.setCursor(305, 85);
  if ((textLimit - bufIndex) < 10) tft.print("0");
  tft.print(textLimit - bufIndex);                            //chars left counter


  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(3, 85);                                       //print ALL
  tft.print(textBuffer);
  //tft.setTextSize(2);

  tone(PIEZO, 1000, 3);                                       //tick
  digitalWrite(MOTOR, HIGH);
  delay(40);
  digitalWrite(MOTOR, LOW);

}
