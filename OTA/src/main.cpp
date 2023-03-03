/*
  Rui Santos
  Complete project details
   - Arduino IDE: https://RandomNerdTutorials.com/esp32-ota-over-the-air-arduino/
   - VS Code: https://RandomNerdTutorials.com/esp32-ota-over-the-air-vs-code/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
// Import required libraries
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>
#include <WiFiSettings.h>




#include <OneWire.h>
#include <DallasTemperature.h>
#include <MQTT.h>
#define Sprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); String r = s; free(s); r; })

const int buttonpin  = 0;
const int onewirepin = 4;
OneWire ds(onewirepin);
DallasTemperature sensors(&ds);
WiFiClient wificlient;
MQTTClient mqtt;

int    num_sensors;
String topic;
bool   publish_all;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Set number of outputs
#define NUM_OUTPUTS  4

// Assign each GPIO to an output
int outputGPIOs[NUM_OUTPUTS] = {2, 4, 12, 14};

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  char apName[11];
  uint32_t chipId = 0;
  //ESP32 only
  for(int i=0; i<17; i=i+8) {
	  chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
  sprintf(apName, "ESP-%06X", chipId);
  WiFiSettings.hostname = apName;

String server = WiFiSettings.string("mqtt_server", 64, "test.mosquitto.org");
    int port      = WiFiSettings.integer("mqtt_port", 0, 65535, 1883);
    topic         = WiFiSettings.string("mqtt_topic", "thermo");
    num_sensors   = WiFiSettings.integer("num_sensors", 1);
    publish_all   = WiFiSettings.checkbox("publish_all", true);

    // for (int i = 0; i < 1000; i++) {
    //     if (!digitalRead(onewirepin)) WiFiSettings.portal();
    //     delay(1);
    // }


  WiFiSettings.connect();
  while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(10);
    }
  Serial.println(WiFi.localIP());
  
    mqtt.begin(server.c_str(), port, wificlient);
}

String getOutputStates(){
  JSONVar myArray;
  for (int i =0; i<NUM_OUTPUTS; i++){
    myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
    myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
  }
  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) {
      notifyClients(getOutputStates());
    }
    else{
      int gpio = atoi((char*)data);
      digitalWrite(gpio, !digitalRead(gpio));
      notifyClients(getOutputStates());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
 sensors.begin();
  // Set GPIOs as outputs
  for (int i =0; i<NUM_OUTPUTS; i++){
    pinMode(outputGPIOs[i], OUTPUT);
  }
  initSPIFFS();
  initWiFi();
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html",false);
  });

  server.serveStatic("/", SPIFFS, "/");

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);
  
  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
   while (!mqtt.connected()) {
        if (!mqtt.connect("")) delay(500);
    }
    sensors.requestTemperatures();
    String all = "";
    for (int i = 0; i < num_sensors; i++) {
        float C = sensors.getTempCByIndex(i);
        Serial.printf("%d: %.2f\n", i, C);
        mqtt.publish(topic + "/" + i, Sprintf("%.2f", C));
        if (all.length()) all += ";";
        all += Sprintf("%.2f", C);
        delay(100);
    }
    if (publish_all) mqtt.publish(topic + "/all", all);

    delay(1000 - num_sensors * 100);
}