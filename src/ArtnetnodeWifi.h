#ifndef ARTNETNODEWIFI_H
#define ARTNETNODEWIFI_H
/*

Copyright (c) Charles Yarnold charlesyarnold@gmail.com 2015

Copyright (c) 2016 Stephan Ruloff
https://github.com/rstephan

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

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "OpCodes.h"
#include "NodeReportCodes.h"
#include "StyleCodes.h"
#include "PriorityCodes.h"
#include "ProtocolSettings.h"
#include "PollReply.h"

// Packet
#define ART_NET_ID "Art-Net"
#define ART_DMX_START 18

class ArtnetnodeWifi
{
public:
  ArtnetnodeWifi();

  uint8_t begin(void);
  uint16_t read();

  // Node identity
  uint8_t setShortName(const char name[]);
  uint8_t setLongName(const char name[]);
  uint8_t setName(const char name[]);

  uint8_t setStartingUniverse(uint16_t startingUniverse);

  // DMX controls
  void enableDMX();
  void disableDMX();
  void enableDMXOutput(uint8_t outputID);
  void disableDMXOutput(uint8_t outputID);

  uint8_t setDMXOutput(uint8_t outputID, uint8_t uartNum, uint16_t attachedUniverse);

  // DMX tick
  void tickDMX(uint32_t time);
  
  // Return a pointer to the start of the DMX data
  inline uint8_t* getDmxFrame(void)
  {
    return artnetPacket + ART_DMX_START;
  }

  inline void setArtDmxCallback(void (*fptr)(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)) 
  {
    artDmxCallback = fptr;
  }

private:
  WiFiUDP Udp;
  PollReply PollReplyPacket;

  // Packet handlers
  uint16_t handleDMX(uint8_t nzs);
  uint16_t handlePollRequest();
  
  // Packet vars
  uint8_t artnetPacket[ARTNET_MAX_BUFFER];
  uint16_t packetSize;
  uint16_t opcode;
  IPAddress localIP;
  IPAddress localMask;
  IPAddress localBroadcast;

  // Packet functions
  bool isBroadcast();

  // DMX settings
  bool DMXOutputStatus;
  uint16_t DMXOutputs[DMX_MAX_OUTPUTS][3];
  uint8_t DMXBuffer[DMX_MAX_OUTPUTS][DMX_MAX_BUFFER];

  uint16_t startingUniverse;

  // DMX tick
  void sendDMX();
  uint8_t* getDmxFrame(uint8_t outputID);
  uint8_t msSinceDMXSend;

  void (*artDmxCallback)(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data);
  static const char artnetId[];
};

#endif
