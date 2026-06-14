#ifndef RELAIS_H
#define RELAIS_H

#include <Arduino.h>

class Relais
{
private:
    uint8_t pin;
    bool state;
    String name;

public:
    Relais(uint8_t pin, String name);

    void turnOn();
    void turnOff();
    String getName() const;
    bool getState() const;
};

#endif // RELAIS_H
