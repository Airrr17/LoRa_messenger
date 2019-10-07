#Blue pill STM32 based 2way communication device.  
  
Uncoment lines 434-436 for real sendng. Change freq. for your RA-02 specs (line 147).  
  
You can view received messages on screen and send with onscreen keyboard.  
  
You will need:  
-Blue pill board with arduino bootloader. (Sketch uses 59104 bytes, so a 64k version can be used).  
-ili9341 TFT with touch (2.2, 2.4 or 2.8").  
-DS3231 RTC, as stm32's RTC wont _increment_ time without power (of course with Vbat present). My bad.  
-SD card module (maybe on TFT's board).  
-RA-02 LoRa module.  
  
*To communicate youll need atleast two devices :)  
  
Modify source for MAX/MIN X/Y touch, depens on tft.  
Also LoRa setting can be modified (SpreadingFactor, Bandwidth, CodingRate and syncWord).  
All messages are logged to SD card in 'current date's filename' (ie 29092019).  
Received messages are logged with their RSSI and SNR.  
Log line example:  
05.10.19  19:37:36  Rx: Hello AAA No:0   With RSSI: -23 SNR: 12.00  
  
  
  
Hardware connections:  
  
TFT, SD and Touch SPI:  
SCK        - PA5  
MISO       - PA6  
MOSI       - PA7  
Other:  
TFT SS     - PA4  
SD SS      - PA3 (weak pullup adviced)  
TFT DC     - PA1  
Touch SS   - PB11  
Touch irq  - PB10  
LoRa irq   - PB1  
LoRa reset - PB0  
DS3231 SDA    - PB7  
DS3231 SCL    - PB6  
  
![alt text](https://github.com/Airrr17/LoRa_messenger/blob/master/LoRa1.jpg)  
  
![alt text](https://github.com/Airrr17/LoRa_messenger/blob/master/LoRa2.jpg)  
  
  
How to setup proper touch position:  
Start programm and enter "calibON" command.  
Touch anywhere and see how far touched point is painted yellow.  
Adjust in code TS_MINX, TS_MINY, TS_MAXX and TS_MAXY and recompile. :)  
  
Set DS3231 time and date with sketch from DS3231 library.  
Please use only libraries mentioned in the code (ds3231.h and XPT2046_touch.h).  
  
  
ToDo:  
-Set clock and date via interface command.  
-Better SD card initialization.  
-Makeup code to use 2D message array instead of 10 arrays :)  
-Better tr\rx messages color marking with alligning to left.  