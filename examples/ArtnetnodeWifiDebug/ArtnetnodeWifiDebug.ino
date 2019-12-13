/*
Example, transmit all received ArtNet messages (DMX) out of the serial port in plain text.

Stephan Ruloff 2016-2019
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
    Serial.println(IPAddress(WiFi.localIP()));
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  boolean tail = false;
  
  Serial.print("DMX: Univ: ");
  Serial.print(universe, DEC);
  Serial.print(", Seq: ");
  Serial.print(sequence, DEC);
  Serial.print(", Data (");
  Serial.print(length, DEC);
  Serial.print("): ");
  
  if (length > 16) {
    length = 16;
    tail = true;
  }
  // send out the buffer
  for (int i = 0; i < length; i++)
  {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  if (tail) {
    Serial.print("...");
  }
  Serial.println();
}

void setup()
{
  // set-up serial for debug output
  Serial.begin(115200);
  ConnectWifi();
  // max. 17 characters
  artnetnode.setShortName("ESP-Art-Net Debug");
  // max. 63 characters
  artnetnode.setLongName("ESP8266 - Art-Net (debug), the long version");
  // set a starting universe if you wish, defaults to 0
  //artnetnode.setStartingUniverse(4);
  artnetnode.setNumPorts(4);
  artnetnode.enableDMXOutput(0);
  artnetnode.enableDMXOutput(1);
  artnetnode.enableDMXOutput(2);
  artnetnode.enableDMXOutput(3);
  artnetnode.begin();

  // this will be called for each packet received
  artnetnode.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnetnode.read();
}
