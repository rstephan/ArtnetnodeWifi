/*
Example Artnet RGB LED, 3 LED (one per color) react on ArtNet messages from a master.
Charles Yarnold 2015 - charlesyarnold@gmail.com

Stephan Ruloff 2016
https://github.com/rstephan
*/
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArtnetnodeWifi.h>
#include <Ticker.h>

#define DMX_MAX 512 // max. number of DMX data packages.
uint8_t DMXBuffer[DMX_MAX];

//Wifi settings
const char* ssid = "ssid";
const char* password = "pAsSwOrD";

Ticker ticker;
WiFiUDP UdpSend;
ArtnetnodeWifi artnetnode;

char udpBeatPacket[70];
int pinR = 15;
int pinG = 12;
int pinB = 13;


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
  if (state){
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

void beat()
{

  UdpSend.beginPacket({192,168,0,100},33333);
  UdpSend.write(udpBeatPacket, sizeof(udpBeatPacket)-1);
  //Udp.write("{\"mac\":\"f6:8b:d9:c2:9a:69\",\"ip\":\"192.168.1.75\",\"voltage\":579}");
  UdpSend.endPacket();
}

void setup()
{
  Serial.begin(115200);
  artnetnode.setName("ESP8266 - Artnet");
  artnetnode.setStartingUniverse(1);
  
  ConnectWifi();

#if 0
  while(artnetnode.begin(ssid, password, 1) == false){
    Serial.print("X");
  }
#endif
  artnetnode.begin();

  Serial.println();
  Serial.println("Connected");

  analogWriteRange(255);
  
  artnetnode.setDMXOutput(0,1,0);

  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  analogWrite(pinR, 0);
  analogWrite(pinG, 0);
  analogWrite(pinB, 0);

  UdpSend.begin(4000);
  IPAddress localIP = WiFi.localIP();
  
  uint8_t mac[6];
  WiFi.macAddress(mac);
    
  sprintf(udpBeatPacket, "{\"mac\":\"%x:%x:%x:%x:%x:%x\",\"ip\":\"%d.%d.%d.%d\",\"voltage\":1}", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], localIP[0],  localIP[1],  localIP[2],  localIP[3]);
  //Serial.println();
  //Serial.println(udpBeatPacket);
  ticker.attach(5, beat);
  beat();
}

void loop()
{ 
  uint16_t code = artnetnode.read();
  if (code) {
    if (code == OpDmx)
    {
      //Serial.print("D");
      //analogWrite(pinR, artnetnode.returnDMXValue(0, 1));
      //analogWrite(pinG, artnetnode.returnDMXValue(0, 2));
      //analogWrite(pinB, artnetnode.returnDMXValue(0, 3));
    }
    else if (code == OpPoll) {
      Serial.println("Art Poll Packet");
    }
  }
  if (WiFi.status() == 6){
    ESP.reset();
  }
}
