/*
This example will transmit a universe via Art-Net into the network.

Copyright (c) 2020 Stephan Ruloff
https://github.com/rstephan/ArtnetnodeWifi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, under version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <ArtnetnodeWifi.h>


//Wifi settings
const char* ssid = "ssid"; // CHANGE FOR YOUR SETUP
const char* password = "pAsSwOrD"; // CHANGE FOR YOUR SETUP

// Artnet settings
const int startUniverse = 0; // CHANGE FOR YOUR SETUP for most software this is 1, some software send out artnet first universe as 0.
const char host[] = "2.1.1.1"; // CHANGE FOR YOUR SETUP your destination


ArtnetnodeWifi artnet;


// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}

void setup()
{
  Serial.begin(115200);
  ConnectWifi();
  artnet.begin(host);
  artnet.setLength(3);
  artnet.setUniverse(startUniverse);
  artnet.setShortName("Transmit Example"); // max. 17 characters
  artnet.setLongName("ArtnetnodeWifiTransmit Example"); // max. 63 characters
  artnet.setPortType(0, 0x40);
}

void loop()
{
  uint8_t i;
  uint8_t j;

  // set the first 3 byte to all the same value. A RGB lamp will show a ramp-up white.
  for (j = 0; j < 255; j++) {
    for (i = 0; i < 3; i++) {
      artnet.setByte(i, j);
    }
    // send out the Art-Net DMX data
    artnet.write();

    artnet.read();
    delay(100);
  }
}
