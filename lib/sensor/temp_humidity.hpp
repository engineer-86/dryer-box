#ifndef TEMP_HUMIDITY_H
#define TEMP_HUMIDITY_H

#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

void setupDHT();
void startDHTMonitoring(float &temperature, float &humidity);
float stopDHTMonitoring();

#endif // TEMP_HUMIDITY_H
