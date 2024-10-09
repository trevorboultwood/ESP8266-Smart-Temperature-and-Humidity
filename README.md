# Introduction

This project ultilises the low cost D1 ESP8266 microcontroller in order to POST temperature and humidity to a HTTP server. 

# Preface

I live in a traditional stone cottage within the United Kingdom, and I am trying to manage the humidity within the property and this project aims to assist me in logging and tracking, in order to guide me on where i need to heat, or run the dehumidifier.

# Hardware

Originally, this project was going to be run on an ESP8266 01 device, due to it's low size and low powersupply. I actually designed a PCB and charging circuit, however, given the cost and the likelyhood of it not working, i searched for ESP8266 on Alliexpress which reveals this type of development board.

![alt text](/media/esp8266%20D1.jpg "WeMos D1 ESP-Wroom-02 ESP8266 Nodemcu WiFi Module With 18650 Battery Charging")
Figure - WeMos D1 ESP-Wroom-02 ESP8266 Nodemcu WiFi Module With 18650 Battery Charging

I picked a couple of these up for less than £6.00 includding shipping from the following link. [1 ESP-Wroom-02 ESP8266 Nodemcu](https://www.aliexpress.com/item/1005007129983755.html?spm=a2g0o.order_list.order_list_main.10.525e1802KDzQMa), but there are many other procurement routes.


This board comes with the charging circuitry required for an 18650 battery, again these are easily available.

Finally, the DHT11/22 is simply soldered to the board, it should be noted that the PCB produces heat, so, it is wise to insulate the PCB away from the sensor. The code can be updated for either sensor; the DHT22 is the preffered choice, given it's increased accuracy and precision.

# Software

This project uses PlatformIO, which runs on VSCode. THis project can easily be ported to the Arduino IDE, but I find PlatformIO the better tool for programming ESP boards. I personally find the compule times are a lot quicker, and if i need to modify a library on the fly, this can easily be done. VSCode has a load of hotkeys which makes developing embedded code a breeze.

The code can be changed from being a server, client or both. This can be controlled via the definititons at the top of the file.

When the server is enabled, a simple HTTP server is spawned, and when accessed from a web browser or CURL request, the temperature and humidty is returned.

When the client is enabled, a simple HTTP `POST` request is made to the endpoint defined at the top of the file. The frequency in which the ESP carries out the HTTP `POST` request can also be configured by modifying the variable towards the top of the file. If <b>only</b> the client is enabled, to preservere battery life, the ESP enters deep sleep. This requires a jumper to be added to the D0 (GPIO16) and the reset pin (RST). If this jumper is not added, the ESP will not enter deep sleep, but will still work as intended, but the ESP controller will remain connected to the WIFI network and will be consuming normal power.







The code is really noddy....

I turn on,

deep sleep,

wake up

read sensor vales.


post to an HTTP server


go back to sleep
