#include <Arduino.h>
#include <TempHumidity.hpp>
#include <Wifi.hpp>
#include <Mqtt.hpp>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <Relais.hpp>
#include <FilamentSettings.hpp>
#include <HeaterSettings.hpp>

HeaterSettings heater(DHTPIN);
Relais heaterRelay(HEATER_RELAIS_PIN, "Heater");
Relais fanRelay(FAN_RELAIS_PIN, "Fan");

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  String messageStr(message);

  if (String(topic) == "cmnd/dryer/filament/")
  {
    for (const auto &setting : filamentSettings)
    {
      if (setting.material == messageStr)
      {
        heater.setTargetTemperature(setting.temperature, heaterRelay);
        heater.setTargetTime(setting.time);
        Serial.println("TURN ON HEATER");
        fanRelay.turnOff();
        heaterRelay.turnOn();

        break;
      }
    }
  }
}

void publishDryerState()
{
  StaticJsonDocument<300> doc;
  float currentTemperature = heater.readCurrentTemperature();
  unsigned long remainingTime = heater.computeRemainingTime();
  bool heaterState = heaterRelay.getState();
  bool fanState = fanRelay.getState();

  doc["currentTemperature"] = currentTemperature;
  doc["remainingTime"] = remainingTime;
  doc["heaterState"] = heaterState;
  doc["fanState"] = fanState;

  char buffer[512];
  serializeJson(doc, buffer);
  mqtt_client.publish("tele/dryer/state", buffer);

  char infoBuffer[100];

  sprintf(infoBuffer, "INFO: %s %d \n",
          heaterRelay.getName().c_str(),
          heaterRelay.getState() ? 1 : 0);

  Serial.print(infoBuffer);
}

void setupRelais()
{
  pinMode(HEATER_RELAIS_PIN, OUTPUT);
  pinMode(FAN_RELAIS_PIN, OUTPUT);
}

void setup()
{
  setupRelais();
  Serial.begin(115200);
  connectToWifi();
  connectToBroker();
  mqtt_client.setCallback(mqttCallback);
}
void loop()
{

  float temperature = 0.0;
  float humidtiy = 0.0;
  startDHTMonitoring(temperature, humidtiy);
  float currentTemperature = heater.readCurrentTemperature();
  unsigned long remainingTime = heater.computeRemainingTime();

  if (!mqtt_client.connected())
  {
    reconnectToBroker();
  }
  mqtt_client.loop();

  publishDryerState();
  delay(10000);

  if (currentTemperature > heater.getTargetTemperature() || remainingTime == 0)
  {
    heaterRelay.turnOff(); 
    fanRelay.turnOn();     // Schalte den Lüfter aus (oder ein, je nach Anschluss)
    publishDryerState();
  }
}
