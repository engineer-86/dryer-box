#include "Relais.hpp"

Relais::Relais(uint8_t p, String n) : pin(p), state(false), name(n)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Relais::turnOn()
{
    digitalWrite(pin, HIGH);
    this->state = true;
}

void Relais::turnOff()
{
    digitalWrite(pin, LOW);
    this->state = false;
}

String Relais::getName() const
{
    return name;
}

bool Relais::getState() const
{
    return state;
}
