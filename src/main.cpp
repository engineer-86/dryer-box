#include <Arduino.h>
#include "temp_humidity.h"

#define HEATER_RELAIS_PIN 5

// put function definitions here:
void toggleRelais()
{
  startDHTMonitoring();
  digitalWrite(HEATER_RELAIS_PIN, HIGH);
  Serial.print("heater on\n");
  delay(1000);
  startDHTMonitoring();
  digitalWrite(HEATER_RELAIS_PIN, LOW);
  Serial.print("heater off\n");
  delay(1000);
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(HEATER_RELAIS_PIN, OUTPUT);
  setupDHT();
  Serial.begin(115200);
}

void loop()
{
  toggleRelais();
  // put your main code here, to run repeatedly:
}
