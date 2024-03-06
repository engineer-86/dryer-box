#include "relais.hpp"

Relais::Relais(uint8_t p, String n) : pin(p), name(n)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Relais::turnOn()
{
    digitalWrite(pin, HIGH);
}

void Relais::turnOff()
{
    digitalWrite(pin, LOW);
}

String Relais::getName()
{
    return name;
}

bool Relais::getState()
{
    return state;
}
