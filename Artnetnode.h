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

class Artnetnode{
public:
  Artnetnode();

  uint8_t begin(byte mac[]);
  uint16_t read();

  // Node identity
  uint8_t setShortName(char name[]);
  uint8_t setLongName(char name[]);

  // DMX controls
  uint8_t enableDMX();
  uint8_t disableDMX();
  

private:
  EthernetUDP Udp;

  // Node identity
  char shortName[ARTNET_SHORT_NAME_MAX_LENGTH+1];
  char longName[ARTNET_LONG_NAME_MAX_LENGTH+1];

  // Packet handlers
  uint16_t handleDMX();
  uint16_t handlePollRequest();
  
  // Packet vars
  uint8_t artnetPacket[ARTNET_MAX_BUFFER];
  uint16_t packetSize;
  uint16_t opcode;

  // DMX settings
  bool DMXOutputStatus;
};

#endif