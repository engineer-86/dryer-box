#include "HeaterSettings.hpp"

HeaterSettings::HeaterSettings(TempHumidity& tempHumidity) : tempHumidity(tempHumidity), targetTemperature(0), targetTime(0), startTime(0) {
}

void HeaterSettings::setTargetTemperature(uint8_t temperature, Relais& heaterRelay, Relais& fanRelay) {
    tempHumidity.updateReadings(); 
    targetTemperature = temperature;
}

void HeaterSettings::setTargetTime(unsigned long time) {
    targetTime = time;
    startTime = millis();
}

uint8_t HeaterSettings::getTargetTemperature() const {
    return targetTemperature;
}

unsigned long HeaterSettings::getTargetTime() const {
    return targetTime;
}

unsigned long HeaterSettings::computeRemainingTime() {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;
    if (elapsedTime >= targetTime) {
        return 0;
    }
    return targetTime - elapsedTime;
}
