#include <Arduino.h>
#include <temp_humidity.hpp>
#include <wifi.hpp>
#include <mqtt.hpp>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>

static String info_text = "Watering system ready";
bool pump_on = false;
static PubSubClient connected_mqtt_client;
char payload[200];

void toggleRelais()
{

  // digitalWrite(FAN_RELAIS_PIN, LOW);
  // Serial.print("FAN ON\n");
  // delay(1000);

  digitalWrite(HEATER_RELAIS_PIN, HIGH);
  Serial.print("heater on\n");
  delay(1000);
}

void setup()
{

  pinMode(HEATER_RELAIS_PIN, OUTPUT);
  pinMode(FAN_RELAIS_PIN, OUTPUT);
  Serial.begin(115200);
  setupDHT();
  connectTohWifi();
  connected_mqtt_client = connectToBroker();
}

void loop()
{
  float temperature = 0.0;
  float humidtiy = 0.0;

  startDHTMonitoring(temperature, humidtiy);

  // temperature never higher then 80 degrees C
  if (temperature > 20)
  {
    digitalWrite(FAN_RELAIS_PIN, HIGH);
    Serial.print("heater off\n");
  }
  static StaticJsonDocument<300> doc;
  static StaticJsonDocument<300> to_publish;

  toggleRelais();
  doc["temperature"] = temperature;
  doc["humidtiy"] = humidtiy;
  to_publish = doc;

  serializeJson(to_publish, payload);
  connected_mqtt_client.publish("tele/heater/state", payload);
}
