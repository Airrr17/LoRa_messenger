//LORA-messenger V2.0 for hardware V0.9                    (c)Airrr(r) Mar 2025.       (STM32F103CBT7)
//Using https://github.com/Serasidis/STM32_HID_Bootloader
//Using Board stm32 v2.6.0(!): https://github.com/Bodmer/TFT_eSPI/issues/3046//  >---------------->------------------>------------------>----------------> STM32 boards groups > Generic STM32F1 series > Generic F103CBTx > (USB) CDC generic serial > HID Bootloader
//
//User_Setup.h content for tft_eSPI:
//#define USER_SETUP_INFO "User_Setup"
//#define STM32
//#define ILI9341_DRIVER
//#define TOUCH_CS PB11
//#define TFT_MOSI PA7
//#define TFT_MISO PA6
//#define TFT_SCLK PA5
//#define TFT_CS   PA4
//#define TFT_DC   PA1
//#define TFT_RST  -1
//#define LOAD_GLCD
//#define SPI_FREQUENCY  55000000
//#define SPI_READ_FREQUENCY  20000000
//#define SPI_TOUCH_FREQUENCY  2500000
//



/////////////////////// Libs: //////////////////////////////////////////////
#include <avr/pgmspace.h>
#include <SPI.h>                     //Using library SPI at version 1.0
#include <LoRa.h>                    //Using library LoRa at version 0.8.0
#include <TFT_eSPI.h>                //Using library TFT_eSPI at version 2.5.0
#include <Wire.h>                    //Using library Wire at version 1.0
#include <ds3231.h>                  //Using library ds3231FS at version 1.1.1   DS3231FS library, https://github.com/Jorropo/ds3231 (404)
#include "SdFat.h"                   //Using library SdFat at version 1.1.4
#include <Adafruit_NeoPixel.h>       //Using library Adafruit_NeoPixel at version 1.8.0
#include "ClickButton.h"             //Using library ClickButton at version 1.1.0

#define IsWithin(x, a, b) ((x>=a)&&(x<=b))

/////////////////////// Pins: //////////////////////////////////////////////
#define LORA_RESET PB0        //LoRa Reset pin
#define LORA_IRQ   PB9        //LoRa Interrupt pin
#define LORA_SS    PA0        //LoRa select
#define TOUCH_SS   PB11       //Touch slave select
#define TOUCH_IRQ  PB10       //Touch IRQ
#define TFT_SS     PA4        //TFT slave select
#define TFT_DC     PA1        //TFT DC
#define TFT_LED    PA2        //TFT LED
#define LED        PC13       //OnBoard LED
#define SD_SS      PA3        //SD card slave select
#define BAT_LVL    PB1        //BAT.LVL
#define RGB_LED    PB8        //RGBled
#define PIEZO      PB15       //Piezo
#define MOTOR      PA15       //Motor
#define BUTTON     PB12       //Button

/////////////////////// Vars: //////////////////////////////////////////////
#define default_Lora_Freq                   433000000    //410M-525M //RA-02 module: The SX1278 supports frequency 410-525MHz
#define default_Lora_TxPower                20           //1-20
#define default_Lora_SpreadingFactor        11           //6-12
#define default_Lora_SignalBandwidth        125000       //7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3
#define default_Lora_CodingRate4            8            //Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8. (4/5 4/6 4/7 4/8)
#define default_Lora_SyncWord               18           //def 0x12=18dec hz: (0xF3);  // ranges from 0-0xFF, default 0x34, see API
#define default_Lora_Crc                    1            //yes or no?

#define maxF                                525000000
#define minF                                410000000

#define touchForce                          1000         //about 800-1500 for my tft

#define TRANSMIT                            true         //do a real tx? for testing

#define fastBoot                            false        //Less attractive boot while true


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

const char* DAYS[7] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
int Band[10] = {7800, 10400, 15600, 20800, 31250, 41700, 62500, 125000, 250000, 500000};

const String welc = F("(c)Airrr(r) fw:2.00");                //Welcome string.

//Lora vars:
long Lora_Freq = default_Lora_Freq;
int Lora_TxPower = default_Lora_TxPower;
int Lora_SpreadingFactor = default_Lora_SpreadingFactor;
int Lora_SignalBandwidth = default_Lora_SignalBandwidth;
int Lora_CodingRate4 = default_Lora_CodingRate4;
int Lora_SyncWord = default_Lora_SyncWord;
bool Lora_Crc = default_Lora_Crc;
//LoRa.setGain(gain); Supported values are between 0 and 6. If gain is 0, AGC will be enabled and LNA gain will not be used. Else if gain is from 1 to 6, AGC will be disabled and LNA gain will be used.
//byte b = LoRa.random();            //Returns random byte.

String myID = "0000";                                        //temp, SD ID
int TS_MINX, TS_MINY, TS_MAXX, TS_MAXY;                      //touch
const char* configFile = "config.txt";                       //all settings filename
bool sdini = 0;                                              //used at sd card initialization
float bat;                                                   //analog battery read
const byte msgLength = 72;                                   //total message len. bytes: 4 = id, 49 = free text, 1 = ttl, 1 = control, 8 = 8byte, 8 = 8byte.  =71
const byte textLimit = 49;                                   //max TEXT message len. LIMITED BY SCREEN RESOLUTION for one line of text for a choosen font.
String TXbuffer;                                             //send packet form
bool configtxt = 0;                                          //config file error parsing flag
ts tiime;                                                    //ts is a struct findable in ds3231.h
char s[19];                                                  //RTCtime sprint buffer
char sdate[10];                                              //RTC date for filenaming
byte toTouch = 150;                                          //points to touch while touch calibration initialized, (search var for other place)
uint16_t x_touch, y_touch;
int keyDelay = 150;                                          //key auto-repeat ms
unsigned int lightTimeout = 0;                               //see below(where?)
byte lcd_timeout = 60;                                       //here is LCD led timeout. in seconds!+
bool newM = false;                                           //got new message while lcd off
int motorTime = 0;
bool light = false;
char MyBuffer[msgLength];                                    //input text buffer
bool showTouchDebug = false;                                 //yellow touch calibration marks
bool menuStay = true;                                        //menu inner loop
bool timeMenuStay = true;                                    //time menu inner loop
bool loraMenuStay = true;                                    //lora menu inner loop
float antennaL = 0;                                          //Dipole length, cm
bool inMenu = false;                                         //main or menu

typedef struct {
  String v1st = "";                              //8 byte         //[8][8][1][1][xIDx][0123456789012345678901234567890123456789012345678]
  String v2nd = "";                              //8 byte         //5D1F7777777777777777777777777777777777777777777777777XYzzzzzzzz88888888
  String ttl = "";                               //1 byte         //Not used for now.
  String contr = "";                             //1 byte         //jjjj:888888::::
  String id = "";                                //4 byte         //ID
  String text = "";                              //49 bytes max   //free text
} vseLinii;
vseLinii vseLinii2[10];





TFT_eSPI tft = TFT_eSPI();
SdFat SD;
File myFile;
Adafruit_NeoPixel rgb(1, RGB_LED, NEO_GRB + NEO_KHZ800);
ClickButton button1(BUTTON, LOW, CLICKBTN_PULLUP);
