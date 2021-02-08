# SyncLed

A controller for addressable RGB-Ledstrips (WS2812b) based on the ESP8266.  
Focused on synchronisation and customization.

## What you need:
- ESP8266 (ESP-12E or DevBoards)
- WS2812b Ledstrip
- a sufficient 5V PowerSupply (for 180 leds I use one with 10 Amps)

## First things first:
- connect the leds and the ESP to the PowerSupply  
(**CAUTION**: the ESP-12E is a **3.3V** device - I use 2 diodes to drop the voltage)
- connect the data pin of the leds to the Rx pin of the ESP (allows fast HW-accelerated communication)
- change the line `IPAddress ip(192, 168, 178, ...);` in ***SyncLed.ino*** to whatever IP you want for your Led-Controller
- Upload the sketch!
