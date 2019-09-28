#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "XPT2046_touch.h"           // Serasidis_XPT2046_touch library!!  https://github.com/rogerclarkmelbourne/Arduino_STM32
#include <avr/pgmspace.h>
#include <Wire.h>
#include <ds3231.h>                  // DS3231FS library!!   https://github.com/Jorropo/ds3231
#include "SdFat.h"

//869.525000 use a long 869525000 <-just example
//                      100000000  E6
//float snr = LoRa.packetSnr();
//LoRa.setTxPower(txPower);
//LoRa.setSpreadingFactor(spreadingFactor);   //6-12
//LoRa.setSignalBandwidth(signalBandwidth);   //7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3(def), 250E3
//LoRa.setCodingRate4(codingRateDenominator); //Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8.
//LoRa.setSyncWord(syncWord);                 //def 12
//LoRa.enableCrc(); //LoRa.disableCrc();
//byte b = LoRa.random();                     //Returns random byte.

#define IsWithin(x, a, b) ((x>=a)&&(x<=b))
#define TS_MINX 600  // 350   // 229       Adjust those manually!
#define TS_MINY 300  // 236   // 236
#define TS_MAXX 3800 // 3840  // 3840
#define TS_MAXY 3800 //       // 3903

//Colors:
//#define ILI9341_BLACK       0x0000  ///<   0,   0,   0  Builtin commands:
//#define ILI9341_NAVY        0x000F  ///<   0,   0, 123  calibON  - turn on touch debug
//#define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0  calibOFF - turn off touch debug
//#define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123
//#define ILI9341_MAROON      0x7800  ///< 123,   0,   0
//#define ILI9341_PURPLE      0x780F  ///< 123,   0, 123
//#define ILI9341_OLIVE       0x7BE0  ///< 123, 125,   0
//#define ILI9341_LIGHTGREY   0xC618  ///< 198, 195, 198
//#define ILI9341_DARKGREY    0x7BEF  ///< 123, 125, 123
//#define ILI9341_BLUE        0x001F  ///<   0,   0, 255
//#define ILI9341_GREEN       0x07E0  ///<   0, 255,   0
//#define ILI9341_CYAN        0x07FF  ///<   0, 255, 255
//#define ILI9341_RED         0xF800  ///< 255,   0,   0
//#define ILI9341_MAGENTA     0xF81F  ///< 255,   0, 255
//#define ILI9341_YELLOW      0xFFE0  ///< 255, 255,   0
//#define ILI9341_WHITE       0xFFFF  ///< 255, 255, 255
//#define ILI9341_ORANGE      0xFD20  ///< 255, 165,   0
//#define ILI9341_GREENYELLOW 0xAFE5  ///< 173, 255,  41
//#define ILI9341_PINK        0xFC18  ///< 255, 130, 198


const char Mobile_KB[3][13] PROGMEM = {
  {0, 13, 10, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
  {1, 12, 9, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'},
  {3, 10, 7, 'Z', 'X', 'C', 'V', 'B', 'N', 'M'},
};

const char Mobile_NumKeys[3][13] PROGMEM = {
  {0, 13, 10, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
  {0, 13, 10, '-', '/', ':', ';', '(', ')', '$', '&', '@', '"'},
  {5, 8, 5, '.', '\,', '?', '!', '\''}
};

const char Mobile_SymKeys[3][13] PROGMEM = {
  {0, 13, 10, '[', ']', '{', '}', '#', '%', '^', '*', '+', '='},
  {4, 9, 6, '_', '\\', '|', '~', '<', '>'}, //4
  {5, 8, 5, '.', '\,', '?', '!', '\''}
};
//======================================================================Vsyakaya hernya=====================================

bool command = false;                                  //command or message? exec or send?
bool calib = false;
const char textLimit = 53;                             //Max message len
char MyBuffer[textLimit];
//char line0[textLimit];
//char line1[textLimit];
//char line2[textLimit];
//char line3[textLimit];
//char line4[textLimit];
//char line5[textLimit];
//char line6[textLimit];
//char line7[textLimit];
//char line8[textLimit];
//char line9[textLimit];
uint16_t xy[2];
ts tiime;                                              //ts is a struct findable in ds3231.h
char s[17];                                            //RTCtime sprint buffer
char sdate[9];                                         //RTC date for filenaming

//char fileLine[100];                                  //Read a line from file

SPIClass mySPI(1);                                     //Create an SPI instance on SPI1 port.
XPT2046_touch tscr(PB11, mySPI);                       //Chip Select pin, SPI port
Adafruit_ILI9341 tft = Adafruit_ILI9341(PA4, PA1);     //TFT_SS, TFT_DC
SdFat SD;
File myFile;

void setup() {
  //  mySPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0)); //1000000

  Serial.begin(115200);
  delay(300);                                    //USB serial init time.
  Wire.begin();                                  //For RTC. SCL-PB6, SDA-PB7
  tft.begin();
  tft.fillScreen(ILI9341_BLUE);
  tft.setRotation(7); //7 or -1-

  pinMode(PB0, OUTPUT);                          //LoRa Reset pin
  pinMode(PB1, INPUT);                           //LoRa Interrupt pin
  pinMode(PB11, OUTPUT);                         //Touch slave select
  pinMode(PB10, INPUT);                          //Touch IRQ
  pinMode(PA4, OUTPUT);                          //TFT slave select
  pinMode(PA1, OUTPUT);                          //TFT DC
  pinMode(PC13, OUTPUT);                         //OnBoard LED
  pinMode(PA3, OUTPUT);                          //SD card slave select

  DS3231_init(DS3231_INTCN);                     //register the ds3231 (DS3231_INTCN is the default address of ds3231, this is set by macro for no performance loss)

  if (!SD.begin(PA3, SPI_HALF_SPEED)) {          //or (!SD.begin(PA3))
    tft.setTextSize(2);
    tft.setCursor(5, 5);
    tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
    tft.println(F("SDcard init. failed!"));
    tft.print(F("       Try reset."));
    delay(500);
    Serial.println("SDcard init. failed! Try reset.");
    while (1);
  }

  delay(200);

  LoRa.setPins(PA0, PB0, PB1);                   //(ss, reset, dio0-interrupt)
  //  LoRa.setSPIFrequency(2E6);
  if (!LoRa.begin(433000000)) {                  //433e6 Hz
    tft.setTextSize(2);
    tft.setCursor(5, 5);
    tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
    tft.print(F("LoRa connection failed!"));
    delay(500);
    Serial.println(F("LoRa connection failed!"));
    while (1);
  }


  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(11);       //6-12
  LoRa.setSignalBandwidth(125E3);    //7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3(def), 250E3
  LoRa.setCodingRate4(8);            //Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8.
  LoRa.enableCrc();                  //LoRa.disableCrc();


  tscr.begin();
  MakeKB_Button(Mobile_KB);

  //  LoRa.beginPacket();
  //  LoRa.print("hello AAA");
  //  LoRa.endPacket();

  DS3231_get(&tiime);    ///////////// dlya filename //////////////
  sprintf_P(s, PSTR("%02d.%02d.%02d  %02d:%02d:%02d"), tiime.mday, tiime.mon, tiime.year_s , tiime.hour, tiime.min, tiime.sec);
  sprintf_P(sdate, PSTR("%02d%02d%02d"), tiime.mday, tiime.mon, tiime.year);

  myFile = SD.open(sdate, FILE_WRITE);

  ////////////////////////////// READ LINE if file exist /////////////////////////////
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(1, 231);
  //    myFile.fgets(fileLine, sizeof(fileLine));
  //    tft.print(fileLine);
  //    myFile.close();
  tft.print("Using logfile: ");
  tft.print(sdate);
  ////////////////////////////// READ LINE if file exist /////////////////////////////
  //tft.setCursor(1, 1);
  //tft.println("12345678901234567890123456789012345678901234567890abc");
  //tft.println("2");
  //tft.println("3");
  //tft.println("4");
  //tft.println("5");
  //tft.println("6");
  //tft.println("7");
  //tft.println("8");
  //tft.println("9");
  //tft.println("10");

}




void loop() {

  gpio_write_bit(GPIOC, 13, !digitalRead(PC13));         // migat` stsuk!

  if (digitalRead(PB10) == 0) {                          // Touch IRQ
    TS_Point p = tscr.getPoint();
    if (p.z > 0) {
      GetKeyPress(MyBuffer);
    }
  }

  /////////////////////////////////////////////////// try to parse packet

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setCursor(1, 1);

    tft.print("Rx: ");
    myFile.print(s);
    myFile.print("  Rx: ");

    ///////////////////////////////// read packet
    while (LoRa.available()) {
      char rxsym = LoRa.read();
      tft.print(rxsym);
      myFile.print(rxsym);
    }

    // print RSSI of packet
    tft.print("' RSSI: ");
    myFile.print("   With RSSI: ");
    tft.print(LoRa.packetRssi());
    myFile.print(LoRa.packetRssi());
    tft.print(" SNR: ");
    myFile.print(" SNR: ");
    float snr = LoRa.packetSnr();
    tft.println(snr);
    myFile.println(snr);
    tft.setTextSize(2);
    myFile.flush();
  }




  if (millis() % 1000 == 0) {   ////////////// Update CLOCK
    DS3231_get(&tiime);
    sprintf_P(s, PSTR("%02d.%02d.%02d  %02d:%02d:%02d"), tiime.mday, tiime.mon, tiime.year_s , tiime.hour, tiime.min, tiime.sec);
    sprintf_P(sdate, PSTR("%02d%02d%02d"), tiime.mday, tiime.mon, tiime.year);
    tft.setTextSize(1);                               //Serial.println(s);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setCursor(211, 231);
    tft.print(s);
  }
}



//======================================================================Vsyakaya hernya=====================================
byte TouchButton(int x, int y, int w, int h)
{
  int X, Y;
  if (tscr.read_XY(xy)) {

    Y = map(xy[1], TS_MINY, TS_MAXY, 0, 240); //240
    X = map(xy[0], TS_MINX, TS_MAXX, 0, 320);  //320

    if (calib) tft.drawPixel( X, Y, ILI9341_YELLOW);  ////////////////////////////////////////////////////////////// for touching debug or calibrating

    return (IsWithin(X, x, x + w) & IsWithin(Y, y, y + h));
  }
}
//======================================================================Vsyakaya hernya=====================================
void MakeKB_Button(const char type[][13])
{
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE, 0xf000);
  for (int y = 0; y < 3; y++)
  {
    int ShiftRight = 15 * pgm_read_byte(&(type[y][0]));
    for (int x = 3; x < 13; x++)
    {
      if (x >= pgm_read_byte(&(type[y][1]))) break;

      drawButton(15 + (30 * (x - 3)) + ShiftRight, 100 + (30 * y), 20, 25); // this will draw the button on the screen by so many pixels
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

  tft.fillRect(1, 86, 318, 12, 0x03EF);          //text input field 0x3186
}
//======================================================================Vsyakaya hernya=====================================
void drawButton(int x, int y, int w, int h)
{
  tft.fillRoundRect(x - 3, y + 3, w, h, 3, 0x8888);                  //Button Shading
  tft.fillRoundRect(x, y, w, h, 3, 0xffff);                          // outter button color
  tft.fillRoundRect(x + 1, y + 1, w - 1 * 2, h - 1 * 2, 3, 0xf800);  //inner button color
}
//======================================================================Vsyakaya hernya=====================================
void GetKeyPress(char * textBuffer) {
  char key = 0;
  static bool shift = false, special = false, back = false, lastSp = false, lastSh = false;
  static char bufIndex = 0;

  //ShiftKey
  if (TouchButton(15, 160, 35, 25))    {
    shift = !shift;
    delay(100);
  }

  //Special Characters
  if (TouchButton(15, 190, 35, 25))    {
    special = !special;
    delay(100);
  }

  if (special != lastSp || shift != lastSh)    {
    if (special)      {
      if (shift)        {
        tft.fillScreen(ILI9341_BLUE);
        MakeKB_Button(Mobile_SymKeys);
      }
      else        {
        tft.fillScreen(ILI9341_BLUE);
        MakeKB_Button(Mobile_NumKeys);
      }
    }
    else      {
      tft.fillScreen(ILI9341_BLUE);
      MakeKB_Button(Mobile_KB);
      tft.setTextColor(0xffff, 0xf800);
    }

    if (special)
      tft.setTextColor(ILI9341_MAGENTA, 0xf800);
    else
      tft.setTextColor(0xFFFF, 0xf800);

    tft.setCursor(21, 195);
    tft.print(F("SY"));

    if (shift)
      tft.setTextColor(ILI9341_MAGENTA, 0xf800);
    else
      tft.setTextColor(0xffff, 0xf800);

    tft.setCursor(27, 168);
    tft.print('^');

    lastSp = special;
    lastSh = shift;
  }

  for (int y = 0; y < 3; y++)    {
    int ShiftRight;
    if (special)      {
      if (shift)
        ShiftRight = 15 * pgm_read_byte(&(Mobile_SymKeys[y][0]));
      else
        ShiftRight = 15 * pgm_read_byte(&(Mobile_NumKeys[y][0]));
    }
    else
      ShiftRight = 15 * pgm_read_byte(&(Mobile_KB[y][0]));

    for (int x = 3; x < 13; x++)
    {
      if (x >=  (special ? (shift ? pgm_read_byte(&(Mobile_SymKeys[y][1])) : pgm_read_byte(&(Mobile_NumKeys[y][1]))) : pgm_read_byte(&(Mobile_KB[y][1])) )) break;

      if (TouchButton(15 + (30 * (x - 3)) + ShiftRight, 100 + (30 * y), 20, 25)) // this will draw the button on the screen by so many pixels
      {
        if (bufIndex < (textLimit - 1))
        {
          delay(100);

          if (special)
          {
            if (shift)
              textBuffer[bufIndex] = pgm_read_byte(&(Mobile_SymKeys[y][x]));
            else
              textBuffer[bufIndex] = pgm_read_byte(&(Mobile_NumKeys[y][x]));
          }
          else
            textBuffer[bufIndex] = (pgm_read_byte(&(Mobile_KB[y][x])) + (shift ? 0 : ('a' - 'A')));

          bufIndex++;
        }
        break;
      }
    }
  }

  //Spacebar
  if (TouchButton(60, 190, 200, 25))    {
    textBuffer[bufIndex++] = ' ';
    delay(100);
  }

  //BackSpace
  if (TouchButton(270, 160, 35, 25))  {
    if ((bufIndex) > 0)
      bufIndex--;
    textBuffer[bufIndex] = 0;
    tft.fillRect(1, 86, 318, 12, 0x03EF);            //text input field 0x3186
    delay(100);
    tft.setTextSize(2); //def
  }

  //------ENTER------
  if (TouchButton(270, 190, 35, 25))  {
    if (bufIndex > 0) {                              //est' tam che vashe?

      if (strcmp(textBuffer, "calibON") == 0) calib = true, command = true;
      if (strcmp(textBuffer, "calibOFF") == 0) calib = false, command = true;

      if (!command) {
        Serial.print(s);
        Serial.print("  Tx: ");
        Serial.println(textBuffer); //or      //  Serial.println(MyBuffer);       //   <---- tut main sending!
        myFile.print(s);
        myFile.print("  Tx: ");
        myFile.println(textBuffer);
        myFile.flush();
      }

      command = false;
      delay(50);
      while (bufIndex > 0)
      {
        bufIndex--;
        textBuffer[bufIndex] = 0;
      }

      tft.fillRect(1, 86, 318, 12, 0x03EF);          //text input field 0x3186
      tft.setTextSize(2);
    }
  }

  tft.setTextSize(1);                                //pishem che natikali na klave
  tft.fillRect(1, 86, 318, 12, 0x03EF);              //text input field 0x3186
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(4, 88);
  tft.print(textBuffer);
  tft.setTextSize(2);
}
//======================================================================Vsyakaya hernya=====================================
