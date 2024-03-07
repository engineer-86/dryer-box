/**
 * @file filament_settings.h
 * @brief Contains the definition of the FilamentSetting structure and the utility function to convert hours to milliseconds.
 */

#ifndef FILAMENT_SETTINGS_H
#define FILAMENT_SETTINGS_H

#include <Arduino.h>

/**
 * @struct FilamentSetting
 * @brief Holds the settings for a filament in a 3D printer dryer.
 *
 * This structure is used to define the drying temperature and time for
 * different types of 3D printer filament.
 */
struct FilamentSetting
{
    String material;     ///< The name of the filament material.
    uint8_t temperature; ///< The recommended drying temperature for the filament in degrees Celsius.
    unsigned long time;  ///< The recommended drying time for the filament in milliseconds.
};

/**
 * @brief Converts hours to milliseconds.
 *
 * This constexpr function calculates milliseconds from hours, which is used for
 * setting up the drying times of the filaments.
 *
 * @param hours The number of hours.
 * @return The equivalent number of milliseconds.
 */
constexpr unsigned long hoursToMilliseconds(unsigned int hours)
{
    return hours * 60 * 60 * 1000;
}

// Filament settings for various materials
const FilamentSetting filamentSettings[] = {
    {"PLA", 50, hoursToMilliseconds(4)},
    {"ABS", 60, hoursToMilliseconds(2)},
    {"PETG", 65, hoursToMilliseconds(2)},
    {"NYLON", 70, hoursToMilliseconds(2)},
    {"PC", 70, hoursToMilliseconds(8)},
    {"TPU", 55, hoursToMilliseconds(4)},
    {"PVA", 50, hoursToMilliseconds(4)},
    {"ASA", 60, hoursToMilliseconds(4)},
    {"PP", 55, hoursToMilliseconds(6)},
    {"TestFilament", 40, 10000}, // for test purposes

};

#endif // FILAMENT_SETTINGS_H
