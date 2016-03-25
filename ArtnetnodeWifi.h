/*

Copyright (c) Charles Yarnold charlesyarnold@gmail.com 2015

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

#ifndef ARTNETNODE_H
#define ARTNETNODE_H

#include "Energia.h"
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "OpCodes.h"
#include "NodeReportCodes.h"
#include "StyleCodes.h"
#include "PriorityCodes.h"
#include "ProtocolSettings.h"
#include "PollReply.h"


class Artnetnode{
public:
  Artnetnode();

  uint8_t begin(byte mac[], uint8_t numOutputs);
  uint16_t read();

  // Node identity
  uint8_t setShortName(char name[]);
  uint8_t setLongName(char name[]);
  uint8_t setName(char name[]);

  uint8_t setStartingUniverse(uint16_t startingUniverse);

  // DMX controls
  void enableDMX();
  void disableDMX();
  void enableDMXOutput(uint8_t outputID);
  void disableDMXOutput(uint8_t outputID);

  uint8_t setDMXOutput(uint8_t outputID, uint8_t uartNum, uint16_t attachedUniverse);

  // DMX tick
  void tickDMX(uint32_t time);

private:
  EthernetUDP Udp;
  PollReply PollReplyPacket;

  // Packet handlers
  uint16_t handleDMX();
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
  void uartDMX(uint8_t outputID, uint8_t uartNum);
  uint8_t* getDmxFrame(uint8_t outputID);
  uint8_t msSinceDMXSend;
};

#endif