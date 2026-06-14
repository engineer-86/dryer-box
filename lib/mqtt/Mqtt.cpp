#include "Mqtt.hpp"
#include <Credentials.hpp>
#include <Wifi.hpp>

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void connectToBroker()
{
    Credentials broker_credentials;
    mqtt_client.setServer(broker_credentials.getBrokerIP(), broker_credentials.getBrokerPort());

    while (!mqtt_client.connected())
    {
        String client_id = "dryer-" + String(random(0xffff), HEX);
        Serial.print("Connecting to MQTT broker...");

        if (mqtt_client.connect(client_id.c_str(),
                                broker_credentials.getBrokerUsername(),
                                broker_credentials.getBrokerPassword()))
        {
            Serial.println("connected");
            mqtt_client.subscribe("cmnd/dryer/filament");
            mqtt_client.subscribe("cmnd/dryer/heater");
            mqtt_client.subscribe("cmnd/dryer/fan");
        }
        else
        {
            Serial.print("Connection failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" — retrying in 5 seconds");
            delay(5000);
        }
    }
}

void reconnectToBroker()
{
    if (!mqtt_client.connected())
    {
        connectToBroker();
    }
}
