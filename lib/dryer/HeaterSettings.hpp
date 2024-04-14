#ifndef HEATER_SETTINGS_HPP
#define HEATER_SETTINGS_HPP

#include <Arduino.h>
#include "TempHumidity.hpp"
#include "Relais.hpp"

class HeaterSettings {
public:
    HeaterSettings(TempHumidity& tempHumidity);
    void setTargetTemperature(uint8_t temperature, Relais& heaterRelay, Relais& fanRelay);
    void setTargetTime(unsigned long time);
    uint8_t getTargetTemperature() const;
    unsigned long getTargetTime() const;
    unsigned long computeRemainingTime();

private:
    TempHumidity& tempHumidity;
    uint8_t targetTemperature;
    unsigned long targetTime;
    unsigned long startTime;
};

#endif // HEATER_SETTINGS_HPP
