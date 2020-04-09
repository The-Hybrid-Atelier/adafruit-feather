/*
  Simple WebSocket client for ArduinoHttpClient library
  Sends JSON responses and accepts JSON commands.
  Sends data from amplifier.
  
  created 3 Apr 2020
  by Cesar Torres, Hedieh Moradi modified from Sandeep Mistry, Tom Igoe
*/

#include <ArduinoHttpClient.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

#define SERVER_ADDRESS "192.168.1.252"
#define SERVER_PORT 3001

/* MIC_API
 *  
 */
#define MIC_PIN A4
#define DATA_SIZE 16 // the sample rate; power of 2

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
/////// Wifi Settings ///////
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

char serverAddress[] = SERVER_ADDRESS;  // server address
int port = SERVER_PORT;

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

  pinMode(MIC_PIN, INPUT);
  
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
  doc["name"] = "audio-streamer";
  doc["version"] = "1.0"; 
  doc["event"] = "connected"; 
  // Registering with Server
  client.beginMessage(TYPE_TEXT);
  //    client.print(output);
  serializeJson(doc, client);
  client.endMessage();
}

DynamicJsonDocument mic_response(4096);
bool capture_on = false;
void start_capture(){
  capture_on = true;
}
void end_capture(){
  capture_on = false;
}
void capture(){
  mic_response["sensor"] = "mic";
  mic_response["time"] = millis();
  JsonArray data = mic_response.createNestedArray("data");
  Serial.println(JSON_ARRAY_SIZE(DATA_SIZE) + JSON_OBJECT_SIZE(3));
  for(int i = 0; i< DATA_SIZE; i++){
    data.add(analogRead(MIC_PIN));
  }
  client.beginMessage(TYPE_TEXT);
  serializeJson(mic_response, client);
  client.endMessage();
  mic_response.clear();
}
/* END OF MIC_API */

// API REGISTRATION
void apiCall(char prefix, JsonObject obj){
  switch(prefix){
    case 'O': 
      Serial.write("MIC_CAPTURE ON\n");
      start_capture();
      break;
    case 'F': 
      Serial.write("MIC_CAPTURE OFF\n");
      end_capture();
      break;
    default:
      Serial.write("COMMAND NOT FOUND\n");
      break;
  }
}

void api_run(){
  if(capture_on){capture();}
}

DynamicJsonDocument json(2048);
void wifi_connect(){
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
    api_run();
  }
  Serial.println("Server disconnected.");
}

void loop() {
  wifi_connect();
}
