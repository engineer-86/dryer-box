#include "Button.hpp"

Button::Button(uint8_t pin)
    : _pin(pin), _lastRaw(false), _debounced(false), _longFired(false),
      _pressStart(0), _shortEvent(false), _longEvent(false)
{}

void Button::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

void Button::update() {
    bool raw = (digitalRead(_pin) == LOW);
    uint32_t now = millis();

    if (raw != _lastRaw) {
        _lastRaw = raw;
        if (raw) {
            _pressStart  = now;
            _debounced   = false;
            _longFired   = false;
        } else if (_debounced && !_longFired) {
            _shortEvent = true;
        }
    }

    if (raw && !_debounced && (now - _pressStart) >= DEBOUNCE_MS) {
        _debounced = true;
    }

    if (raw && _debounced && !_longFired && (now - _pressStart) >= LONG_PRESS_MS) {
        _longFired  = true;
        _longEvent  = true;
    }
}

bool Button::wasPressed() {
    bool e = _shortEvent;
    _shortEvent = false;
    return e;
}

bool Button::wasLongPressed() {
    bool e = _longEvent;
    _longEvent = false;
    return e;
}
