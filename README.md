# Blue pill STM32 based broadcast* rawtext communication device.  
(*means for privet chatroom or smallgroup chatting you'll need to manage one or more settings:  
SpreadingFactor, Bandwidth, CodingRate and syncWord or even implement encryption with AESLib.)  
There is NO receive confirmation implemented yet.  
  
(Un)comment lines 441-443 for real sendng. Change freq. for your RA-02 specs (line 150).  
  
You can view received messages on screen and send with onscreen keyboard.  
  
You will need:  
- Blue pill board with arduino bootloader. (a 64k version can be used). $1.85  
- ili9341 TFT with touch and SD card (2.2, 2.4 or 2.8"). $6.24  
- DS3231 RTC, as stm32's RTC wont _increment_ time without power (of course with Vbat present). My bad. $0.74  
- RA-02 LoRa module. $3.37  
  
*To communicate youll need atleast two devices :)!!!!!!!!!!!!!!!!!!!!!!!!!!!!  
  
Modify source for MAX/MIN X/Y touch, depens on tft. Some touch screens is mirrored on some axis, see source.  
LoRa setting can be modified (SpreadingFactor, Bandwidth, CodingRate and syncWord).  
All messages are logged to SD card in 'current date's filename' (ie 29092019).  
Received messages are logged to SD with their RSSI and SNR values for better debuging.  
Log line example:  
05.10.19  19:37:36  Rx: Hello AAA No:0   With RSSI: -23 SNR: 12.00  
  
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

Start programm and enter "calibON" command or modify line 256.  
Touch anywhere and see how far touched point being painted yellow.  
Adjust in code TS_MINX, TS_MINY, TS_MAXX and TS_MAXY and recompile. :)  
  
Set DS3231 time and date with sketch from DS3231 library.  
Please use only libraries mentioned in the code (ds3231.h and XPT2046_touch.h).  
  
![alt text](https://github.com/Airrr17/LoRa_messenger/blob/master/LoRa_main.png)  
    
## ToDo:  
  
- Adjust clock and date via interface command.  
- Set\show freq, SpreadingFactor, Bandwidth, CodingRate and syncWord via interface commands.  
- Makeup code to use 2D messages array instead of 10 arrays :)  
- Better tr\rx messages color marking with alligning to both sides (SMS style).  
- Show battery voltage on tft.  
- Vibrate\beeper on msg receive.
