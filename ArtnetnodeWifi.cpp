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

#include <Artnetnode.h>
#include <HardwareSerial.cpp>

Artnetnode::Artnetnode() {
  Ethernet.enableLinkLed();
  Ethernet.enableActivityLed();

  // Initalise DMXOutput array
  for(int i = 0; i < DMX_MAX_OUTPUTS; i++){
    DMXOutputs[i][0] = -1;
    DMXOutputs[i][1] = -1;
    DMXOutputs[i][2] = 0;
  }

  // Start DMX tick clock
  msSinceDMXSend = 0;

  // Init DMX buffers
  for(int i = 0; i < DMX_MAX_OUTPUTS; i++){
    memset(DMXBuffer[i], 0, sizeof(DMXBuffer[i]));
  }
}

uint8_t Artnetnode::begin(byte mac[], uint8_t numOutputs){
  if(Ethernet.begin(mac)){
    Udp.begin(ARTNET_PORT);
    localIP = Ethernet.localIP();
    localMask = Ethernet.subnetMask();
    localBroadcast = IPAddress(localIP | ~localMask);

    PollReplyPacket.setMac(mac);
    PollReplyPacket.setIP(localIP);
    PollReplyPacket.canDHCP(true);
    PollReplyPacket.isDHCP(true);

    return 1;
  }
  else{
    return 0;
  }
}

uint8_t Artnetnode::setShortName(char name[]){
  PollReplyPacket.setShortName(name);
}

uint8_t Artnetnode::setLongName(char name[]){
  PollReplyPacket.setLongName(name);
}

uint8_t Artnetnode::setName(char name[]){
  PollReplyPacket.setShortName(name);
  PollReplyPacket.setLongName(name);
}

uint8_t Artnetnode::setStartingUniverse(uint16_t startingUniverse){
  PollReplyPacket.setStartingUniverse(startingUniverse);
}

uint16_t Artnetnode::read(){
  packetSize = Udp.parsePacket();

  
  if (packetSize <= ARTNET_MAX_BUFFER && packetSize > 0){ 
    Udp.read(artnetPacket, ARTNET_MAX_BUFFER);

    // Check packetID equals "Art-Net"
    for (int i = 0 ; i < 9 ; i++){
      if (artnetPacket[i] != ARTNET_ID[i])
        return 0;
    }
      
    opcode = artnetPacket[8] | artnetPacket[9] << 8; 

    if (opcode == OpDmx){
      return handleDMX();
    }
    else if (opcode == OpPoll){
    	return handlePollRequest();
    }
  }
  else{
    return 0;
  }
}

bool Artnetnode::isBroadcast(){
  if(Udp.destIP() == localBroadcast){
    return true;
  }
  else{
    return false;
  }
}

uint16_t Artnetnode::handleDMX(){
  if(isBroadcast()){
    return 0;
  }
  else{
    // Get universe
    uint16_t universe = artnetPacket[14] | artnetPacket[15] << 8;

    // Get DMX frame length
    uint16_t dmxDataLength = artnetPacket[17] | artnetPacket[16] << 8;


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

    return OpDmx;
  }
}

uint16_t Artnetnode::handlePollRequest(){
  if(isBroadcast()){
    
    Udp.beginPacket(localBroadcast, ARTNET_PORT);
    Udp.write(PollReplyPacket.printPacket(), sizeof(PollReplyPacket.packet));
    Udp.endPacket();

    return OpPoll;
  }
  else{
    return 0;
  }
}

void Artnetnode::enableDMX(){
  DMXOutputStatus = true;
}

void Artnetnode::disableDMX(){
  DMXOutputStatus = false;
}

void Artnetnode::enableDMXOutput(uint8_t outputID){
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

void Artnetnode::disableDMXOutput(uint8_t outputID){
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

uint8_t Artnetnode::setDMXOutput(uint8_t outputID, uint8_t uartNum, uint16_t attachedUniverse){
  // Validate input
  if(outputID>-1 && uartNum>-1 && attachedUniverse>-1){
    DMXOutputs[outputID][0] = uartNum;
    DMXOutputs[outputID][1] = attachedUniverse;
    DMXOutputs[outputID][2] = 0;
    PollReplyPacket.setSwOut(outputID, attachedUniverse);
    return 1;
  }
  else{
    return 0;
  }
}

void Artnetnode::tickDMX(uint32_t time){
	msSinceDMXSend += time;
  if(msSinceDMXSend > DMX_MS_BETWEEN_TICKS){
    sendDMX();
    msSinceDMXSend = 0;
  }
}

void Artnetnode::sendDMX(){
	if(DMXOutputStatus){
    for(int i = 0; i <= DMX_MAX_OUTPUTS; i++){
      if(DMXOutputs[i][2]==1){
        uartDMX(i, DMXOutputs[i][0]);
      }
    }
	}
}

void Artnetnode::uartDMX(uint8_t outputID, uint8_t uartNum){
  if(uartNum == 0){
    Serial.begin(125000);
    ROM_UARTConfigSetExpClk(UART0_BASE, F_CPU, 125000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial.write((uint8_t)0);
    Serial.flush();
    Serial.begin(250000);
    ROM_UARTConfigSetExpClk(UART0_BASE, F_CPU, 250000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial.write((uint8_t)0);
    Serial.write(DMXBuffer[outputID], 512);
  }
  else if(uartNum == 1){
    Serial1.begin(125000);
    ROM_UARTConfigSetExpClk(UART1_BASE, F_CPU, 125000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial1.write((uint8_t)0);
    Serial1.flush();
    Serial1.begin(250000);
    ROM_UARTConfigSetExpClk(UART1_BASE, F_CPU, 250000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial1.write((uint8_t)0);
    Serial1.write(DMXBuffer[outputID], 512);
  }
  else if(uartNum == 2){
    Serial2.begin(125000);
    ROM_UARTConfigSetExpClk(UART2_BASE, F_CPU, 125000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial2.write((uint8_t)0);
    Serial2.flush();
    Serial2.begin(250000);
    ROM_UARTConfigSetExpClk(UART2_BASE, F_CPU, 250000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial2.write((uint8_t)0);
    Serial2.write(DMXBuffer[outputID], 512);
  }
  else if(uartNum == 3){
    Serial3.begin(125000);
    ROM_UARTConfigSetExpClk(UART3_BASE, F_CPU, 125000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial3.write((uint8_t)0);
    Serial3.flush();
    Serial3.begin(250000);
    ROM_UARTConfigSetExpClk(UART3_BASE, F_CPU, 250000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial3.write((uint8_t)0);
    Serial3.write(DMXBuffer[outputID], 512);
  }
  else if(uartNum == 4){
    Serial4.begin(125000);
    ROM_UARTConfigSetExpClk(UART4_BASE, F_CPU, 125000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial4.write((uint8_t)0);
    Serial4.flush();
    Serial4.begin(250000);
    ROM_UARTConfigSetExpClk(UART4_BASE, F_CPU, 250000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial4.write((uint8_t)0);
    Serial4.write(DMXBuffer[outputID], 512);
  }
  else if(uartNum == 5){
    Serial5.begin(125000);
    ROM_UARTConfigSetExpClk(UART5_BASE, F_CPU, 125000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial5.write((uint8_t)0);
    Serial5.flush();
    Serial5.begin(250000);
    ROM_UARTConfigSetExpClk(UART5_BASE, F_CPU, 250000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial5.write((uint8_t)0);
    Serial5.write(DMXBuffer[outputID], 512);
  }
  else if(uartNum == 6){
    Serial6.begin(125000);
    ROM_UARTConfigSetExpClk(UART6_BASE, F_CPU, 125000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial6.write((uint8_t)0);
    Serial6.flush();
    Serial6.begin(250000);
    ROM_UARTConfigSetExpClk(UART6_BASE, F_CPU, 250000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial6.write((uint8_t)0);
    Serial6.write(DMXBuffer[outputID], 512);
  }
  else if(uartNum == 7){
    Serial7.begin(125000);
    ROM_UARTConfigSetExpClk(UART7_BASE, F_CPU, 125000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial7.write((uint8_t)0);
    Serial7.flush();
    Serial7.begin(250000);
    ROM_UARTConfigSetExpClk(UART7_BASE, F_CPU, 250000,
                              (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_TWO |
                              UART_CONFIG_WLEN_8));
    Serial7.write((uint8_t)0);
    Serial7.write(DMXBuffer[outputID], 512);
  }
}