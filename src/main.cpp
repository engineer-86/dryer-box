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
#include <DisplayManager.hpp>
#include <Button.hpp>
#include <Pins.hpp>

TempHumidity    tempHumidity(DHTPIN, DHTTYPE);
HeaterSettings  heater(tempHumidity);
Relais          heaterRelay(HEATER_RELAIS_PIN, "Heater");
NcRelay         fanRelay(FAN_RELAIS_PIN, "Fan");
DryerController dryer(heater, heaterRelay, fanRelay, tempHumidity);
Provisioning    provisioning;
DisplayManager  display(DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
Button          btnPreset(BUTTON_PRESET_PIN);
Button          btnStart(BUTTON_START_PIN);
bool            bootCountCleared = false;

// TestFilament (last entry) is excluded from button cycling
constexpr uint8_t NUM_PRESETS = (sizeof(filamentSettings) / sizeof(filamentSettings[0])) - 1;
uint8_t selectedPresetIndex = 0;

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

void updateDisplay() {
  bool idle = (dryer.getState() == DryerState::IDLE);
  const FilamentSetting& preset = filamentSettings[selectedPresetIndex];

  display.update(
    dryer.getStateName(),
    tempHumidity.getTemperature(),
    idle ? preset.temperature : heater.getTargetTemperature(),
    tempHumidity.getHumidity(),
    idle ? preset.time / 60000 : heater.computeRemainingTime() / 60000,
    heaterRelay.getState(),
    fanRelay.getState(),
    idle ? preset.material.c_str() : nullptr
  );
}

void setup() {
  Serial.begin(115200);
  btnPreset.begin();
  btnStart.begin();
  display.begin();

  display.showMessage("Dryer Box", "Provisioning...");
  if (!provisioning.begin()) return;

  const NetworkCredentials& creds = provisioning.getCredentials();

  display.showMessage("Connecting", "WiFi...");
  if (!connectToWifi(creds)) {
    display.showMessage("WiFi failed!", "Resetting...");
    Provisioning::clearCredentials();
    delay(2000);
    ESP.restart();
  }

  display.showMessage("Connecting", "MQTT broker...");
  connectToBroker(creds);
  mqtt_client.setCallback(mqttCallback);
  tempHumidity.setupDHT();

  display.showMessage("Dryer Box", "Ready!");
  delay(1000);
}

void handleButtons() {
  if (btnPreset.wasPressed()) {
    if (dryer.getState() == DryerState::IDLE) {
      selectedPresetIndex = (selectedPresetIndex + 1) % NUM_PRESETS;
      Serial.printf("Preset: %s\n", filamentSettings[selectedPresetIndex].material.c_str());
    }
  }

  if (btnStart.wasPressed()) {
    if (dryer.getState() == DryerState::IDLE) {
      const FilamentSetting& s = filamentSettings[selectedPresetIndex];
      dryer.applyFilamentPreset(s.temperature, s.time);
      Serial.printf("Started: %s\n", s.material.c_str());
    } else {
      dryer.reset();
      Serial.println("Reset");
    }
  }
}

void loop() {
  handleButtons();
  mqtt_client.loop();

  static uint32_t lastTick = 0;
  if (millis() - lastTick >= 1000) {
    lastTick = millis();

    tempHumidity.updateReadings();

    if (!mqtt_client.connected()) {
      reconnectToBroker();
    }

    if (!bootCountCleared && millis() > 10000) {
      provisioning.clearBootCounter();
      bootCountCleared = true;
    }

    dryer.update();
    updateDisplay();
    publishDryerState();
  }
}
