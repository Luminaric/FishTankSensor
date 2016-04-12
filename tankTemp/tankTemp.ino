/*
  This sketch writes directly on the ESP chip.
  The ESP8266WiFi.h library is used here.
  This sensor is battery powered and uses the ESP12 
  chip. The chip is put into sleep between transmissions.
  The DHT22 is used to get the Temp and Humidity. Battery 
  voltage is also sampled and transmitted to the MQTT server.
  
  Version:   1.0
  SensorID:  TankTemperature
  Platform:  ESP8266
  Date:      2016-04-11
  Requires:
  Hardware:  Uses the DS18B20 Dallas Temperature sensor. 

  Update history
  
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "ethernetSettings.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include "Timer.h"

//This bit sets up the onboard voltage read
extern "C" {
#include "user_interface.h"
uint16 readvdd33(void);
}

#define SENSORTXT              " using Dallas sensor. Address"
#define ONE_MINUTE            60000
#define FIVE_MINUTE           300000
#define TEN_MINUTE            600000
#define SIXTY_MINUTE          3600000
#define SENSORID              "TANKTEMP"

WiFiClient wireless;
PubSubClient client(wireless);


OneWire oneWire(2);
DallasTemperature sensors(&oneWire);
DeviceAddress tankSensor;

Timer t;

void doIT() {
  sensors.requestTemperatures();
  float t = sensors.getTempC(tankSensor);
  dtostrf(t,4,1,temperatureC{7});
   
  if(!client.connected()) {
    client.connect(SENSORID,"pi","raspberry");
    Serial.println("Connected");
    Serial.print("rc=");
    Serial.println(client.state());
  }
  String mqttString = "[\"";
  mqttString += SENSORID;
  mqttString += "\",\"temperature\":\"";
  mqttString += temperature;
  mqttString += "\"}]";
  Serial.print("MQTT String: ");
  Serial.println(mqttString);
  char payload[100];
  mqttString.toCharArray(payload, 100);
  client.publish("Sensors",payload);

}


void setup()
{
  Serial.begin(9600);
  Serial.print("Sensor is ");Serial.println(SENSORID);
  Serial.println("Timer is set for 10 Minutes");
  Serial.println("Looking for ");Serial.print(ssid);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(server,1883);
  //int fiveMinutePause = t.every(FIVE_MINUTE, fiveMinutes);
  int tenMinutePause = t.every(TEN_MINUTE, doIT);
  Serial.println("10 minute timmer setup");
  
//collect the first reading and send it.
  doIT();
  delay(1000);
  Serial.println("Setup complete, first data point sent and starting timer loop");
}
  
void loop()
{
  t.update();
}
