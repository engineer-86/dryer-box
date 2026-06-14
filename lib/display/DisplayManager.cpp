#include "DisplayManager.hpp"

DisplayManager::DisplayManager(uint8_t sdaPin, uint8_t sclPin)
    : _sdaPin(sdaPin), _sclPin(sclPin), u8g2(U8G2_R0, U8X8_PIN_NONE)
{}

void DisplayManager::begin() {
    Wire.begin(_sdaPin, _sclPin);
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.sendBuffer();   // push empty buffer to wipe display RAM
    delay(50);
    u8g2.setContrast(200);
    showMessage("Dryer Box", "Starting...");
}

void DisplayManager::update(const char* state,
                            float       currentTemp,
                            uint8_t     targetTemp,
                            float       humidity,
                            uint32_t    remainingMinutes,
                            bool        heaterOn,
                            bool        fanOn,
                            const char* selectedPreset) {
    char buf[28];
    u8g2.clearBuffer();

    // -- State header (bold, top bar) --
    u8g2.setFont(u8g2_font_7x14B_tf);
    if (selectedPreset) {
        snprintf(buf, sizeof(buf), "> %s", selectedPreset);
        u8g2.drawStr(0, 13, buf);
    } else {
        u8g2.drawStr(0, 13, state);
    }

    // Divider
    u8g2.drawHLine(0, 16, 128);

    // -- Temperature: current / target --
    u8g2.setFont(u8g2_font_6x10_tf);
    snprintf(buf, sizeof(buf), "Temp: %.1f / %d C", currentTemp, targetTemp);
    u8g2.drawStr(0, 28, buf);

    // -- Humidity --
    snprintf(buf, sizeof(buf), "Humi: %.0f %%", humidity);
    u8g2.drawStr(0, 40, buf);

    // -- Remaining time --
    if (remainingMinutes > 0) {
        uint32_t h = remainingMinutes / 60;
        uint32_t m = remainingMinutes % 60;
        if (h > 0)
            snprintf(buf, sizeof(buf), "Time: %dh %02dm left", h, m);
        else
            snprintf(buf, sizeof(buf), "Time: %dm left", m);
    } else {
        snprintf(buf, sizeof(buf), "Time: --");
    }
    u8g2.drawStr(0, 52, buf);

    // -- Status icons bottom right --
    drawStatusBar(heaterOn, fanOn);

    u8g2.sendBuffer();
}

void DisplayManager::showMessage(const char* line1, const char* line2) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x14B_tf);
    u8g2.drawStr(0, 22, line1);
    if (line2) {
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.drawStr(0, 40, line2);
    }
    u8g2.sendBuffer();
}

void DisplayManager::drawStatusBar(bool heaterOn, bool fanOn) {
    // Small icons bottom-right: H=heater F=fan, filled = ON
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr(88, 63, heaterOn ? "[H]" : " H ");
    u8g2.drawStr(108, 63, fanOn   ? "[F]" : " F ");
}
