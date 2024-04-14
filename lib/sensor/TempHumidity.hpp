#ifndef TEMP_HUMIDITY_H
#define TEMP_HUMIDITY_H

#include <DHT.h>

class TempHumidity
{
public:
    TempHumidity(uint8_t pin, uint8_t type);
    void setupDHT();
    void updateReadings();
    float getTemperature() const;
    float getHumidity() const;
    void setTemperature(float temperature);
    void setHumidtiy(float humidity);

private:
    DHT dht;
    float temperature;
    float humidity;
};

#endif // TEMP_HUMIDITY_H
