#include <HeaterSettings.hpp>
#include <TempHumidity.hpp>

HeaterSettings::HeaterSettings(uint8_t tempPin) : tempPin(tempPin),
                                                  targetTemperature(0),
                                                  targetTime(0),
                                                  startTime(0)
{
    setupDHT();
}

void HeaterSettings::setTargetTemperature(uint8_t temperature, Relais heaterRelay)
{
    targetTemperature = temperature;
    // Prüfen, ob das Heizelement aktiviert oder deaktiviert werden soll
    float currentTemp = readCurrentTemperature();
    if (currentTemp < targetTemperature)
    {
        // Heizelement aktivieren
        heaterRelay.turnOn();
        Serial.println("Heizelement wird aktiviert.");
    }
    else
    {
        // Heizelement deaktivieren
        heaterRelay.turnOff();
        Serial.println("Heizelement wird deaktiviert.");
    }
}

void HeaterSettings::setTargetTime(unsigned long time)
{
    targetTime = time;
    startTime = millis();
    Serial.print("Heiztimer gestartet für ");
    Serial.print(time / 60000); // Zeit in Minuten
    Serial.println(" Minuten.");
}

uint8_t HeaterSettings::getTargetTemperature() const
{
    return targetTemperature;
}

unsigned long HeaterSettings::getTargetTime() const
{
    return targetTime;
}

float HeaterSettings::readCurrentTemperature()
{
    float temperature, humidity;
    // Use the existing DHT monitoring function to read the temperature
    startDHTMonitoring(temperature, humidity);
    // Assuming you want to return the temperature as an integer
    return temperature;
}

unsigned long HeaterSettings::computeRemainingTime()
{
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;
    if (elapsedTime >= targetTime)
    {
        // Time has expired
        return 0;
    }
    return targetTime - elapsedTime;
}
