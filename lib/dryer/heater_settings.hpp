/**
 * @file heater_settings.h
 * @brief Provides functions to set the temperature and time for a filament dryer heater.
 */

#ifndef HEATER_SETTINGS_H
#define HEATER_SETTINGS_H

#include <Arduino.h>

/**
 * @brief Sets the temperature of the heater.
 *
 * This function will set the heating element to the desired temperature. The temperature
 * control mechanism should be implemented according to your specific hardware.
 *
 * @param temperature The desired temperature in degrees Celsius.
 */
void setTemperature(uint8_t temperature);

/**
 * @brief Sets the time for which the heater should remain on.
 *
 * This function should utilize a timing mechanism to ensure the heater runs for
 * the specified amount of time. Implementation will vary based on your hardware setup
 * and requirements.
 *
 * @param time The amount of time in milliseconds the heater should run.
 */
void setTime(unsigned long time);

/**
 * @brief Returns the temperature which was setted
 *
 * This function should returns the temperature for the specific filament.
 *
 * @return The temperature for the filament
 */
uint8_t getTemperature();

/**
 * @brief Returns the drying time for the filament
 *
 * This function should returns the time for the specific filament.
 *
 * @return The dry time for the filament
 */
unsigned long getTime();

#endif // HEATER_SETTINGS_H
