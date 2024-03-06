#include <TempHumidity.hpp>
#include <Arduino.h>

DHT dht(DHTPIN, DHTTYPE);

void setupDHT()
{
  dht.begin();
}

void startDHTMonitoring(float &temperature, float &humidity)
{
  float offset_temperature = 1;
  humidity = dht.readHumidity();
  temperature = dht.readTemperature() - offset_temperature;

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    // error values to avoid NaN return value of DHT lib
    temperature = -1.0;
    humidity = -1.0;
    return;
  }

  Serial.print(F("Starting DHT Monitoring... Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C "));
}

float stopDHTMonitoring()
{
  Serial.println(F("Stopping DHT Monitoring."));
  return 0.0;
}
