#ifndef MQTT_HPP
#define MQTT_HPP

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

extern PubSubClient mqtt_client; // Externe Deklaration

void connectToBroker();
void reconnectToBroker();

#endif // MQTT_HPP
