#ifndef MQTT_H
#define MQTT_H

/**
 * https://pubsubclient.knolleary.net/api
 */

#include <PubSubClient.h>
#include <ESP8266WiFi.h>

PubSubClient connectToBroker();
void publish_state();

#endif /* MQTT_H */