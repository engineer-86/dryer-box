#include "DryerController.hpp"

DryerController::DryerController(HeaterSettings& heater, Relais& heaterRelay,
                                 NcRelay& fanRelay, TempHumidity& sensor)
    : heater(heater), heaterRelay(heaterRelay), fanRelay(fanRelay),
      sensor(sensor), state(DryerState::IDLE)
{}

void DryerController::transitionTo(DryerState next, const char* reason) {
    Serial.print("STATE ");
    Serial.print(getStateName());
    Serial.print(" -> ");
    state = next;
    Serial.print(getStateName());
    Serial.print(" (");
    Serial.print(reason);
    Serial.println(")");
}

void DryerController::update() {
    float temp = sensor.getTemperature();

    // Safety always overrides every other state
    if (temp >= 80 && state != DryerState::SAFETY) {
        heaterRelay.turnOff();
        fanRelay.turnOn();
        transitionTo(DryerState::SAFETY, "temp >= 80C");
        return;
    }

    switch (state) {
        case DryerState::IDLE:
            break;

        case DryerState::HEATING:
            if (heater.computeRemainingTime() == 0) {
                heaterRelay.turnOff();
                transitionTo(DryerState::COOLING, "timer elapsed");
            } else if (temp >= heater.getTargetTemperature()) {
                heaterRelay.turnOff();
                transitionTo(DryerState::HOLDING, "target reached");
            }
            break;

        case DryerState::HOLDING:
            if (heater.computeRemainingTime() == 0) {
                heaterRelay.turnOff();
                transitionTo(DryerState::COOLING, "timer elapsed");
            } else if (temp < heater.getTargetTemperature()) {
                heaterRelay.turnOn();
                transitionTo(DryerState::HEATING, "temp dropped below target");
            }
            break;

        case DryerState::COOLING:
            if (temp < 30) {
                fanRelay.turnOff();
                transitionTo(DryerState::IDLE, "cooled down");
            }
            break;

        case DryerState::MANUAL:
            break; // relay states driven entirely by MQTT commands

        case DryerState::SAFETY:
            if (temp < 75) { // 5 °C hysteresis before re-engaging
                if (heater.getTargetTemperature() > 0 && heater.computeRemainingTime() > 0) {
                    transitionTo(DryerState::COOLING, "temp safe, letting cool");
                } else {
                    fanRelay.turnOff();
                    transitionTo(DryerState::IDLE, "temp safe, no active target");
                }
            }
            break;
    }
}

void DryerController::applyFilamentPreset(uint8_t targetTemp, unsigned long targetTime) {
    heater.setTargetTemperature(targetTemp);
    heater.setTargetTime(targetTime);
    fanRelay.turnOn();
    enterHeatingOrHolding();
}

void DryerController::reset() {
    heater.setTargetTemperature(0);
    heater.setTargetTime(0);
    heaterRelay.turnOff();
    fanRelay.turnOn(); // keep fan running until cooled; COOLING handles the rest
    transitionTo(DryerState::COOLING, "reset");
}

void DryerController::setManualHeater(bool on) {
    if (on) { heaterRelay.turnOn(); fanRelay.turnOn(); }
    else      heaterRelay.turnOff();
    transitionTo(DryerState::MANUAL, on ? "heater ON" : "heater OFF");
}

void DryerController::setManualFan(bool on) {
    if (on) fanRelay.turnOn();
    else    fanRelay.turnOff();
    transitionTo(DryerState::MANUAL, on ? "fan ON" : "fan OFF");
}

void DryerController::enterHeatingOrHolding() {
    float temp = sensor.getTemperature();
    if (temp >= heater.getTargetTemperature()) {
        heaterRelay.turnOff();
        transitionTo(DryerState::HOLDING, "already at target");
    } else {
        heaterRelay.turnOn();
        transitionTo(DryerState::HEATING, "below target");
    }
}

const char* DryerController::getStateName() const {
    switch (state) {
        case DryerState::IDLE:    return "IDLE";
        case DryerState::HEATING: return "HEATING";
        case DryerState::HOLDING: return "HOLDING";
        case DryerState::COOLING: return "COOLING";
        case DryerState::MANUAL:  return "MANUAL";
        case DryerState::SAFETY:  return "SAFETY";
        default:                  return "UNKNOWN";
    }
}
