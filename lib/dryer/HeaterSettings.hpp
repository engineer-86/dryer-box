/**
 * @file HeaterSettings.hpp
 * @brief Provides a class to manage heater settings for a filament dryer.
 *
 * This class manages the intended (set point) temperature and time for a filament dryer.
 * It also provides functionality to read the actual temperature and remaining time.
 */

#ifndef HEATER_SETTINGS_HPP
#define HEATER_SETTINGS_HPP

#include <Arduino.h>
#include <Relais.hpp>
class HeaterSettings
{
public:
    /**
     * @brief Construct a new Heater Settings object.
     *
     * @param tempPin The pin number where the temperature sensor is attached.
     */
    explicit HeaterSettings(uint8_t tempPin);

    /**
     * @brief Set the Target Temperature object.
     *
     * @param temperature The desired temperature in degrees Celsius.
     */
    void setTargetTemperature(uint8_t temperature, Relais heaterRelay);

    /**
     * @brief Set the Target Time object.
     *
     * @param time The amount of time in milliseconds the heater should run.
     */
    void setTargetTime(unsigned long time);

    /**
     * @brief Get the Target Temperature object.
     *
     * @return uint8_t The target temperature in degrees Celsius.
     */
    uint8_t getTargetTemperature() const;

    /**
     * @brief Get the Target Time object.
     *
     * @return unsigned long The target time in milliseconds.
     */
    unsigned long getTargetTime() const;

    /**
     * @brief Reads the current temperature from the temperature sensor.
     *
     * @return uint8_t The current temperature in degrees Celsius.
     */
    float readCurrentTemperature();

    /**
     * @brief Computes the remaining time for the heater.
     *
     * @return unsigned long The remaining time in milliseconds.
     */
    unsigned long computeRemainingTime();

private:
    uint8_t tempPin;           // The pin where the temperature sensor is attached
    uint8_t targetTemperature; // Desired temperature set point
    unsigned long targetTime;  // Desired time set point
    unsigned long startTime;   // Start time of the heating process
};

#endif // HEATER_SETTINGS_HPP
