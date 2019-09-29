#Blue pill STM32 based 2way communication device.

You can view received messeges on screen and send with onscreen keyboard.

You will need:

-Blue pill board with arduino bootloader.

-ili9341 TFT with touch (2.2, 2.4 or 2.8").

-DS3231 RTC, as stm32's RTC wont _increment_ time without power (of course with Vbat present).

-SD card module (maybe on TFT's board).

-RA-02 LoRa module.

*To communicate you need atleast two devices :)


Modify source for MAX/MIN X/Y touch, depens on tft.

Also LoRa setting can be modified (SpreadingFactor, Bandwidth, CodingRate).

All messeges are logged to SD card in 'current date's filename' (ie 29092019).


Messeges scrolling-up are currently not implemented. 


Hardware connections:

TFT, SD and Touch SPI:

SCK        - PA5

MISO       - PA6

MOSI       - PA7

TFT SS     - PA4

SD SS      - PA3

TFT DC     - PA1

Touch SS   - PB11

Touch irq  - PB10

LoRa irq   - PB1

LoRa reset - PB0

RTC SDA    - PB7

RTC SCL    - PB6


![alt text](https://github.com/Airrr17/LoRa_messenger/blob/master/LoRa1.jpg)
