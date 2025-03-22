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

// Initialize objects
TempHumidity tempHumidity(DHTPIN, DHTTYPE);
HeaterSettings heater(tempHumidity);
Relais heaterRelay(HEATER_RELAIS_PIN, "Heater");
Relais fanRelay(FAN_RELAIS_PIN, "Fan");
bool manualOverride = false;

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  // Convert the payload to a null-terminated string
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  String messageStr(message);

  Serial.print("MQTT message received on topic: ");
  Serial.print(topic);
  Serial.print(" Message: ");
  Serial.println(messageStr);

  String topicStr = String(topic);

  // Handle the RESET command before processing filament settings
  if (topicStr == "cmnd/dryer/filament" && messageStr.equalsIgnoreCase("RESET")) {
    manualOverride = false;
    heaterRelay.turnOff();    // Turn off the heater
    heater.setTargetTime(0);
    fanRelay.turnOn();        // NC: "turnOn" turns the fan OFF
    Serial.println("Heater reset filament state!");
    return; // Exit callback after processing RESET
  }

  if (topicStr == "cmnd/dryer/filament") {
    // Iterate through filament settings to apply matching configuration
    for (const auto &setting : filamentSettings) {
      if (setting.material.equalsIgnoreCase(messageStr)) {
        heater.setTargetTemperature(setting.temperature, heaterRelay, fanRelay);
        heater.setTargetTime(setting.time);
        fanRelay.turnOff();   // NC: "turnOff" turns the fan ON
        heaterRelay.turnOn();  // Turn heater ON
        Serial.println("Filament settings applied: " + messageStr);
        break;
      }
    }
  }
  else if (topicStr == "cmnd/dryer/heater") {
    manualOverride = true; // Enable manual override
    if (messageStr.equalsIgnoreCase("ON")) {
      heaterRelay.turnOn();
      fanRelay.turnOff();    // NC: "turnOff" turns the fan ON
      Serial.println("Heater turned ON (Manual Override)");
    }
    else if (messageStr.equalsIgnoreCase("OFF")) {
      heaterRelay.turnOff();
      fanRelay.turnOn();     // NC: "turnOn" turns the fan OFF
      Serial.println("Heater turned OFF (Manual Override)");
    }
  }
  else if (topicStr == "cmnd/dryer/fan") {
    manualOverride = true; // Enable manual override
    if (messageStr.equalsIgnoreCase("ON")) {
      fanRelay.turnOff();    // NC: "turnOff" turns the fan ON
      Serial.println("Fan turned ON (NC) (Manual Override)");
    }
    else if (messageStr.equalsIgnoreCase("OFF")) {
      fanRelay.turnOn();     // NC: "turnOn" turns the fan OFF
      Serial.println("Fan turned OFF (NC) (Manual Override)");
    }
  }
}

void publishDryerState() {
  // Update sensor readings
  tempHumidity.updateReadings();

  StaticJsonDocument<300> doc;
  doc["humidity"] = tempHumidity.getHumidity();
  doc["currentTemperature"] = tempHumidity.getTemperature();
  doc["targetTemperature"] = heater.getTargetTemperature();
  doc["remainingTime"] = heater.computeRemainingTime() / 60000;
  doc["heaterState"] = heaterRelay.getState();
  // For NC logic: if fanRelay.getState() is false, then the fan is actually ON
  doc["fanState"] = !fanRelay.getState();

  char buffer[512];
  serializeJson(doc, buffer);
  mqtt_client.publish("tele/dryer/state", buffer);

  char infoBuffer[256];
  sprintf(infoBuffer, "INFO: Heater: %s %d, Fan: %s %d\n",
          heaterRelay.getName().c_str(), heaterRelay.getState() ? 1 : 0,
          fanRelay.getName().c_str(), !fanRelay.getState() ? 1 : 0);
  Serial.print(infoBuffer);
}

void setupRelais() {
  pinMode(HEATER_RELAIS_PIN, OUTPUT);
  pinMode(FAN_RELAIS_PIN, OUTPUT);
}

void setup() {
  setupRelais();
  Serial.begin(115200);

  // Connect to WiFi and MQTT broker
  connectToWifi();
  connectToBroker();
  mqtt_client.setCallback(mqttCallback);

  // Initialize the temperature/humidity sensor
  tempHumidity.setupDHT();

  // Set initial states: Heater OFF, Fan OFF in NC configuration
  heaterRelay.turnOff();
  fanRelay.turnOn();
}

void loop() {
  // Update sensor readings
  tempHumidity.updateReadings();
  float temperature = tempHumidity.getTemperature();

  // Ensure MQTT connection is active
  if (!mqtt_client.connected()) {
    reconnectToBroker();
  }
  mqtt_client.loop();

  // Safety shutdown if temperature is >= 80°C
  if (temperature >= 80) {
    heaterRelay.turnOff();
    fanRelay.turnOff();    // NC: "turnOff" turns the fan ON
    Serial.println("Safety: Temperature >= 80°C. Heater OFF, Fan ON.");
  }
  else if (temperature < 30 && !heaterRelay.getState() && !manualOverride) {
    // If temperature is below 30°C and heater is off, turn fan OFF
    fanRelay.turnOn();     // NC: "turnOn" turns the fan OFF
    Serial.println("Temperature < 30°C. Fan OFF.");
  }
  else if (temperature >= heater.getTargetTemperature() && !manualOverride) {
    // If target temperature is reached or exceeded, turn heater OFF and keep fan ON for cooling
    heaterRelay.turnOff();
    fanRelay.turnOff();    // NC: "turnOff" turns the fan ON
    Serial.println("Target temperature reached or exceeded. Heater OFF, Fan ON for cooling.");
  }
  else if (temperature < heater.getTargetTemperature() && !manualOverride) {
    // If temperature is below target, turn heater ON and keep fan ON
    heaterRelay.turnOn();
    fanRelay.turnOff();    // NC: "turnOff" turns the fan ON
    Serial.println("PTC heater is activated. Temperature below target.");
  }

  // Check if the drying time has elapsed and turn off the heater if necessary
  unsigned long remainingTime = heater.computeRemainingTime();
  if (remainingTime == 0 && heaterRelay.getState()) {
    heaterRelay.turnOff();
    Serial.println("Drying complete. Heater OFF.");
    if (temperature < 30 && !heaterRelay.getState()) {
      // At low temperatures, turn fan OFF (NC: turnOn turns the fan OFF)
      fanRelay.turnOn();
      Serial.println("Temperature < 30°C. Fan OFF.");
    }
  }

  publishDryerState();
  delay(1000); // Short delay to prevent loop overload
}
