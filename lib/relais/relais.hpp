/**
 * @file Relais.h
 * @brief Defines the Relais class for controlling relay modules.
 */

#ifndef RELAIS_H
#define RELAIS_H

#include <Arduino.h>

/**
 * @class Relais
 * @brief A class to control a relay module.
 *
 * This class provides methods to turn a relay on and off and to get the
 * relay's name. It is designed to work with the Arduino platform.
 */
class Relais
{
private:
    uint8_t pin; ///< The GPIO pin number connected to the relay.
    bool state;  ///< The state of the relay.
    String name; ///< The name of the relay.

public:
    /**
     * @brief Construct a new Relais object.
     *
     * @param pin The GPIO pin number connected to the relay.
     * @param name The name of the relay.
     */
    Relais(uint8_t pin, String name);

    /**
     * @brief Turn the relay on.
     */
    void turnOn();

    /**
     * @brief Turn the relay off.
     */
    void turnOff();

    /**
     * @brief Get the name of the relay.
     *
     * @return String The name of the relay.
     */
    String getName();

    /**
     * @brief Get the state of the relay.
     *
     * @return bool The state of the relay.
     */
    bool getState();
};

#endif // RELAIS_H
