/*
Example Artnet RGB LED, 3 LED (one per color) react on ArtNet messages from a master.
Charles Yarnold 2015 - charlesyarnold@gmail.com
https://github.com/solexious/ESP8266_artnet_led_node

Stephan Ruloff 2016-2019
https://github.com/rstephan
*/
#include <ArtnetnodeWifi.h>

//Wifi settings
const char* ssid = "ssid";
const char* password = "pAsSwOrD";

ArtnetnodeWifi artnetnode;

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

void setup()
{
  Serial.begin(115200);
  artnetnode.setName("ESP8266 - Artnet");
  artnetnode.setNumPorts(1);
  artnetnode.enableDMXOutput(0);
  artnetnode.setStartingUniverse(1);

  ConnectWifi();

  artnetnode.begin();

  Serial.println();
  Serial.println("Connected");

#if defined(ARDUINO_ARCH_ESP8266)
  analogWriteRange(255);
#endif

  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  analogWrite(pinR, 0);
  analogWrite(pinG, 0);
  analogWrite(pinB, 0);
}

void loop()
{
  uint16_t code = artnetnode.read();
  if (code) {
    if (code == OpDmx) {
      //Serial.print("D");
      analogWrite(pinR, artnetnode.getDmxFrame()[0]);
      analogWrite(pinG, artnetnode.getDmxFrame()[1]);
      analogWrite(pinB, artnetnode.getDmxFrame()[2]);
    }
    else if (code == OpPoll) {
      Serial.println("Art Poll Packet");
    }
  }
#if defined(ARDUINO_ARCH_ESP8266)
  if (WiFi.status() == 6) {
    ESP.reset();
  }
#endif
  yield();
}
