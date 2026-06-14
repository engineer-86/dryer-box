#ifndef NETWORK_CREDENTIALS_HPP
#define NETWORK_CREDENTIALS_HPP

#include <Arduino.h>

struct NetworkCredentials {
    String   wifiSSID;
    String   wifiPassword;
    String   brokerIP;
    uint16_t brokerPort      = 1883;
    String   brokerUser;
    String   brokerPassword;

    bool isValid() const {
        return wifiSSID.length() > 0 && brokerIP.length() > 0;
    }
};

#endif // NETWORK_CREDENTIALS_HPP
