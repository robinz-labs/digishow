// Requires OSC library by Adrian Freed
#include <OSCMessage.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Define your static IP settings
IPAddress localIP(192,168,1,20); // Desired static IP address
IPAddress gateway(192,168,1,1);  // Gateway IP address
IPAddress subnet(255,255,255,0); // Subnet mask

const char* ssid = "CMCC-esy9";     // WiFi SSID
const char* password = "tbet7732";  // WiFi password

WiFiUDP Udp;
const unsigned int localPort = 9000;

IPAddress remoteIP(192,168,1,10);     // IP address of the receiver
const unsigned int remotePort = 8000; // Port of the receiver

int lastSeconds = 0;

void setup() {
  
  Serial.begin(115200);

  WiFi.config(localIP, gateway, subnet);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) delay(200);  
  Serial.println("WiFi connected");
  
  Udp.begin(localPort);
}

void loop() {

  // Send OSC messages
  int seconds = (millis()/1000) % 60;

  if (seconds != lastSeconds) {
    lastSeconds = seconds;
    
    OSCMessage msgOut("/seconds");
    msgOut.add((int32_t)seconds);
    
    Udp.beginPacket(remoteIP, remotePort);
    msgOut.send(Udp);
    Udp.endPacket();
    msgOut.empty();
  }

  // Receive OSC messages  
  OSCMessage msgIn;
  int size = Udp.parsePacket();
  if (size > 0) {
    while (size--) {
      msgIn.fill(Udp.read());
    }
    if (!msgIn.hasError()) {
      Serial.println("Received OSC message");
      
      // Process the OSC message here
      listMessage(msgIn);

      // Give feedback on received messages
      OSCMessage &msg = msgIn;
      Udp.beginPacket(remoteIP, remotePort);
      msg.send(Udp);
      Udp.endPacket();
      msg.empty();
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
