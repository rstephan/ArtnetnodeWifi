/*
Example Send DMX, transmit all received ArtNet messages out of the serial port.

@author Stephan Ruloff
@date 2016-2019
https://github.com/rstephan

*/
#include <ArtnetnodeWifi.h>

//Wifi settings
const char* ssid = "ssid";
const char* password = "pAsSwOrD";

WiFiUDP UdpSend;
ArtnetnodeWifi artnetnode;

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

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  // Send "break" as a "slow" zero.
  Serial.begin(56700);
  Serial.write((uint8_t)0);
  delayMicroseconds(300);
  Serial.begin(250000, SERIAL_8N2);
  
  Serial.write((uint8_t)0); // Start-Byte
  // send out the buffer
  for (int i = 0; i < length; i++)
  {
    Serial.write(data[i]);
  }
}

void setup()
{
  // set-up serial for initial info output, hopefully DMX gear will not be confused.
  Serial.begin(115200);
  ConnectWifi();
  artnetnode.setName("ESP8266 - Art-Net");
  artnetnode.setNumPorts(1);
  artnetnode.enableDMXOutput(0);
  artnetnode.begin();

  // this will be called for each packet received
  artnetnode.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnetnode.read();
}
