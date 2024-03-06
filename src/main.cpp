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

TempHumidity tempHumidity(DHTPIN, DHTTYPE); // Erstellen Sie eine Instanz von TempHumidity
HeaterSettings heater(tempHumidity);        // Passen Sie den Konstruktor an, um TempHumidity zu verwenden
Relais heaterRelay(HEATER_RELAIS_PIN, "Heater");
Relais fanRelay(FAN_RELAIS_PIN, "Fan");

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
    }
  }
  else if (String(topic) == "cmnd/dryer/heater")
  {
    if (messageStr.equalsIgnoreCase("ON"))
    {
      heaterRelay.turnOn();
      fanRelay.turnOff(); // on NC
      Serial.println("Heater turned ON");
    }
    else if (messageStr.equalsIgnoreCase("OFF"))
    {
      heaterRelay.turnOff();
      fanRelay.turnOn(); // NC
      Serial.println("Heater turned OFF");
    }
  }
  else if (String(topic) == "cmnd/dryer/fan")
  {
    if (messageStr.equalsIgnoreCase("ON"))
    {
      fanRelay.turnOff(); // NC
      Serial.println("Fan turned ON (NC)");
    }
    else if (messageStr.equalsIgnoreCase("OFF"))
    {
      fanRelay.turnOn(); // turns fan off NC
      Serial.println("Fan turned OFF (NC)");
    }
  }
}

void publishDryerState()
{
  tempHumidity.updateReadings(); // Aktualisieren Sie die Messwerte

  StaticJsonDocument<300> doc;
  doc["humidity"] = tempHumidity.getHumidity();
  doc["currentTemperature"] = tempHumidity.getTemperature();
  doc["targetTemperature"] = heater.getTargetTemperature();
  doc["remainingTime"] = (heater.computeRemainingTime() / 60000);
  doc["heaterState"] = heaterRelay.getState();
  doc["fanState"] = fanRelay.getState();

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

  // Initialisieren des DHT-Sensors
  tempHumidity.setupDHT();

  // Initialzustand der Relais setzen
  heaterRelay.turnOff();
  fanRelay.turnOff();
}
void loop()
{
  // Überwachung der Umgebungsbedingungen
  float temperature = tempHumidity.getTemperature();

  // MQTT Verbindung überprüfen und wiederverbinden bei Bedarf
  if (!mqtt_client.connected())
  {
    reconnectToBroker();
  }
  mqtt_client.loop();

  // Überprüfung der Sicherheitsbedingungen
  if (temperature > 80)
  {
    heaterRelay.turnOff();
    fanRelay.turnOff(); // Lüfter einschalten bei NC-Anschluss
    Serial.println("Safety: Temperature > 80°C. Heater OFF, Fan ON (NC).");
  }

  publishDryerState();
  delay(1000); // Verzögerung zur Begrenzung der Ausführungshäufigkeit
}
