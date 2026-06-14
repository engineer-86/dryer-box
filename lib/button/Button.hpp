#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin);
    void begin();
    void update();           // call once per loop before reading events
    bool wasPressed();       // short press: released before LONG_PRESS_MS
    bool wasLongPressed();   // held for >= LONG_PRESS_MS (fires once at threshold)

private:
    uint8_t  _pin;
    bool     _lastRaw;
    bool     _debounced;
    bool     _longFired;
    uint32_t _pressStart;
    bool     _shortEvent;
    bool     _longEvent;

    static constexpr uint32_t DEBOUNCE_MS   = 50;
    static constexpr uint32_t LONG_PRESS_MS = 3000;
};

#endif // BUTTON_HPP
