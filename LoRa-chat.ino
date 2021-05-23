#include <SPI.h>                     //Using library SPI at version 1.0
#include <LoRa.h>                    //Using library LoRa at version 0.8.0
#include <Adafruit_GFX.h>            //Using library Adafruit_GFX_Library at version 1.7.5
#include <Adafruit_ILI9341.h>        //Using library Adafruit_ILI9341 at version 1.5.6
#include "XPT2046_touch.h"           //Serasidis_XPT2046_touch library!!  https://github.com/rogerclarkmelbourne/Arduino_STM32/tree/master/STM32F1/libraries/Serasidis_XPT2046_touch     Using library Serasidis_XPT2046_touch at version 1.0
#include <avr/pgmspace.h>
#include <Wire.h>                    //Using library Wire at version 1.0
#include <ds3231.h>                  //DS3231FS library!!   https://github.com/Jorropo/ds3231    Using library ds3231FS at version 1.1.1
#include "SdFat.h"                   //Using library SdFat at version 1.1.4

//byte b = LoRa.random();                     //Returns random byte.
//RA-02 module: The SX1278 supports frequency 137-525MHz

#define IsWithin(x, a, b) ((x>=a)&&(x<=b))

//Colors:
//#define ILI9341_BLACK       0x0000  ///<   0,   0,   0  Builtin commands:
//#define ILI9341_NAVY        0x000F  ///<   0,   0, 123  calibON  - turn on touch debug
//#define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0  calibOFF - turn off touch debug
//#define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123  resetconfig - delete config.txt
//#define ILI9341_MAROON      0x7800  ///< 123,   0,   0  TssmmhhWDDMMYYYY - aka set time (T355720619112011)
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
bool calib = false;                                    //Yellow touch calibration marks
const char textLimit = 53;                             //Max message len
unsigned int lightTimeout = 0;
char MyBuffer[textLimit];
uint16_t xy[2];
ts tiime;                                              //ts is a struct findable in ds3231.h
char s[19];                                            //RTCtime sprint buffer
char sdate[10];                                        //RTC date for filenaming
bool sdini = 0;
bool configtxt = 0;                                    //config file error parsing flag
int TS_MINX, TS_MINY, TS_MAXX, TS_MAXY;
long LoraFreq;             //137-525
int TxPower;               //1-20
int SpreadingFactor;       //6-12
long SignalBandwidth;      //7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3
int CodingRate4;           //Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8.
int SyncWord;              //def 0x12=18dec hz: (0xF3);  // ranges from 0-0xFF, default 0x34, see API
bool Crc;

String liniya;

char lines0[55];
char lines1[55];
char lines2[55];
char lines3[55];
char lines4[55];
char lines5[55];
char lines6[55];
char lines7[55];
char lines8[55];
char lines9[55];


SPIClass mySPI(1);                                     //Create an SPI instance on SPI1 port.
XPT2046_touch tscr(PB11, mySPI);                       //Chip Select pin, SPI port
Adafruit_ILI9341 tft = Adafruit_ILI9341(PA4, PA1);     //TFT_SS, TFT_DC
SdFat SD;
File myFile;

void setup() {
  mySPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0)); //2000000

  Serial.begin(115200);
  delay(750);                                    //USB serial init time.
  Serial.println(F("Unit starting."));
  Serial.println(F("Airrr v1.1"));

  pinMode(PB0, OUTPUT);                          //LoRa Reset pin
  pinMode(PB1, INPUT);                           //LoRa Interrupt pin
  pinMode(PB11, OUTPUT);                         //Touch slave select
  pinMode(PB10, INPUT);                          //Touch IRQ
  pinMode(PA4, OUTPUT);                          //TFT slave select
  pinMode(PA1, OUTPUT);                          //TFT DC
  pinMode(PA2, OUTPUT);                          //TFT LED
  pinMode(PC13, OUTPUT);                         //OnBoard LED
  pinMode(PA3, OUTPUT);                          //SD card slave select

  gpio_write_bit(GPIOB, 11, 1);
  gpio_write_bit(GPIOA, 4, 1);
  gpio_write_bit(GPIOA, 3, 1);
  gpio_write_bit(GPIOA, 2, 1);                   //LED on

  Wire.begin();                                  //For RTC. SCL-PB6, SDA-PB7
  tft.begin();
  tft.fillScreen(ILI9341_BLUE);
  tft.setRotation(7);                            //7 or -1-
  Serial.println(F("TFT"));
  DS3231_init(DS3231_INTCN);                     //register the ds3231 (DS3231_INTCN is the default address of ds3231, this is set by macro for no performance loss)

  delay (50);
  tft.setTextSize(2);
  tft.setCursor(200, 0);
  tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
  tft.print(F("Airrr v1.1"));
  tft.setCursor(0, 0);
  tft.println(F("Unit starting."));
  tft.println(F("SD card init..."));

  while (!sdini) {
    if (!SD.begin(PA3)) {                        //(!SD.begin(PA3, SPI_FULL_SPEED));
      tft.setCursor(0, 32);
      tft.println(F("SDcard init.: failed!"));
      tft.println(F("(re)insert card now."));
      Serial.println(F("SDcard init. failed! Retrying."));
      gpio_write_bit(GPIOC, 13, !digitalRead(PC13));         // migat`!
      delay (100);
    }
    else {
      sdini = 1;
    }
  }

  delay(50);
  tscr.begin();
  //////////////////////////////////////////////////////////////////////////////////config.txt start/////////////////////////////////////////////////////////

  tft.println(F("SD card init.: OK"));
  tft.println(F("Loading config.txt"));
  if (SD.exists("config.txt")) {                                                    /// config present. applying.
    tft.println(F("Parsing config.txt"));                                           /// parse
    myFile = SD.open("config.txt", FILE_READ);

    tft.setTextSize(1);
    tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
    int n;
    char lineT[11];

    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) TS_MINX = String(lineT).toInt();
    if ((TS_MINX < 1) or (TS_MINX > 1000)) tft.println(F("TS_MINX value error.")), configtxt = 1;      //config.txt error flag ON
    if (configtxt == 0) tft.print("TS_MINX = "), tft.println(TS_MINX);
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) TS_MINY = String(lineT).toInt();
    if ((TS_MINY < 1) or (TS_MINY > 1000)) tft.println(F("TS_MINY value error.")), configtxt = 1;      //config.txt error flag ON
    if (configtxt == 0) tft.print("TS_MINY = "), tft.println(TS_MINY);
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) TS_MAXX = String(lineT).toInt();
    if ((TS_MAXX < 1000) or (TS_MAXX > 5000)) tft.println(F("TS_MAXX value error.")), configtxt = 1;   //config.txt error flag ON
    if (configtxt == 0) tft.print("TS_MAXX = "), tft.println(TS_MAXX);
    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) TS_MAXY = String(lineT).toInt();
    if ((TS_MAXY < 1000) or (TS_MAXY > 5000)) tft.println(F("TS_MAXY value error.")), configtxt = 1;   //config.txt error flag ON
    if (configtxt == 0) tft.print("TS_MAXY = "), tft.println(TS_MAXY);

    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) LoraFreq = String(lineT).toInt();
    if ((LoraFreq < 137000000) or (LoraFreq > 525000000)) tft.println(F("LoraFreq value error.")), configtxt = 1;                    //config.txt error flag ON
    if (configtxt == 0) tft.print("LoraFreq = "), tft.println(LoraFreq);

    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) TxPower = String(lineT).toInt();
    if ((TxPower < 1) or (TxPower > 20)) tft.println(F("TxPower value error.")), configtxt = 1;                                      //config.txt error flag ON
    if (configtxt == 0) tft.print("TxPower = "), tft.println(TxPower);

    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) SpreadingFactor = String(lineT).toInt();
    if ((SpreadingFactor < 6) or (SpreadingFactor > 12)) tft.println(F("SpreadingFactor value error.")), configtxt = 1;              //config.txt error flag ON
    if (configtxt == 0) tft.print("SpreadingFactor = "), tft.println(SpreadingFactor);

    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) SignalBandwidth = String(lineT).toInt();
    if ((SignalBandwidth < 78000) or (SignalBandwidth > 500000)) tft.println(F("SignalBandwidth value error.")), configtxt = 1;      //config.txt error flag ON
    if (configtxt == 0) tft.print("SignalBandwidth = "), tft.println(SignalBandwidth);

    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) CodingRate4 = String(lineT).toInt();
    if ((CodingRate4 < 5) or (CodingRate4 > 8)) tft.println(F("CodingRate4 value error.")), configtxt = 1;                           //config.txt error flag ON
    if (configtxt == 0) tft.print("CodingRate4 = "), tft.println(CodingRate4);

    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) SyncWord = String(lineT).toInt();
    if ((SyncWord < 1) or (SyncWord > 245)) tft.println(F("SyncWord value error.")), configtxt = 1;                                  //config.txt error flag ON
    if (configtxt == 0) tft.print("SyncWord = "), tft.println(SyncWord);

    if ((n = myFile.fgets(lineT, sizeof(lineT))) > 0) Crc = String(lineT).toInt();
    if ((Crc < 0) or (Crc > 1)) tft.println(F("CRC value error.")), configtxt = 1;                                                   //config.txt error flag ON
    if (configtxt == 0) tft.print("Crc = "), tft.println(Crc);

    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
    if (configtxt == 0) tft.println(F("OK"));
    delay(3000);

    if (configtxt == 1) {
      tft.println(F("Error in setup data."));
      tft.println(F("Calibrate touch now."));
      delay (5000);
      touchCalibrate();
    }
    myFile.close();                                            // close the file.
  } else {                                                     // esli net fila.
    tft.println(F("Error opening config.txt"));
    tft.println(F("Calibrate touch now."));
    delay(5000);
    touchCalibrate();
  }

  //////////////////////////////////////////////////////////////////////////////////config.txt end/////////////////////////////////////////////////////////
  tft.fillScreen(ILI9341_BLUE);

  LoRa.setPins(PA0, PB0, PB1);                   //(ss, reset, dio0-interrupt)
  //LoRa.setSPIFrequency(50000000);
  if (!LoRa.begin(LoraFreq)) {                   //433e6
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
    tft.print(F("LoRa connection failed!"));
    delay(500);
    Serial.println(F("LoRa connection failed!"));
    while (1);
  }
  delay(100);

  LoRa.setTxPower(TxPower);                    //1-20
  LoRa.setSpreadingFactor(SpreadingFactor);    //6-12
  LoRa.setSignalBandwidth(SignalBandwidth);    //7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3
  LoRa.setCodingRate4(CodingRate4);            //Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8.
  LoRa.setSyncWord(SyncWord);                  //def 0x12=18dec hz: (0xF3);  // ranges from 0-0xFF, default 0x34, see API
  if (Crc == true) LoRa.enableCrc();           //LoRa Crc
  if (Crc == false) LoRa.disableCrc();


  MakeKB_Button(Mobile_KB);

  DS3231_get(&tiime);    ///////////// dlya filename //////////////
  sprintf_P(s, PSTR("%02d.%02d.%02d  %02d:%02d:%02d"), tiime.mday, tiime.mon, tiime.year_s , tiime.hour, tiime.min, tiime.sec);
  sprintf_P(sdate, PSTR("%02d%02d%02d"), tiime.mday, tiime.mon, tiime.year);


  //////////////////////////////  logfile open  /////////////////////////////
  myFile = SD.open(sdate, FILE_WRITE);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(1, 231);
  tft.print("Using logfile: ");
  tft.print(sdate);
  //////////////////////////////  logfile open   /////////////////////////////
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
    gpio_write_bit(GPIOA, 2, 1), lightTimeout = 0;                   //LED on
    myFile.print(s);
    myFile.print("  Rx: ");

    ///////////////////////////////// read packet
    while (LoRa.available()) {
      liniya = LoRa.readString();
      myFile.print(liniya);

    }

    myFile.print(" >>> RSSI: ");
    myFile.print(LoRa.packetRssi());
    myFile.print(" SNR: ");
    float snr = LoRa.packetSnr();
    myFile.print(snr);
    myFile.print(" freqErr: ");
    long freqErr = LoRa.packetFrequencyError();
    myFile.println(freqErr);
    myFile.flush();

    liniya = 'R' + liniya;
    moveUP();
  }


  if (millis() % 1000 == 0) {   ////////////// Update CLOCK
    DS3231_get(&tiime);
    sprintf_P(s, PSTR("%02d.%02d.%02d  %02d:%02d:%02d"), tiime.mday, tiime.mon, tiime.year_s , tiime.hour, tiime.min, tiime.sec);
    sprintf_P(sdate, PSTR("%02d%02d%02d"), tiime.mday, tiime.mon, tiime.year);
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setCursor(211, 231);
    tft.print(s);
    lightTimeout ++;
    if (lightTimeout >= 30) gpio_write_bit(GPIOA, 2, 0), lightTimeout = 30;                   //LED off
  }
}



//======================================================================Vsyakaya hernya=====================================
byte TouchButton(int x, int y, int w, int h)
{
  int X, Y;
  if (tscr.read_XY(xy)) {

    Y = map(xy[1], TS_MINY, TS_MAXY, 240, 0);  //TS_MINY, TS_MAXY, 0, 240 //need to mirror?
    X = map(xy[0], TS_MINX, TS_MAXX, 0, 320);  //TS_MINX, TS_MAXX, 0, 320

    if (calib) tft.drawPixel( X, Y, ILI9341_YELLOW);  ////////////////////////////////////////////////////////////// for touching debug or calibrating
    gpio_write_bit(GPIOA, 2, 1), lightTimeout = 0;                   //LED on

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
  pechat();
}
//======================================================================Vsyakaya hernya=====================================
void drawButton(int x, int y, int w, int h)
{
  tft.fillRoundRect(x - 3, y + 3, w, h, 3, 0x8888);                  //Button Shading
  tft.fillRoundRect(x, y, w, h, 3, 0xffff);                          //outter button color
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

      if (strcmp(textBuffer, "calibON") == 0) calib = true, command = true;                                      ///CALIBRATION////////////////////////////////////////////////
      if (strcmp(textBuffer, "calibOFF") == 0) calib = false, command = true;
      if ((strcmp(textBuffer, "resetconfig") == 0) and (SD.exists("config.txt"))) {
        tft.setCursor(0, 0);
        tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
        tft.setTextSize(2);
        tft.fillScreen(ILI9341_BLUE);
        tft.println(F("Deleting config.txt file."));
        SD.remove("config.txt");
        tft.println(F("Done. Reset manually."));
        delay(5000000);
        command = true;
      }
      if ((textBuffer[0]  == 84) and (strlen(textBuffer) == 16)) {            //84="T"                          ////////////////////////////////Set RTC////////////////////////////// TssmmhhWDDMMYYYY
        textBuffer[0] = 0;
        setVremya(textBuffer);
        command = true;
      }

      if (!command) {
        //        Serial.print(s);
        //        Serial.print("  Tx: ");
        //        Serial.println(textBuffer); //or      //  Serial.println(MyBuffer);       //   <---- tut main sending!
        myFile.print(s);
        myFile.print("  Tx: ");
        myFile.println(textBuffer);
        myFile.flush();

        LoRa.beginPacket();      /////////////////// UNCOMENT FOR REAL SENDING!!!//////////////////////
        LoRa.print(textBuffer);
        LoRa.endPacket();

        liniya = String(textBuffer);
        liniya = 'T' + liniya;
        moveUP();
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

void moveUP() {

  memcpy(lines0, lines1, sizeof lines1); //Ugly code goes from here and on
  memcpy(lines1, lines2, sizeof lines2);
  memcpy(lines2, lines3, sizeof lines3);
  memcpy(lines3, lines4, sizeof lines4);
  memcpy(lines4, lines5, sizeof lines5);
  memcpy(lines5, lines6, sizeof lines6);
  memcpy(lines6, lines7, sizeof lines7);
  memcpy(lines7, lines8, sizeof lines8);
  memcpy(lines8, lines9, sizeof lines9);

  liniya.toCharArray(lines9, liniya.length() + 1); //OK dlya 1D bez [x][x]
  pechat();
}


void pechat() {

  tft.fillRect(0, 0, 320, 81, ILI9341_BLUE);////////// still have a lot of memory :)))
  tft.setTextSize(1);

  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  if ((lines0[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines0[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 1);
  tft.print(lines0);

  if ((lines1[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines2[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 9);
  tft.print(lines1);

  if ((lines2[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines2[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 17);
  tft.print(lines2);

  if ((lines3[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines3[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 25);
  tft.print(lines3);

  if ((lines4[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines4[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 33);
  tft.print(lines4);

  if ((lines5[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines5[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 41);
  tft.print(lines5);

  if ((lines6[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines6[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 49);
  tft.print(lines6);

  if ((lines7[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines7[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 57);
  tft.print(lines7);

  if ((lines8[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines8[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 65);
  tft.print(lines8);

  if ((lines9[0]) == 'T') tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  if ((lines9[0]) == 'R') tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setCursor(-5, 73);
  tft.print(lines9);

  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(1, 231);
  tft.print("Using logfile: ");
  tft.print(sdate);
  tft.setTextSize(2);
}

void touchCalibrate() {
  int toTouch = 150;
  int minimalX = 1500;
  int minimalY = 1500;
  int maximalX = 1500;
  int maximalY = 1500;
  tft.setTextSize(1);
  tft.fillScreen(ILI9341_BLUE);
  tft.setCursor(0, 0);
  tft.println(F("Setup initialized."));
  if (myFile.isOpen()) tft.println(F("Closing config.txt file.")), myFile.close();
  if (SD.exists("config.txt")) tft.println(F("Deleting config.txt file.")), SD.remove("config.txt");
  tft.println(F("Move stylus around all corners of the screen"));
  tft.println(F("adjusting four MIN/MAX values"));
  tft.println(F("untill touch counter lowered to 0."));

  while (toTouch > 0) {
    if (tscr.read_XY(xy)) {
      toTouch--;
      tft.setCursor(170, 232);
      tft.print(F("Points left to touch:    "));
      tft.setCursor(302, 232);
      tft.print(toTouch);

      tft.setCursor(100, 80);
      tft.print(F("Current X =     "));
      tft.setCursor(172, 80);
      tft.print(xy[0]);
      tft.setCursor(100, 90);
      tft.print(F("Current Y =     "));
      tft.setCursor(172, 90);
      tft.print(xy[1]);

      if (minimalX > xy[0]) minimalX = xy[0];
      if (minimalY > xy[1]) minimalY = xy[1];
      if (maximalX < xy[0]) maximalX = xy[0];
      if (maximalY < xy[1]) maximalY = xy[1];

      tft.setCursor(100, 100);
      tft.print(F("TS_MINX   =     "));
      tft.setCursor(172, 100);
      tft.print(minimalX);
      tft.setCursor(100, 110);
      tft.print(F("TS_MINY   =     "));
      tft.setCursor(172, 110);
      tft.print(minimalY);
      tft.setCursor(100, 120);
      tft.print(F("TS_MAXX   =     "));
      tft.setCursor(172, 120);
      tft.print(maximalX);
      tft.setCursor(100, 130);
      tft.print(F("TS_MAXY   =     "));
      tft.setCursor(172, 130);
      tft.print(maximalY);
      delay (20);
    }
  }
  TS_MINX = minimalX, TS_MINY = minimalY, TS_MAXX = maximalX, TS_MAXY = maximalY;
  tft.setCursor(0, 170);
  tft.println(F("All right, creating config.txt file with those"));
  tft.println(F("calibration setting and DEFAULT LORA settings."));
  myFile = SD.open("config.txt", FILE_WRITE);
  myFile.println(TS_MINX);
  myFile.println(TS_MINY);
  myFile.println(TS_MAXX);
  myFile.println(TS_MAXY);
  myFile.println("433000000");
  myFile.println("20");
  myFile.println("11");
  myFile.println("125000");
  myFile.println("8");
  myFile.println("18");
  myFile.println("1");
  myFile.close();

  tft.println(F("Done."));
  tft.println(F("Now you can edit config file to adjust LORA settings."));
  tft.println(F("Please don't touch first four lines in file."));
  tft.println(F("To set time use TssmmhhWDDMMYYYY structure and send."));

  delay(16000);
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextSize(2);
}

void setVremya(char * textBuffer) {                                // SET TIME and date and week
  Serial.println("time? set!");                                    // TssmmhhWDDMMYYYY - aka set time (T003201123052021)
  Serial.println(textBuffer);
  struct ts t;
  t.sec = inp2toi(textBuffer, 1);
  t.min = inp2toi(textBuffer, 3);
  t.hour = inp2toi(textBuffer, 5);
  t.wday = textBuffer[7] - 48;
  t.mday = inp2toi(textBuffer, 8);
  t.mon = inp2toi(textBuffer, 10);
  t.year = inp2toi(textBuffer, 12) * 100 + inp2toi(textBuffer, 14);
  DS3231_set(t);
  Serial.println("Time set OK");

  tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
  tft.setCursor(211, 231);
  tft.print("Set successfully!!!");
  delay (4000);
}
