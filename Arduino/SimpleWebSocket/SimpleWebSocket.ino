/*
  Simple WebSocket client for ArduinoHttpClient library

  Sends JSON responses and accepts JSON commands.
  
  created 3 Apr 2020
  by Cesar Torres modified from Sandeep Mistry, Tom Igoe
*/
#include <ArduinoHttpClient.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

#define API_LIGHT_PIN 9
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
/////// Wifi Settings ///////
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char serverAddress[] = "192.168.1.252";  // server address
int port = 3001;

WiFiClient wifi;
WebSocketClient client = WebSocketClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;
int count = 0;
char output[128];
DynamicJsonDocument doc(1024);
IPAddress ip;

void setup() {
  WiFi.setPins(8,7,4,2);
  Serial.begin(9600);
  Serial.println("Websocket Client: Smart Grinder");

  pinMode(API_LIGHT_PIN, OUTPUT);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println("Starting JSONWebSocket Client");
  
}


/* A Hello World Message that communicates a human-readable description
 * of this service's identity.
 * 
 */
 
void greet(){
  doc["name"] = "led-switcher";
  doc["version"] = "1.0"; 
  doc["event"] = "connected"; 
  // Registering with Server
  client.beginMessage(TYPE_TEXT);
  //    client.print(output);
  serializeJson(doc, client);
  client.endMessage();
}

/* LIGHT_API
 *  
 */


void apiLightBlinkLoop(int times, int delayTime){
  
  for(int i = 0; i < times; i++){
    apiLightOn();
    delay(delayTime);
    apiLightOff();
    delay(delayTime);
  }
}
void apiLightOn(){
  analogWrite(API_LIGHT_PIN, 255);
}

void apiLightOff(){
  analogWrite(API_LIGHT_PIN, 0);
}

int fadeAmount;

void apiLightFade(int delayTime){
  fadeAmount = 5;
  for (int i = 0; i < 255; i += fadeAmount){
    analogWrite(API_LIGHT_PIN, i);
    delay(delayTime);
  }   
}

/* END OF LIGHT_API */

int numOfBlinks;
int delayTime;

// API REGISTRATION
void apiCall(char prefix, JsonObject obj){
  switch(prefix){
    case 'L': 
      Serial.write("LIGHT_ON CALLED\n");
      apiLightOn();
      break;
    case 'O': 
      Serial.write("LIGHT_OFF CALLED\n");
      apiLightOff();
      break;
    case 'B':
      Serial.write("BLINK CALLED\n");
      numOfBlinks = obj[String("numOfBlinks")];
      delayTime = obj[String("delayTime")];
      Serial.println(numOfBlinks);
      Serial.println(delayTime);
      apiLightBlinkLoop(numOfBlinks, delayTime);
      break;
    case 'F':
      Serial.write("FADE CALLED\n");
      delayTime = obj[String("delayTime")];
      Serial.println(delayTime);
      apiLightFade(delayTime);
      break;
    
    default:
      Serial.write("COMMAND NOT FOUND\n");
      break;
  }
}

DynamicJsonDocument json(2048);
void loop() {
  while(! client.connected()){
    Serial.print("\tAttempting connection to ");
    Serial.print(serverAddress);
    Serial.print(" from ");
    Serial.println(ip);
    client.begin();
    greet();
  }
  
  while (client.connected()) {
    // check if a message is available to be received
    int messageSize = client.parseMessage();

    if (messageSize > 0) {
      Serial.println("Received a message:");
      
      String response = client.readString();
      Serial.println(response);
      deserializeJson(json, response);
      JsonObject obj = json.as<JsonObject>();
      
      if(obj.containsKey("api")){
        obj = obj[String("api")];
        if(obj.containsKey("command")){
          Serial.print("API CALL: ");
          String command = obj[String("command")];
          Serial.println(command);
          obj = obj[String("params")];
          apiCall(command[0], obj);
        }
      }
      json.clear();
    }

    // wait 5 seconds
    // delay(1000);
  }
  
  Serial.println("Server disconnected.");
}
