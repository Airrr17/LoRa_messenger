# Blue pill STM32 based broadcast* rawtext communication device.  
(*means for privet chatroom or smallgroup chatting you'll need to manage one or more settings:  
Freq, SpreadingFactor, Bandwidth, CodingRate, syncWord and CRC or even implement encryption with AESLib.)  
See v1.1 information.  
There is NO receive confirmation implemented yet.  
  
You can view received messages on screen and send with onscreen keyboard.  
  
You will need:  
- Blue pill board with arduino bootloader. (a 64k version can be used). $1.85  
- ili9341 TFT with touch and SD card (2.2, 2.4 or 2.8"). $6.24  
- DS3231 RTC, as stm32's RTC wont _increment_ time without power (of course with Vbat present). My bad. $0.74  
- RA-02 LoRa module. $3.37  
  
*_To communicate youll need at least two devices :)!!!!!!_  
  
  
# VERSION 1.1 update (23.05.21):  
Since v1.1 you don't need to calibrate touch positions anymore. Not even compile the sources. Wire up all together.  
Burn _bin_ provided (STLink or any) then insert formatted SD card and setup will begin.  
You will be prompted to move stylus over corners until counter became 0.  
Just follow the instructions on the screen.  
Then setup will create "config.txt" file on SD root.  
First four lines is calibrated X-Y positions. Do not change!  
Others are for LORA setup. Changing those values leads to communication variants.  
Two or more units with the same settings only will communicate.  
To repeat setup process delete settings.txt or command "resetconfig" then send.  
  
+v1.1 Much more debug information printed on screen during boot or setup.  
+v1.1 Current LORA settings are displayed at startup.  
+v1.1 To set time and date use command  "TssmmhhWDDMMYYYY" (ie T003201123052021) W = 1 - Sunday,  
2 - Monday, and so on. Then press send.  
+v1.1 Here is _LoRa-Heltek.ino_ for HELTEK LoRa32 (esp32) board. This provides compatible hardware for communication tests.  
Already assembled board. Thus you can set up only one LoRa_messenger and test settings or range with heltek.  
Software prints last received message on OLED and sends packets on PRG button press.  
  
  
All messages are logged to SD card in 'current date's filename' (ie 29092019).  
Received messages are logged to SD with their RSSI, SNR and FreqErr values for better debuging.  
Log line example:  
23.02.20  04:10:43  Rx: Hello AAA No:0 >>> RSSI: -55 SNR: 11.00 freqErr: 582  
  
How far you can communicate? It's complex question depends on so many parameters. There is no proper answer.  
Some peoples achived thousands of kilometers within line of sight with directional antenna. The last word:  
everything depends on the right choosen antennas. Current setting are set for maximal range.  
  
Hardware connections:  
  
TFT, SD, LoRa and Touch SPI:  
SCK        - PA5  
MISO       - PA6  
MOSI       - PA7  
Other:  
TFT reset  - reset  
TFT LED    - PA2 (30sec timeout)  
TFT SS     - PA4  
SD SS      - PA3  
Touch SS   - PB11  
TFT DC     - PA1  
Touch irq  - PB10  
LoRa irq   - PB1  
LoRa reset - PB0  
LoRa SS    - PA0  
i2c:  
DS3231 SDA    - PB7  
DS3231 SCL    - PB6  
  
![alt text](https://github.com/Airrr17/LoRa_messenger/blob/master/LoRa1.jpg)  
  
![alt text](https://github.com/Airrr17/LoRa_messenger/blob/master/LoRa2.jpg)  
  
## How to setup proper touch position:  

UPD v1.1: Built-in calibration and time setting are implemented.  
Obsolete:  
Start programm and enter "calibON" command or modify line 256.  
Touch anywhere and see how far touched point being painted yellow.  
Adjust in code TS_MINX, TS_MINY, TS_MAXX and TS_MAXY and recompile. :)  
  
Set DS3231 time and date with sketch from DS3231 library.  
Please use only libraries mentioned in the code (ds3231.h and XPT2046_touch.h).  
  
![alt text](https://github.com/Airrr17/LoRa_messenger/blob/master/LoRa_main.png)  
    
## ToDo:  
  
- Adjust clock and date via interface command. DONE! v1.1  
- Set\show freq, SpreadingFactor, Bandwidth, CodingRate and syncWord via interface commands. DONE! v1.1  
- Makeup code to use 2D messages array instead of 10 arrays :)  
- Better tr\rx messages color marking with alligning to both sides (SMS style).  
- Show battery voltage on tft.  
- Vibrate\beeper on msg receive.  
