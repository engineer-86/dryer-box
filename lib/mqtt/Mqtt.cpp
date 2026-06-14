#include "Mqtt.hpp"

WiFiClient   espClient;
PubSubClient mqtt_client(espClient);

// Retained so reconnectToBroker() can re-use them without re-passing through main
static NetworkCredentials storedCreds;

void connectToBroker(const NetworkCredentials& creds) {
    storedCreds = creds;
    mqtt_client.setServer(creds.brokerIP.c_str(), creds.brokerPort);

    while (!mqtt_client.connected()) {
        String client_id = "dryer-" + String(random(0xffff), HEX);
        Serial.print("Connecting to MQTT broker...");

        if (mqtt_client.connect(client_id.c_str(),
                                creds.brokerUser.c_str(),
                                creds.brokerPassword.c_str())) {
            Serial.println("connected");
            mqtt_client.subscribe("cmnd/dryer/filament");
            mqtt_client.subscribe("cmnd/dryer/heater");
            mqtt_client.subscribe("cmnd/dryer/fan");
            mqtt_client.subscribe("cmnd/dryer/config");
            mqtt_client.subscribe("cmnd/dryer/control");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" — retrying in 5s");
            delay(5000);
        }
    }
}

void reconnectToBroker() {
    if (!mqtt_client.connected()) {
        connectToBroker(storedCreds);
    }
}
