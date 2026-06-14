#include "Button.hpp"

Button::Button(uint8_t pin)
    : _pin(pin), _lastRaw(true), _confirmed(false), _lastChangeTime(0)
{}

void Button::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

bool Button::wasPressed() {
    bool raw = (digitalRead(_pin) == LOW);
    uint32_t now = millis();

    if (raw != _lastRaw) {
        _lastChangeTime = now;
        _lastRaw = raw;
    }

    if ((now - _lastChangeTime) >= DEBOUNCE_MS) {
        if (raw && !_confirmed) {
            _confirmed = true;
            return true;
        }
        if (!raw) {
            _confirmed = false;
        }
    }
    return false;
}
