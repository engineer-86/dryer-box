#ifndef MQTT_HPP
#define MQTT_HPP

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <NetworkCredentials.hpp>

extern PubSubClient mqtt_client;

void connectToBroker(const NetworkCredentials& creds);
void reconnectToBroker(); // uses credentials from the last connectToBroker call

#endif // MQTT_HPP
