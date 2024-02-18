#include <Arduino.h>
#include <temp_humidity.hpp>
#include <wifi.hpp>
#include <mqtt.hpp>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>

#define HEATER_RELAIS_PIN 5


static String info_text = "Watering system ready";
bool pump_on = false;
static PubSubClient connected_mqtt_client;


void toggleRelais()
{
 
  digitalWrite(HEATER_RELAIS_PIN, HIGH);
  Serial.print("heater on\n");
  delay(1000);
 
  digitalWrite(HEATER_RELAIS_PIN, LOW);
  Serial.print("heater off\n");
  delay(1000);
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(HEATER_RELAIS_PIN, OUTPUT);
  
  Serial.begin(115200);
  setupDHT();
  connectTohWifi();
  connected_mqtt_client = connectToBroker();

}

void loop()
{
  startDHTMonitoring();

  static StaticJsonDocument<300> doc;
  static StaticJsonDocument<300> to_publish;
  
  toggleRelais();
  doc["pump_on"] = pump_on;
  
  
}
