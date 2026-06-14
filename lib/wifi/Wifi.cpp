#include "Wifi.hpp"
#include <ESP8266WiFi.h>

bool connectToWifi(const NetworkCredentials& creds, unsigned long timeoutMs) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(creds.wifiSSID.c_str(), creds.wifiPassword.c_str());

    Serial.print("Connecting to WiFi");
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start >= timeoutMs) {
            Serial.println("\nWiFi connection timed out.");
            return false;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nWiFi connected. IP: ");
    Serial.println(WiFi.localIP().toString());
    return true;
}
