#include "TempHumidity.hpp"
#include <Arduino.h>

TempHumidity::TempHumidity(uint8_t pin, uint8_t type) : dht(pin, type), temperature(0.0f), humidity(0.0f)
{
}

void TempHumidity::setupDHT()
{
  dht.begin();
}

void TempHumidity::updateReadings()
{
  float newHumidity = dht.readHumidity();
  float newTemperature = dht.readTemperature() - 1; // Offset

  if (!isnan(newHumidity) && !isnan(newTemperature))
  {
    humidity = newHumidity;
    temperature = newTemperature;
    Serial.print(F("DHT Monitoring... Humidity: "));
    Serial.print(humidity);
    Serial.print(F("%, Temperature: "));
    Serial.print(temperature);
    Serial.println(F("°C"));
  }
  else
  {
    Serial.println(F("Failed to read from DHT sensor!"));
  }
}

float TempHumidity::getTemperature() const
{
  return temperature;
}

float TempHumidity::getHumidity() const
{
  return humidity;
}

void TempHumidity::setTemperature(float temperature)
{
  this->temperature = temperature;
}

void TempHumidity::setHumidtiy(float humidity)
{
  this->humidity = humidity;
}