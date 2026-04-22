// requires OSC library by Adrian Freed
#include <OSCMessage.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi settings
const char* ssid = "CMCC-esy9";       // WiFi SSID
const char* password = "tbet7732";    // WiFi password

// remote IP settings
IPAddress remoteIP(192,168,1,3);      // IP address of the receiver
const unsigned int remotePort = 8000; // Port of the receiver

// local IP settings
IPAddress localIP(192,168,1,20);      // Static IP address
IPAddress gateway(192,168,1,1);       // Gateway IP address
IPAddress subnet(255,255,255,0);      // Subnet mask
const unsigned int localPort = 9000;

WiFiUDP Udp;

int _ticks = 0;

void setup() {
  
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  WiFi.config(localIP, gateway, subnet);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) delay(200);  
  Serial.println("WiFi connected");
  
  Udp.begin(localPort);
}

void loop() {

  // send OSC messages
  int ticks = millis()/50;
  if (ticks != _ticks) {
    _ticks = ticks;

    unsigned int val1 = analogRead(A0);
    unsigned int val2 = analogRead(A1);
    
    OSCMessage msgOut("/value");
    msgOut.add((int32_t)val1);
    msgOut.add((int32_t)val2);
    
    Udp.beginPacket(remoteIP, remotePort);
    msgOut.send(Udp);
    Udp.endPacket();
    msgOut.empty();
  }

  // receive OSC messages  
  OSCMessage msgIn;
  int size = Udp.parsePacket();
  if (size > 0) {
    while (size--) {
      msgIn.fill(Udp.read());
    }
    if (!msgIn.hasError()) {
      Serial.println("Received OSC message");
      
      // list OSC message contents
      listMessage(msgIn);

      // control LED according to the message
      processMessage(msgIn);      
    }
  }
}

void listMessage(OSCMessage &msg) {
  Serial.print("OSC Address: ");
  char address[50];
  msg.getAddress(address, 0, 50);
  Serial.println(address);

  for (int i = 0; i < msg.size(); i++) {
    if (msg.isInt(i)) {
      Serial.print("integer: ");
      Serial.println(msg.getInt(i));
    } else if (msg.isFloat(i)) {
      Serial.print("float: ");
      Serial.println(msg.getFloat(i));
    } else if (msg.isBoolean(i)) {
      Serial.print("boolean: ");
      Serial.println(msg.getBoolean(i));
    } else if (msg.isString(i)) {
      char buffer[100];
      msg.getString(i, buffer, 100);
      Serial.print("string: ");
      Serial.println(buffer);
    }
  }
}

void processMessage(OSCMessage &msg) {
  char address[50];
  msg.getAddress(address, 0, 50);

  if (strcmp(address, "/led")==0 && msg.isBoolean(0)) {
    bool state = msg.getBoolean(0);
    digitalWrite(LED_BUILTIN, state);
  }
}
