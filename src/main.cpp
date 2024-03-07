#include <Arduino.h>
#include <TempHumidity.hpp>
#include <Wifi.hpp>
#include <Mqtt.hpp>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <Relais.hpp>
#include <FilamentSettings.hpp>
#include <HeaterSettings.hpp>
#include <Pins.hpp>

TempHumidity tempHumidity(DHTPIN, DHTTYPE);
HeaterSettings heater(tempHumidity);
Relais heaterRelay(HEATER_RELAIS_PIN, "Heater");
Relais fanRelay(FAN_RELAIS_PIN, "Fan");
bool manualOverride = false;

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  String messageStr(message);
  Serial.print("MQTT message received on topic: ");
  Serial.print(topic);
  Serial.print(" Message: ");
  Serial.println(messageStr);

  if (String(topic) == "cmnd/dryer/filament")
  {
    for (const auto &setting : filamentSettings)
    {
      if (setting.material.equalsIgnoreCase(messageStr))
      {
        heater.setTargetTemperature(setting.temperature, heaterRelay, fanRelay);
        heater.setTargetTime(setting.time);
        fanRelay.turnOff();
        heaterRelay.turnOn();
        Serial.println("Filament settings applied: " + messageStr);
        break;
      }
      else if (messageStr.equalsIgnoreCase("RESET"))
      {
        manualOverride = false;
        heaterRelay.turnOff();
        heater.setTargetTime(0);
        fanRelay.turnOn(); // NC
        Serial.println("Heater reseted filament state!");
      }
    }
  }

  else if (String(topic) == "cmnd/dryer/heater")
  {
    manualOverride = true; // Manual Override
    if (messageStr.equalsIgnoreCase("ON"))
    {
      heaterRelay.turnOn();
      fanRelay.turnOff(); // on NC
      Serial.println("Heater turned ON (Manual Override)");
    }
    else if (messageStr.equalsIgnoreCase("OFF"))
    {
      heaterRelay.turnOff();
      fanRelay.turnOn(); // NC
      Serial.println("Heater turned OFF (Manual Override)");
    }
  }
  else if (String(topic) == "cmnd/dryer/fan")
  {
    manualOverride = true; // Manual Override
    if (messageStr.equalsIgnoreCase("ON"))
    {
      fanRelay.turnOff(); // NC
      Serial.println("Fan turned ON (NC) (Manual Override)");
    }
    else if (messageStr.equalsIgnoreCase("OFF"))
    {
      fanRelay.turnOn(); // turns fan off NC
      Serial.println("Fan turned OFF (NC) (Manual Override)");
    }
  }
}

void publishDryerState()
{
  tempHumidity.updateReadings();

  StaticJsonDocument<300> doc;
  doc["humidity"] = tempHumidity.getHumidity();
  doc["currentTemperature"] = tempHumidity.getTemperature();
  doc["targetTemperature"] = heater.getTargetTemperature();
  doc["remainingTime"] = (heater.computeRemainingTime() / 60000);
  doc["heaterState"] = heaterRelay.getState();
  doc["fanState"] = fanRelay.getState() ? false : true; // change bool for fan monitoring (NC)

  char buffer[512];
  serializeJson(doc, buffer);
  mqtt_client.publish("tele/dryer/state", buffer);

  char infoBuffer[256];
  sprintf(infoBuffer, "INFO: Heater: %s %d, Fan: %s %d\n",
          heaterRelay.getName().c_str(), heaterRelay.getState() ? 1 : 0,
          fanRelay.getName().c_str(), fanRelay.getState() ? 0 : 1);

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

  tempHumidity.setupDHT();

  heaterRelay.turnOff();
  fanRelay.turnOff();
}
void loop()
{
  // Update temperature and humidity readings
  tempHumidity.updateReadings();
  float temperature = tempHumidity.getTemperature();

  // Check MQTT connection and reconnect if necessary
  if (!mqtt_client.connected())
  {
    reconnectToBroker();
  }
  mqtt_client.loop();

  // Safety shutdown at temperatures above 80°C
  if (temperature >= 80)
  {
    heaterRelay.turnOff();
    fanRelay.turnOff(); // Fan ON due to NC connection, safety shutdown
    Serial.println("Safety: Temperature >= 80°C. Heater OFF, Fan ON.");
  }
  else if (temperature < 30 && !heaterRelay.getState() && manualOverride == false)
  {
    // Turn off the fan if the temperature is below 30 degrees and the heater is off
    fanRelay.turnOn(); // Fan OFF due to NC connection
    Serial.println("Temperature < 30°C. Fan OFF.");
  }
  else if (temperature >= heater.getTargetTemperature() && manualOverride == false)
  {
    // Turn off the heater and keep the fan running when the target temperature is reached
    heaterRelay.turnOff();
    fanRelay.turnOff(); // fan keeps cooling (NC)
    Serial.println("Target temperature reached or exceeded. Heater OFF, Fan continues for cooling.");
  }
  else if (temperature < heater.getTargetTemperature() && manualOverride == false)
  {
    // Turn on the heater if the temperature is below the target temperature
    heaterRelay.turnOn();
    fanRelay.turnOff(); // fan keeps cooling (NC)
    Serial.println("PTC heater is activated. Temperature below target.");
  }

  // Calculate remaining time and turn off the heater when time up
  unsigned long remainingTime = heater.computeRemainingTime();
  if (remainingTime == 0 && heaterRelay.getState())
  {
    heaterRelay.turnOff();
    Serial.println("Drying complete. Heater OFF.");

    if (temperature < 30 && !heaterRelay.getState())
    {
      fanRelay.turnOff(); // Fan ON for cooling
      Serial.println("Temperature < 30°C. Fan OFF.");
    }
  }

  publishDryerState();
  delay(1000); // Short pause to not overload the loop
}
