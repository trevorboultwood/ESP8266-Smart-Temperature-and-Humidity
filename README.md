# ESP8266 SmartTemp

## Introduction

This project utilises the low cost D1 ESP8266 microcontroller in order to host an HTTP server and/or send temperature and humidity to an external HTTP server (ESP as client).

## Preface

I live in a traditional stone cottage within the United Kingdom, and I am trying to manage the humidity within the property, and this project aims to assist me in logging and tracking, in order to guide me on where I need to heat, or run the dehumidifier.

I didn't want the project to be plugged into a USB charger, and therefore chose a development board which comes equipped with battery and charging capability.

## Hardware

Originally, this project was going to be run on an ESP8266 01 device, due to its low size and low power consumption. I actually designed a PCB and charging circuit, however, given the cost and testing required, I searched for ESP8266 on Aliexpress, revealing the following type of development board.

![alt text](/media/esp8266%20D1.jpg "WeMos D1 ESP-Wroom-02 ESP8266 Nodemcu WiFi Module With 18650 Battery Charging")
Figure - WeMos D1 ESP-Wroom-02 ESP8266 Nodemcu WiFi Module With 18650 Battery Charging

I picked a couple of these up for less than £6.00 includding shipping from the following link. [1 ESP-Wroom-02 ESP8266 Nodemcu](https://www.aliexpress.com/item/1005007129983755.html?spm=a2g0o.order_list.order_list_main.10.525e1802KDzQMa), but there are many other procurement routes.

This board comes with the charging circuitry required for an 18650 battery, again these are easily available.

The DHT11/22 is simply soldered to the board, it should be noted that the PCB produces heat, so, it is wise to insulate the PCB away from the sensor. The code can be updated for either sensor; the DHT22 is the preferred choice, given it's increased accuracy and precision with little difference in price.

Lastly, this code will also run on the ESP8266 01 and ESP32 boards with a little configuration!

## Software

This project uses PlatformIO, which runs on VSCode. THis project can easily be copied into the Arduino IDE, but I find PlatformIO the better tool for programming ESP boards. I personally find the compile times are a lot quicker, and if I need to modify a library on the fly, this can easily be done. VSCode has a load of hotkeys which makes developing embedded code a breeze, I urge those who haven't used PlatformIO to give it a shot.

The code can be changed from being a server, client or both. This can be controlled via the definitions at the top of the file.

When the server is enabled, a simple HTTP server is spawned, and when accessed from a web browser or CURL request, the temperature and humidity is returned.

When the client is enabled, a simple HTTP `POST` request is made to the endpoint defined at the top of the file. The frequency in which the ESP carries out the HTTP `POST` request can also be configured by modifying the variable towards the top of the file. If <b>only</b> the client is enabled, to preserve battery life, the ESP enters deep sleep. This requires a jumper to be added to the D0 (GPIO16) and the reset pin (RST). If this jumper is not added, the ESP will not enter deep sleep, but I have designed the code to still work, but the ESP controller will remain connected to the WIFI network and will delay every N seconds instead.

Enabling DeepSleep is explained below.

## Further power reduction

In order to reduce the power the SmartTemp board uses; hardware modifications are required.

Additional power saving measures include:

- Not using `Serial.print`.
- Remove the CH340 TTL chip or 5V regulator.
- Having a longer delay between POST requests.
 
IT takes the ESP roughly 6 seconds to carry out a HTTP `POST` request. When the ESP is not in deep sleep it consumes roughly 20-60mA/H. When in deep sleep, this is reduced to <1mA/H.

The 5v regulator and CH340 IC consume roughly 20mA/H at idle!

Given these modifications, you could expect a 3000A/H 18650 battery with an interval of 1 minute to last roughly 25 days without the need for a recharge!

## Deep sleep Enable

## Remove 5v regulator CH340

Remove 5v regulator - to document

## Outcome

Before

After

## 3D printed enclosure

Not completed yet.
