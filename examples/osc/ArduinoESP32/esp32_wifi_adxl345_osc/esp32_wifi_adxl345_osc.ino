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

WiFiUDP Udp;

// requires Adafruit ADXL345 library
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// Assign a unique ID to this sensor at the same time
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

int _ticks = 0;

void setup() {
  
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) delay(200);  
  Serial.println("WiFi connected");
  
  // initialise the ADXL345 accelerometer
  accel.begin();
  accel.setRange(ADXL345_RANGE_16_G);
}

void loop() {

  // send OSC messages
  int ticks = millis()/50;
  if (ticks != _ticks) {
    _ticks = ticks;

    // get a new sensor event
    sensors_event_t event; 
    accel.getEvent(&event);

    // get acceleration valus in m/s^2
    // value range: -100.0 ~ +100.0
    float fx = event.acceleration.x;
    float fy = event.acceleration.y;
    float fz = event.acceleration.z;

    // value range: 0 ~ 20000
    int x = constrain(fx*100, -10000, 10000) + 10000;
    int y = constrain(fy*100, -10000, 10000) + 10000;
    int z = constrain(fz*100, -10000, 10000) + 10000;

    OSCMessage msgOut("/value");
    msgOut.add((int32_t)x);
    msgOut.add((int32_t)y);
    msgOut.add((int32_t)z);

    Udp.beginPacket(remoteIP, remotePort);
    msgOut.send(Udp);
    Udp.endPacket();
    msgOut.empty();
  }
}
