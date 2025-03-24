#include <LoRa.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 16, /* clock=*/ 15, /* data=*/ 4);   // ESP32 Thing, HW I2C with pin remapping for Heltek LoRa ESP32 V2

#define BAND    433E6                         //you can set band here directly,e.g. 868E6,915E6

#define LORA_DEFAULT_SPI           SPI;
#define LORA_DEFAULT_SPI_FREQUENCY 8E6 ;
#define LORA_DEFAULT_SS_PIN        18;
#define LORA_DEFAULT_RESET_PIN     14;
#define LORA_DEFAULT_DIO0_PIN      26;


byte msgCount = 0;                           // count of outgoing messages


void setup()
{
  pinMode(25, OUTPUT);                       //onboard led
  Serial.begin(115200);
  u8g2.begin();

  LoRa.setPins(18, 14, 26);                  //(ss, reset, dio0-interrupt)
  LoRa.begin(433000000);
  Serial.println("Start");
  delay(100);
  LoRa.setFrequency(433000000);
  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(11);       //6-12
  LoRa.setSignalBandwidth(125E3);    //7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3(def), 250E3
  LoRa.setCodingRate4(8);            //Supported values are between 5 and 8, these correspond to coding rates of 4/5 and 4/8.
  LoRa.enableCrc();


  Serial.println("LoRa init succeeded.");

  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_4x6_mf); // choose a suitable font
  u8g2.drawStr(0, 6, "LoRa init succeeded."); // write something to the internal memory
  // u8g2.drawStr(0, 12, "1234567890AWQMN_SomeTestString");
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);
}

void loop()
{
  if (digitalRead(0) == false)                       //PRG button click
  {
    String message = "true ";   // send a message
    message += msgCount;

    Serial.println("Sending " + message);
    u8g2.drawStr(0, 18, "Sending ");
    u8g2.setCursor(32, 18);
    u8g2.print(message);
    sendMessage(message);  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    delay(200);
    msgCount++;
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  u8g2.sendBuffer();
}

void sendMessage(String outgoing)
{
  digitalWrite(25, HIGH);
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  digitalWrite(25, LOW);
}

void onReceive(int packetSize)
{
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  String incoming = "";

  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }

  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));


  u8g2.setCursor(0, 30);
  u8g2.print("Message: ");
  u8g2.setCursor(0, 37);
  u8g2.print(incoming);
  u8g2.setCursor(0, 64);
  u8g2.print("RSSI: " + String(LoRa.packetRssi()));
  u8g2.setCursor(84, 64);
  u8g2.print("Snr: " + String(LoRa.packetSnr()));
  u8g2.sendBuffer();
}
