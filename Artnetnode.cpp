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
    for(int y = 0; y < DMX_MAX_BUFFER; y++){
      DMXBuffer[i][y] = 0;
    }
  }
}

uint8_t Artnetnode::begin(byte mac[], uint8_t numOutputs){
  if(Ethernet.begin(mac)){
	  Udp.begin(ARTNET_PORT);
	  return 1;
	}
	else{
		return 0;
	}
}

uint8_t Artnetnode::setShortName(char name[]){
	// Check name is short enough
	int nameLength = sizeof(name);
	if(nameLength<=ARTNET_SHORT_NAME_MAX_LENGTH && nameLength > 0){
		for(int i = 0; i<ARTNET_SHORT_NAME_MAX_LENGTH+1; i++){
			if(i<nameLength){
				shortName[i] = name[i];
			}
			else{
				shortName[i] = '\0';
			}
		}
		return 1;
	}
	else{
		return 0;
	}
}

uint8_t Artnetnode::setLongName(char name[]){
	// Check name is short enough
	int nameLength = sizeof(name);
	if(nameLength<=ARTNET_LONG_NAME_MAX_LENGTH && nameLength > 0){
		for(int i = 0; i<ARTNET_LONG_NAME_MAX_LENGTH+1; i++){
			if(i<nameLength){
				longName[i] = name[i];
			}
			else{
				longName[i] = '\0';
			}
		}
		return 1;
	}
	else{
		return 0;
	}
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

uint16_t Artnetnode::handleDMX(){
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

uint16_t Artnetnode::handlePollRequest(){
	uint8_t artnetPacketReply[233];

  for (byte i = 0 ; i < 244 ; i++)
  {
    artnetPacketReply[i] = '\0';
  }

  // Start with id
  for (byte i = 0 ; i < 9 ; i++)
  {
    artnetPacketReply[i] = ARTNET_ID[i];
  }
  
  // Add opcode
  artnetPacketReply[8] = 0x2100 & 0xFF;
  artnetPacketReply[9] = 0x2100 >> 8;
  
  // Add IP
  artnetPacketReply[10] = 192;
  artnetPacketReply[11] = 168;
  artnetPacketReply[12] = 1;
  artnetPacketReply[13] = 150;
  
  // Add Port
  artnetPacketReply[14] =  0x1936 & 0xFF;
  artnetPacketReply[15] =  0x1936 >> 8;
  
  // HW Versioning
  artnetPacketReply[16] =  0;
  artnetPacketReply[17] =  1;
  
  // Netswitch and sub switch
  artnetPacketReply[18] =  0; // FIXME
  artnetPacketReply[19] =  0; // FIXME
  
  // OEm
  artnetPacketReply[20] = 0x0190 & 0xFF;
  artnetPacketReply[21] = 0x0190 >> 8;
  
  // ubea
  artnetPacketReply[22] = 0;
  
  // Status 1
  artnetPacketReply[23] = 0b00000000;
  
  // Esta Man codes
  artnetPacketReply[24] = 0;
  artnetPacketReply[25] = 0;
  
  // Short name
  artnetPacketReply[26] = 'E';
  artnetPacketReply[27] = 'l';
  artnetPacketReply[28] = 'e';
  artnetPacketReply[29] = 'c';
  artnetPacketReply[30] = 't';
  artnetPacketReply[31] = 'r';
  artnetPacketReply[32] = 'i';
  artnetPacketReply[33] = 'c';
  artnetPacketReply[34] = 'L';
  artnetPacketReply[35] = 'a';
  artnetPacketReply[36] = 'b';
  artnetPacketReply[37] = 'A';
  artnetPacketReply[38] = 'r';
  artnetPacketReply[39] = 't';
  artnetPacketReply[40] = 'N';
  artnetPacketReply[41] = '\0';
  artnetPacketReply[42] = '\0';
  artnetPacketReply[43] = '\0';
  
  // Long name
  artnetPacketReply[44] = 'E';
  artnetPacketReply[45] = 'l';
  artnetPacketReply[46] = 'e';
  artnetPacketReply[47] = 'c';
  artnetPacketReply[48] = 't';
  artnetPacketReply[49] = 'r';
  artnetPacketReply[50] = 'i';
  artnetPacketReply[51] = 'c';
  artnetPacketReply[52] = 'L';
  artnetPacketReply[53] = 'a';
  artnetPacketReply[54] = 'b';
  artnetPacketReply[55] = 'A';
  artnetPacketReply[56] = 'r';
  artnetPacketReply[57] = 't';
  artnetPacketReply[58] = 'N';
  artnetPacketReply[59] = 'e';
  artnetPacketReply[60] = 't';
  artnetPacketReply[61] = '\0';
  //artnetPacketReply[62-107] = '\0';
  
  // Node Report
  //artnetPacketReply[108-171] = '\0';
  
  // NumPorts
  artnetPacketReply[172] = 0;
  artnetPacketReply[173] = 1;
  
  // Port types
  artnetPacketReply[174] = 0b10000000;
  artnetPacketReply[175] = 0b00000000;
  artnetPacketReply[176] = 0b00000000;
  artnetPacketReply[177] = 0b00000000;
  
  // GoodInput
  artnetPacketReply[178] = 0b10000000;
  artnetPacketReply[179] = 0b00000000;
  artnetPacketReply[180] = 0b00000000;
  artnetPacketReply[181] = 0b00000000;
  
  // GoodOutput
  artnetPacketReply[182] = 0b10000000;
  artnetPacketReply[183] = 0b00000000;
  artnetPacketReply[184] = 0b00000000;
  artnetPacketReply[185] = 0b00000000;
  
  // SWin
  artnetPacketReply[186] = 0;
  artnetPacketReply[187] = 0;
  artnetPacketReply[188] = 0;
  artnetPacketReply[189] = 0;
  
  // SWout
  artnetPacketReply[190] = 0;
  artnetPacketReply[191] = 0;
  artnetPacketReply[192] = 0;
  artnetPacketReply[193] = 0; 
  
  // SwVideo
  artnetPacketReply[194] = 0;
  
  // SwMacro
  artnetPacketReply[195] = 0;
  
  // SwRemote
  artnetPacketReply[196] = 0;
  
  // Spare channels
  artnetPacketReply[197] = 0;
  artnetPacketReply[198] = 0;
  artnetPacketReply[199] = 0;
  
  // Style
  artnetPacketReply[200] = 0;
  
  // Mac address
  artnetPacketReply[201] = 0x35;
  artnetPacketReply[202] = 0xD0;
  artnetPacketReply[203] = 0x02;
  artnetPacketReply[204] = 0xB6;
  artnetPacketReply[205] = 0x1A;
  artnetPacketReply[206] = 0x00;
  
  // BindIp
  artnetPacketReply[207] = 0;
  artnetPacketReply[208] = 0;
  artnetPacketReply[209] = 0;
  artnetPacketReply[210] = 0;
  
  // BindIndex
  artnetPacketReply[211] = 0;
  
  // Status 2
  artnetPacketReply[212] = 0;
  
  // Filler
  //artnetPacketReply[209-234] = 0;
  
  Udp.beginPacket({192,168,1,255}, 6454);
  Udp.write(artnetPacketReply, sizeof(artnetPacketReply));
  Udp.endPacket();

  return OpPoll; 
}

void Artnetnode::enableDMX(){
  DMXOutputStatus = true;
}

void Artnetnode::disableDMX(){
  DMXOutputStatus = false;
}

void Artnetnode::enableDMXOutput(uint8_t outputID){
  DMXOutputs[outputID][2] = 1;
}

void Artnetnode::disableDMXOutput(uint8_t outputID){
  DMXOutputs[outputID][2] = 0;
}

uint8_t Artnetnode::setDMXOutput(uint8_t outputID, uint8_t uartNum, uint16_t attachedUniverse){
	// Validate input
	if(outputID>-1 && uartNum>-1 && attachedUniverse>-1){
		DMXOutputs[outputID][0] = uartNum;
		DMXOutputs[outputID][1] = attachedUniverse;
		DMXOutputs[outputID][2] = 0;
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