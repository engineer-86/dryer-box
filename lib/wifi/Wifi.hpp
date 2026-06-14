#ifndef WIFI_H
#define WIFI_H

#include <NetworkCredentials.hpp>

// Returns true on success, false if connection times out.
bool connectToWifi(const NetworkCredentials& creds, unsigned long timeoutMs = 30000);

#endif // WIFI_H
