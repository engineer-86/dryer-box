#include <Arduino.h>
#include <ArduinoJson.h>
#include <TempHumidity.hpp>
#include <Wifi.hpp>
#include <Mqtt.hpp>
#include <Relais.hpp>
#include <NcRelay.hpp>
#include <FilamentSettings.hpp>
#include <HeaterSettings.hpp>
#include <DryerController.hpp>
#include <Provisioning.hpp>
#include <Pins.hpp>

TempHumidity    tempHumidity(DHTPIN, DHTTYPE);
HeaterSettings  heater(tempHumidity);
Relais          heaterRelay(HEATER_RELAIS_PIN, "Heater");
NcRelay         fanRelay(FAN_RELAIS_PIN, "Fan");
DryerController dryer(heater, heaterRelay, fanRelay, tempHumidity);
Provisioning    provisioning;
bool            bootCountCleared = false;

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  String topicStr(topic);
  String messageStr(message);

  Serial.print("MQTT | ");
  Serial.print(topicStr);
  Serial.print(" | ");
  Serial.println(messageStr);

  if (topicStr == "cmnd/dryer/filament") {
    if (messageStr.equalsIgnoreCase("RESET")) {
      dryer.reset();
      return;
    }
    for (const auto &s : filamentSettings) {
      if (s.material.equalsIgnoreCase(messageStr)) {
        dryer.applyFilamentPreset(s.temperature, s.time);
        Serial.println("Preset applied: " + messageStr);
        break;
      }
    }
  }
  else if (topicStr == "cmnd/dryer/heater") {
    dryer.setManualHeater(messageStr.equalsIgnoreCase("ON"));
  }
  else if (topicStr == "cmnd/dryer/fan") {
    dryer.setManualFan(messageStr.equalsIgnoreCase("ON"));
  }
  else if (topicStr == "cmnd/dryer/config" && messageStr.equalsIgnoreCase("RESET")) {
    // Wipe credentials and re-enter provisioning on next boot
    Provisioning::clearCredentials();
    delay(500);
    ESP.restart();
  }
}

void publishDryerState() {
  StaticJsonDocument<300> doc;
  doc["state"]              = dryer.getStateName();
  doc["humidity"]           = tempHumidity.getHumidity();
  doc["currentTemperature"] = tempHumidity.getTemperature();
  doc["targetTemperature"]  = heater.getTargetTemperature();
  doc["remainingTime"]      = heater.computeRemainingTime() / 60000;
  doc["heaterState"]        = heaterRelay.getState();
  doc["fanState"]           = fanRelay.getState();

  char buffer[512];
  serializeJson(doc, buffer);
  mqtt_client.publish("tele/dryer/state", buffer);
}

void setup() {
  Serial.begin(115200);

  if (!provisioning.begin()) return; // AP mode is blocking; this branch is unreachable

  const NetworkCredentials& creds = provisioning.getCredentials();

  if (!connectToWifi(creds)) {
    // Saved credentials are no longer valid — clear them and re-provision
    Serial.println("WiFi failed. Clearing credentials for re-provisioning.");
    Provisioning::clearCredentials();
    delay(500);
    ESP.restart();
  }

  connectToBroker(creds);
  mqtt_client.setCallback(mqttCallback);
  tempHumidity.setupDHT();
}

void loop() {
  tempHumidity.updateReadings();

  if (!mqtt_client.connected()) {
    reconnectToBroker();
  }
  mqtt_client.loop();

  // Reset boot counter once the device has been running for 10 s.
  // This ensures the 5x power-cycle reset only triggers on rapid cycling.
  if (!bootCountCleared && millis() > 10000) {
    provisioning.clearBootCounter();
    bootCountCleared = true;
  }

  dryer.update();
  publishDryerState();
  delay(1000);
}
