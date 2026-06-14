#ifndef DRYER_CONTROLLER_HPP
#define DRYER_CONTROLLER_HPP

#include <Arduino.h>
#include <Relais.hpp>
#include <NcRelay.hpp>
#include <TempHumidity.hpp>
#include "HeaterSettings.hpp"

enum class DryerState {
    IDLE,     // no active drying cycle, all outputs off
    HEATING,  // temperature below target, heater on
    HOLDING,  // temperature at target, heater cycling off
    COOLING,  // cycle complete or reset, fan running until cool
    MANUAL,   // relay states controlled directly via MQTT
    SAFETY    // over-temperature cutoff (>= 80 °C)
};

class DryerController {
public:
    DryerController(HeaterSettings& heater, Relais& heaterRelay,
                    NcRelay& fanRelay, TempHumidity& sensor);

    // Call once per loop iteration to evaluate transitions
    void update();

    // MQTT-triggered transitions
    void applyFilamentPreset(uint8_t targetTemp, unsigned long targetTime);
    void reset();
    void setManualHeater(bool on);
    void setManualFan(bool on);

    DryerState  getState()     const { return state; }
    const char* getStateName() const;

private:
    HeaterSettings& heater;
    Relais&         heaterRelay;
    NcRelay&        fanRelay;
    TempHumidity&   sensor;
    DryerState      state;

    void enterHeatingOrHolding();
    void transitionTo(DryerState next, const char* reason);
};

#endif // DRYER_CONTROLLER_HPP
