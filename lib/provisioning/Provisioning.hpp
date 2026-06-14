#ifndef PROVISIONING_HPP
#define PROVISIONING_HPP

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <NetworkCredentials.hpp>

class Provisioning {
public:
    Provisioning();

    // Mounts LittleFS and loads stored credentials.
    // Returns true  → credentials found, caller may proceed with normal operation.
    // Returns false → no valid credentials; AP mode is started and blocks until restart.
    bool begin();

    const NetworkCredentials& getCredentials() const { return credentials; }

    // Delete stored credentials so the next boot enters AP mode.
    static void clearCredentials();

    // Call this once the device has been running normally for >10 s.
    // Resets the rapid-power-cycle counter so it doesn't accumulate across
    // normal reboots.
    void clearBootCounter();

private:
    NetworkCredentials credentials;
    ESP8266WebServer   server;
    DNSServer          dnsServer;

    bool loadCredentials();
    void saveCredentials(const NetworkCredentials& creds);
    void startAPMode();

    void handleRoot();
    void handleSave();
    void handleNotFound();

    // Boot-counter helpers
    uint8_t readBootCount();
    void    writeBootCount(uint8_t count);

    static constexpr const char* CREDENTIALS_FILE = "/credentials.json";
    static constexpr const char* BOOT_COUNT_FILE  = "/boot_count.dat";
    static constexpr const char* AP_SSID          = "Dryer-Setup";
    static constexpr uint8_t     DNS_PORT          = 53;
    static constexpr uint8_t     RESET_BOOT_COUNT  = 5;
};

#endif // PROVISIONING_HPP
