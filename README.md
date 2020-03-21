# ArtnetnodeWifi

This is a version for Arduino ESP8266 modules. It is adapted to use the ESP8266 Wifi instead of ethernet.
Besides the ESP8266, it can also run on ESP32, WiFi101 (e.g. MKR1000) or WiFiNINA (e.g. NANO 33 IoT) devices.

## Installation

Place this in your `~/Documents/Arduino/libraries` folder.

## Supported OpCodes

|Name           | Status | Description
|---------------|--------|--------------------------|
|OpDmx/Output   | yes    | Send/Receive DMX-data    |
|OpNzs          | yes    | DMX-data, non-zero start |
|OpPoll/Response| yes    | Request device status    |
|OpCommand      | no     | |
|OpRdm*         | no     | |
|OpVideo*       | no     | |
|OpMedia*       | no     | |
|OpFile*        | no     | |
|OpDirectory*   | no     | |


## Examples

### RGB LED

Light up 3 LEDs (R, G and B) based on artnet frames.

### Send DMX

Send out DMX messages on the serial port at 250k

### Debug

Send out the debug-text on the serial port at 115.2k

### Transmit

Transmit a universe into the network.

## Copyright
Stephan Ruloff, 2016-2020
https://github.com/rstephan/ArtnetnodeWifi

# Art-Net

Art-Net(tm) is a trademark of Artistic Licence Holdings Ltd. The Art-Net protocol and associated documentation is copyright Artistic Licence Holdings Ltd.

[Art-Net](http://www.artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf)

# History

Based on: https://github.com/solexious/Artnetnode

# Artnetnode
An artnet and DMX processing and handling library for the Tiva C Launchpad though the Energia IDE

### Requirements
Requires a build of the latest energia from git for changes to the ethernet library

https://github.com/energia/Energia

### Copyright
Charles Yarnold 2015 - charlesyarnold@gmail.com

### Acknowledgments
* DMX sending based on - https://playground.arduino.cc/Learning/DMXSerial
* Artnet handling inspired by - https://github.com/natcl/Artnet
* https://github.com/solexious/Artnetnode
* https://github.com/solexious/ESP8266_artnet_led_node
