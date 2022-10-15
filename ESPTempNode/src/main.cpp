#include <Arduino.h>


// Temp sensor include
#include <OneWire.h>
#include <DallasTemperature.h>

// Wifi include
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define SLEEPTIME 400e6

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Update these with values suitable for your network.

#define WIFI_SSID "SomeSSID";
#define WIFI_PASS "SOMEPASS";
#define WIFI_CHANELL 9; // For faster connection
const char *mqtt_server = "192.168.1.181";
// const char *deviceID = "temperature/kitchen_window";
// const char *deviceName = "ESP_Temperature_kitchen_Window";
// const char *deviceID = "temperature/kitchen_couch";
// const char *deviceName = "ESP_Temperature_kitchen_couch";
//const char *deviceID = "temperature/work_window";
//const char *deviceName = "ESP_Temperature_Work_Window";
const char *deviceID = "temperature/work_closet";
const char *deviceName = "ESP_Temperature_Work_closet";

WiFiClient espClient;
PubSubClient client(espClient);
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];


void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
    #ifdef CONFIGURE_WIFI
        WiFi.persistent(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_CHANELL);
    #endif

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(100);
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
  {
    // digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
    //  but actually the LED is on; this is because
    //  it is active low on the ESP-01)
  }
  else
  {
    // digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = deviceName;
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
      
      client.publish("outTopic", deviceID);
      client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}
void publishMessage(float temp)
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  snprintf(msg, MSG_BUFFER_SIZE, "%2.2f", temp);
  Serial.print("Publish message: ");
  Serial.println(temp);
  client.publish(deviceID, msg);
  //client.publish("outTopic", msg);
}

float getTemperature()
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);
  }
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
  return tempC;
}
void setup()
{
  // put your setup code here, to run once:
  // Start up the library
  Serial.begin(115200);
  Serial.println("Initializing sensor");
  sensors.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Perform measurement
  float t = getTemperature();

  publishMessage(t);

  // Go to deep sleep
  Serial.println("Going to sleep");
  delay(100);  // This delay is so the message would be able to arrive
  ESP.deepSleep(SLEEPTIME);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
