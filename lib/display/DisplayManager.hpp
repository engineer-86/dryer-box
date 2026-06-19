#ifndef DISPLAY_MANAGER_HPP
#define DISPLAY_MANAGER_HPP

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

class DisplayManager {
public:
    DisplayManager(uint8_t sdaPin, uint8_t sclPin);

    void begin();

    void update(const char* state,
                float       currentTemp,
                uint8_t     targetTemp,
                float       humidity,
                uint32_t    remainingMinutes,
                bool        heaterOn,
                bool        fanOn,
                const char* selectedPreset = nullptr);

    // Show a full-screen message (AP mode, WiFi connecting, etc.)
    void showMessage(const char* line1, const char* line2 = nullptr);

private:
    uint8_t _sdaPin;
    uint8_t _sclPin;
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

    void rewire();
    void scanI2C();
    void drawContent(const char* state, float currentTemp, uint8_t targetTemp,
                     float humidity, uint32_t remainingMinutes,
                     bool heaterOn, bool fanOn, const char* selectedPreset);
};

#endif // DISPLAY_MANAGER_HPP
