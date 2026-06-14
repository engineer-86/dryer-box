#ifndef NC_RELAY_HPP
#define NC_RELAY_HPP

#include "Relais.hpp"

// Wraps a relay wired in Normally-Closed (NC) configuration so that
// turnOn/turnOff/getState reflect the connected device's actual state,
// not the relay coil state.
class NcRelay {
public:
    NcRelay(uint8_t pin, const String& name) : relay(pin, name) {}

    void turnOn()  { relay.turnOff(); } // de-energize NC contact → device receives power
    void turnOff() { relay.turnOn();  } // energize NC contact   → device loses power

    bool getState() const { return !relay.getState(); }
    String getName() const { return relay.getName(); }

private:
    Relais relay;
};

#endif // NC_RELAY_HPP
