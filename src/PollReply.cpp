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

// Class for saving details to and for constructing pollreply packets

#include <PollReply.h>

PollReply::PollReply(){
  // Zero out vars
  memset(packet.IPAddr, 0, sizeof(packet.IPAddr));
  memset(packet.NodeReport, 0, sizeof(packet.NodeReport));
  memset(packet.PortTypes, 0, sizeof(packet.PortTypes));
  memset(packet.GoodInput, 0, sizeof(packet.GoodInput));
  memset(packet.GoodOutput, 0, sizeof(packet.GoodOutput));
  memset(packet.SwIn, 0, sizeof(packet.SwIn));
  memset(packet.SwOut, 0, sizeof(packet.SwOut));
  memset(packet.Filler, 0, sizeof(packet.Filler));
  setStartingUniverse(0);
}

void PollReply::setMac(byte mac[]){
  packet.Mac[0] = mac[0];
  packet.Mac[1] = mac[1];
  packet.Mac[2] = mac[2];
  packet.Mac[3] = mac[3];
  packet.Mac[4] = mac[4];
  packet.Mac[5] = mac[5];
}

void PollReply::setIP(IPAddress IP){
  packet.IPAddr[0] = IP[0];
  packet.IPAddr[1] = IP[1];
  packet.IPAddr[2] = IP[2];
  packet.IPAddr[3] = IP[3];
}

void PollReply::setShortName(const char name[]){
  int shortNameLen = sizeof(packet.ShortName);

  memset(packet.ShortName, 0, shortNameLen);

  shortNameLen--;
  for(int i = 0; i < shortNameLen && name[i] != 0; i++){
    packet.ShortName[i] = name[i];
  }
}

void PollReply::setLongName(const char name[]){
  int longNameLen = sizeof(packet.LongName);

  memset(packet.LongName, 0, longNameLen);

  longNameLen--;
  for(int i = 0; i < longNameLen && name[i] != 0; i++){
    packet.LongName[i] = name[i];
  }
}

void PollReply::canDHCP(bool can){
  if(can){
    packet.Status2 = packet.Status2 | 0b00100000;
  }
  else{
    packet.Status2 = packet.Status2 & (~0b00100000);
  }
}

void PollReply::isDHCP(bool is){
  if(is){
    packet.Status2 = packet.Status2 | 0b01000000;
  }
  else{
    packet.Status2 = packet.Status2 & (~0b01000000);
  }
}

void PollReply::setNumPorts(uint8_t num){
  packet.NumPortsLo = num;
}

void PollReply::setSwOut(uint8_t port, uint16_t universe){
  if(port < 4){
    packet.SwOut[port] = universe & 0b0000000000001111;
  }
}

void PollReply::setOutputEnabled(uint8_t port){
  if(port < 4){
    packet.PortTypes[port] = packet.PortTypes[port] | 0b10000000;
    packet.GoodOutput[port] = packet.GoodOutput[port] | 0b10000000;
    setSwOut(port, startingUniverse + port);
  }
}

void PollReply::setOutputDisabled(uint8_t port){
  if(port < 4){
    packet.PortTypes[port] = packet.PortTypes[port] & (~0b10000000);
    packet.GoodOutput[port] = packet.GoodOutput[port] & (~0b10000000);
    setSwOut(port, 0);
  }
}

void PollReply::setStartingUniverse(uint16_t startUniverse){
  startingUniverse = startUniverse;
  packet.NetSwitch = startUniverse >> 8;
  packet.SubSwitch = (startUniverse & 0b000000011111111) >> 4;
}

uint8_t* PollReply::printPacket(){
  return (uint8_t *)&packet;
}
