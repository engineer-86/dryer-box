#include <mqtt.hpp>
#include <credentials.hpp>

WiFiClient espClient;
PubSubClient mqtt_client(espClient);
PubSubClient connectToBroker()
{
    Credentials broker_credentials;
    mqtt_client.setServer(broker_credentials.getBrokerIP(),
                          broker_credentials.getBrokerPort());

    while (!mqtt_client.connected())
    {
        String client_id = "Irrigation-";

        if (mqtt_client.connect(client_id.c_str(), broker_credentials.getBrokerUsername(),
                                broker_credentials.getBrokerPassword()))
        {
        }
        else
        {
            Serial.print("failed with state ");
            Serial.print(mqtt_client.state());
            delay(2000);
        }
    }

    return mqtt_client;
}