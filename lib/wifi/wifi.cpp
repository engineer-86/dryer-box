/**
 * https://starthardware.org/arduino-wifi-mit-dem-esp8266-als-arduino/
 */
#include <wifi.hpp>
#include <credentials.hpp>
#include <ESP8266WiFi.h>

// Credentials credentials;
void connectTohWifi()
{

    Credentials wifi_credentials;
    WiFi.begin(wifi_credentials.getWifiSSID(),
               wifi_credentials.getWifiPassword());

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("Versuche zu verbinden mit WIFI.\n");
        delay(500);
    }
    Serial.println("");
    Serial.println("WiFi verbunden\n");
}