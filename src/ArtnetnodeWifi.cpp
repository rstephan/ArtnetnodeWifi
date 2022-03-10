/*

Copyright (c) Charles Yarnold charlesyarnold@gmail.com 2015

Copyright (c) 2016-2020 Stephan Ruloff
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


const char ArtnetnodeWifi::artnetId[] = ARTNET_ID;

ArtnetnodeWifi::ArtnetnodeWifi()
{
  // Initalise DMXOutput array
  for (int i = 0; i < DMX_MAX_OUTPUTS; i++) {
    DMXOutputs[i][0] = 0xFF;
    DMXOutputs[i][1] = 0xFF;
    DMXOutputs[i][2] = 0;
  }

  // Start DMX tick clock
  msSinceDMXSend = 0;

  // Init DMX buffers
  for (int i = 0; i < DMX_MAX_OUTPUTS; i++) {
    memset(DMXBuffer[i], 0, sizeof(DMXBuffer[i]));
  }

  sequence = 1;
  physical = 0;
  outgoingUniverse = 0;
  dmxDataLength = 0;
}

/**
@retval 0 Ok
*/
uint8_t ArtnetnodeWifi::begin(String hostname)
{
  byte mac[6];

  Udp.begin(ARTNET_PORT);
  localIP = WiFi.localIP();
  localMask = WiFi.subnetMask();
  localBroadcast = IPAddress((uint32_t)localIP | ~(uint32_t)localMask);

  WiFi.macAddress(mac);
  PollReplyPacket.setMac(mac);
  PollReplyPacket.setIP(localIP);
  PollReplyPacket.canDHCP(true);
  PollReplyPacket.isDHCP(true);

  host = hostname;

  return 0;
}

void ArtnetnodeWifi::setShortName(const char name[])
{
  PollReplyPacket.setShortName(name);
}

void ArtnetnodeWifi::setLongName(const char name[])
{
  PollReplyPacket.setLongName(name);
}

void ArtnetnodeWifi::setName(const char name[])
{
  PollReplyPacket.setShortName(name);
  PollReplyPacket.setLongName(name);
}

void ArtnetnodeWifi::setNumPorts(uint8_t num)
{
  PollReplyPacket.setNumPorts(num);
}

void ArtnetnodeWifi::setStartingUniverse(uint16_t startingUniverse)
{
  PollReplyPacket.setStartingUniverse(startingUniverse);
}

uint16_t ArtnetnodeWifi::read()
{
  uint8_t startcode;

  packetSize = Udp.parsePacket();

  if (packetSize <= ARTNET_MAX_BUFFER && packetSize > 0) {
    senderIp =  Udp.remoteIP();
    Udp.read(artnetPacket, ARTNET_MAX_BUFFER);

    // Check that packetID is "Art-Net" else ignore
    if (memcmp(artnetPacket, artnetId, sizeof(artnetId)) != 0) {
      return 0;
    }

    opcode = artnetPacket[8] | artnetPacket[9] << 8;

    switch (opcode) {
    case OpDmx:
      return handleDMX(0);
    case OpPoll:
      return handlePollRequest();
    case OpNzs:
      startcode = artnetPacket[13];
      if (startcode != 0 && startcode != DMX_RDM_STARTCODE) {
        return handleDMX(startcode);
      }
    }

    return opcode;
  }

  return 0;
}

uint16_t ArtnetnodeWifi::makePacket(void)
{
  uint16_t len;
  uint16_t version;

  memcpy(artnetPacket, artnetId, sizeof(artnetId));
  opcode = OpDmx;
  artnetPacket[8] = opcode;
  artnetPacket[9] = opcode >> 8;
  version = 14;
  artnetPacket[10] = version >> 8;
  artnetPacket[11] = version;
  artnetPacket[12] = sequence;
  sequence++;
  if (!sequence) {
    sequence = 1;
  }
  artnetPacket[13] = physical;
  artnetPacket[14] = outgoingUniverse;
  artnetPacket[15] = outgoingUniverse >> 8;
  len = dmxDataLength + (dmxDataLength % 2); // make an even number
  artnetPacket[16] = len >> 8;
  artnetPacket[17] = len;

  return len;
}

int ArtnetnodeWifi::write(void)
{
  uint16_t len;

  len = makePacket();
  Udp.beginPacket(host.c_str(), ARTNET_PORT);
  Udp.write(artnetPacket, ARTNET_DMX_START_LOC + len);

  return Udp.endPacket();
}

int ArtnetnodeWifi::write(IPAddress ip)
{
  uint16_t len;

  len = makePacket();
  Udp.beginPacket(ip, ARTNET_PORT);
  Udp.write(artnetPacket, ARTNET_DMX_START_LOC + len);

  return Udp.endPacket();
}

void ArtnetnodeWifi::setByte(uint16_t pos, uint8_t value)
{
  if (pos > 512) {
    return;
  }
  artnetPacket[ARTNET_DMX_START_LOC + pos] = value;
}


bool ArtnetnodeWifi::isBroadcast()
{
  if (Udp.remoteIP() == localBroadcast){
    return true;
  } else {
    return false;
  }
}

uint16_t ArtnetnodeWifi::handleDMX(uint8_t nzs)
{
  if (isBroadcast()) {
    return 0;
  } else {
    // Get universe
    uint16_t universe = artnetPacket[14] | artnetPacket[15] << 8;

    // Get DMX frame length
    uint16_t dmxDataLength = artnetPacket[17] | artnetPacket[16] << 8;
    
    // Sequence
    uint8_t sequence = artnetPacket[12];

    if (artDmxCallback) {
      (*artDmxCallback)(universe, dmxDataLength, sequence, artnetPacket + ARTNET_DMX_START_LOC);
    }

    for(int a = 0; a < DMX_MAX_OUTPUTS; a++){
      if(DMXOutputs[a][1] == universe){
        for (int i = 0 ; i < DMX_MAX_BUFFER ; i++){
          if(i < dmxDataLength){
            DMXBuffer[a][i] = artnetPacket[i+ARTNET_DMX_START_LOC];
          }
          else{
            DMXBuffer[a][i] = 0;
          }
        }
      }
    }

    if (nzs) {
      return OpNzs;
    } else {
      return OpDmx;
    }
  }
}

uint16_t ArtnetnodeWifi::handlePollRequest()
{
  if (1 || isBroadcast()) {
    Udp.beginPacket(localBroadcast, ARTNET_PORT);
    Udp.write(PollReplyPacket.printPacket(), sizeof(PollReplyPacket.packet));
    Udp.endPacket();

    return OpPoll;
  } else{
    return 0;
  }
}

void ArtnetnodeWifi::enableDMX()
{
  DMXOutputStatus = true;
}

void ArtnetnodeWifi::disableDMX()
{
  DMXOutputStatus = false;
}

void ArtnetnodeWifi::enableDMXOutput(uint8_t outputID)
{
  DMXOutputs[outputID][2] = 1;

  int numEnabled = 0;
  for(int i = 0; i < DMX_MAX_OUTPUTS; i++){
    if(DMXOutputs[i][2]==1){
      if(numEnabled<4){
        numEnabled++;
      }
    }
  }
  PollReplyPacket.setNumPorts(numEnabled);
  PollReplyPacket.setOutputEnabled(outputID);
}

void ArtnetnodeWifi::disableDMXOutput(uint8_t outputID)
{
  DMXOutputs[outputID][2] = 0;

  int numEnabled = 0;
  for(int i = 0; i < DMX_MAX_OUTPUTS; i++){
    if(DMXOutputs[i][2]==1){
      if(numEnabled<4){
        numEnabled++;
      }
    }
  }
  PollReplyPacket.setNumPorts(numEnabled);
  PollReplyPacket.setOutputDisabled(outputID);
}

uint8_t ArtnetnodeWifi::setDMXOutput(uint8_t outputID, uint8_t uartNum, uint16_t attachedUniverse)
{
  // Validate input
  if(outputID < DMX_MAX_OUTPUTS && uartNum != 0xFF && attachedUniverse != 0xFF){
    DMXOutputs[outputID][0] = uartNum;
    DMXOutputs[outputID][1] = attachedUniverse;
    DMXOutputs[outputID][2] = 0;
    PollReplyPacket.setSwOut(outputID, attachedUniverse);
    return 1;
  } else {
    return 0;
  }
}

void ArtnetnodeWifi::tickDMX(uint32_t time)
{
  msSinceDMXSend += time;
  if(msSinceDMXSend > DMX_MS_BETWEEN_TICKS){
    sendDMX();
    msSinceDMXSend = 0;
  }
}
