#include "Wifi.hpp"
#include <Credentials.hpp>
#include <ESP8266WiFi.h>

void connectToWifi()
{
    Credentials wifi_credentials;
    WiFi.begin(wifi_credentials.getWifiSSID(), wifi_credentials.getWifiPassword());

    Serial.print("Verbinden mit WIFI");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi verbunden");
}
