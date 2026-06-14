#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>

class Button {
public:
    Button(uint8_t pin);
    void begin();
    bool wasPressed();

private:
    uint8_t  _pin;
    bool     _lastRaw;
    bool     _confirmed;
    uint32_t _lastChangeTime;

    static constexpr uint32_t DEBOUNCE_MS = 50;
};

#endif // BUTTON_HPP
