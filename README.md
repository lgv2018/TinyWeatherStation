# TinyWeatherStation
This is an open sourced small weather station that are able to retrieve data from Open Weather and display it on TFT LCD Screen. Powered with Hibiscus Sense by Myduino

TO ADD IMAGE HERE

## Hardware

1. Hibiscus Sensy ESP32 IoT Developtment Board - [Cytron](https://my.cytron.io/p-hibiscus-sense-esp32-iot-development-board?search=hibiscus&description=1) or [Myduino](https://myduino.com/product/myd-036/)
2. 1.44-inch 128x128 TFT LCD Breakout (ST7735) - [Cytron](https://my.cytron.io/p-1.44-inch-128x128-tft-lcd-breakout-st7735?search=st7735&description=1)
3. Female-Female Jumper Wire - [Cytron](https://my.cytron.io/p-40-ways-female-to-female-jumper-wire?search=jumper%20wire&description=1)
4. 3D Printed Enclosure - ADD LINK TO THINGIVERSE HERE
5. Type C USB Cable for programming and power supply

## Software

1. Arduino IDE

## Library Used
1. NTPClient         -   Date/Time manager 
2. WiFi.h            -   Wifi Configuration
3. WiFiUdp           -   Library to send and receive UDP messages
4. WiFiClient        -   Driver for WiFi client
5. ArduinoJson       -   Arduino Json to parse request into JSON object. Installed version 5.13, last version is not compatible.
6. Adafruit_GFX      -   LCD graphical driver
7. TFT_eSPI          -   Graphics and font library for ST7735 driver chip
8. SPI               -   SPI connection
9. SPIFFS            -   Serial Peripheral Interface Flash File System to store and retrieve data on ESP
10. Adafruit_NeoPixel -   RGB Lighting
