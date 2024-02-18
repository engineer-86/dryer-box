#include "temp_humidity.h"
#include <Arduino.h>

DHT dht(DHTPIN, DHTTYPE);

void setupDHT()
{
  dht.begin();
}

float startDHTMonitoring()
{
  float offset_temperature = 1;
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature() - offset_temperature;

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return -1.0;
  }

  Serial.print(F("Starting DHT Monitoring... Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C "));

  return temperature;
}

float stopDHTMonitoring()
{
  Serial.println(F("Stopping DHT Monitoring."));
  return 0.0;
}
