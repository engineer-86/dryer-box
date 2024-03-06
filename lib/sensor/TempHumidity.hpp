#ifndef TEMP_HUMIDITY_H
#define TEMP_HUMIDITY_H

#include <DHT.h>
#include <pins.hpp>

void setupDHT();
void startDHTMonitoring(float &temperature, float &humidity);
float stopDHTMonitoring();

#endif // TEMP_HUMIDITY_H
