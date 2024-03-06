#include <Arduino.h>
#include <temp_humidity.hpp>
#include <wifi.hpp>
#include <mqtt.hpp>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <relais.hpp>
#include <filament_settings.hpp>

static String info_text = "Watering system ready";
bool pump_on = false;
static PubSubClient connected_mqtt_client;
char payload[200];

void mqttCallback(char *topic, byte *payload, unsigned int length)
{

  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  // setDryerSettings(String(message));
}

void setupRelais()
{
  pinMode(HEATER_RELAIS_PIN, OUTPUT);
  pinMode(FAN_RELAIS_PIN, OUTPUT);
}

Relais heaterRelay(HEATER_RELAIS_PIN, "HEATER");
Relais fanRelay(FAN_RELAIS_PIN, "FAN");

void setup()
{
  setupRelais();
  setupDHT();
  Serial.begin(115200);

  connectTohWifi();
  connected_mqtt_client = connectToBroker();
}

void loop()
{
  char infoBuffer[100];
  float temperature = 0.0;
  float humidtiy = 0.0;
  startDHTMonitoring(temperature, humidtiy);

  sprintf(infoBuffer, "INFO: %s %d \n",
          heaterRelay.getName().c_str(),
          heaterRelay.getState() ? 1 : 0);

  Serial.print(infoBuffer);

  // Security statement temperature never higher then 80 degrees C
  if (temperature > 80)
  {
    heaterRelay.turnOff();
    fanRelay.turnOff(); // Fan relay is normally open, turnOff means fan on!
  }

  static StaticJsonDocument<300> doc;
  static StaticJsonDocument<300> to_publish;
  doc["heater relay state"] = heaterRelay.getState();
  doc["fan relay state"] = fanRelay.getState();
  doc["temperature"] = temperature;
  doc["humidtiy"] = humidtiy;
  to_publish = doc;

  serializeJson(to_publish, payload);
  connected_mqtt_client.publish("tele/dryer/state", payload);
}
