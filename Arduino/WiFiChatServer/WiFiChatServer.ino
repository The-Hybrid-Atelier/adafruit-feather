/*
 Simple LED  Server

 Contributors:
 * Derived from WiFi101 WiFiChat Server by David Mellis and Tom Igoe
 * Cesar Torres 
 
 A simple server that distributes any incoming messages to all
 connected clients.  To use telnet to  your device's IP address and type.
 You can see the client's input in the serial monitor as well.
 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.
 */

#include <SPI.h>
#include <WiFi101.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(443);

boolean alreadyConnected = false; // whether or not the client was connected previously
char buffer = ' ';

#define API_LIGHT_PIN 13

void apiLightBlinkLoop(int times){
  for(int i = 0; i < times; i++){
    apiLightOn();
    delay(100);
    apiLightOff();
    delay(100);
  }
}
void apiLightOn(){
  digitalWrite(API_LIGHT_PIN, HIGH);
}

void apiLightOff(){
  digitalWrite(API_LIGHT_PIN, LOW);
}

void findCommandEnd(WiFiClient client){
  buffer = ' ';
  while(buffer != '\n'){
    if(client.available() > 0){
      buffer = client.read();
    }
  }
}

void apiCall(WiFiClient client, char prefix){
  switch(prefix){
    case 'L': 
      Serial.write("LIGHT_ON CALLED\n");
      apiLightOn();
      findCommandEnd(client);
      break;
    case 'O': 
      Serial.write("LIGHT_OFF CALLED\n");
      apiLightOff();
      findCommandEnd(client);
      break;
    default:
      Serial.write("COMMAND NOT FOUND\n");
      findCommandEnd(client);
      break;
  }
  
}



void loop() {
  // wait for a new client:
  WiFiClient client = server.available();


  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clead out the input buffer:
      client.flush();
      Serial.println("We have a new client");
      client.println("Hello, client!");
      alreadyConnected = true;
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = client.read();
      // echo the bytes back to the client:
//      server.write(thisChar);
      // echo the bytes to the server as well:
//      Serial.write(thisChar);
        apiCall(client, thisChar);
        server.write("OK");
    }
  }
}


void printWiFiStatus() {
  apiLightBlinkLoop(10);
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void setup() {
  WiFi.setPins(8,7,4,2);
  //Initialize serial and wait for port to open:
//  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // start the server:
  server.begin();
  // you're connected now, so print out the status:
  printWiFiStatus();
}
