#include "managers/DisplayManager.hpp"
#include "utils/Logger.hpp"
#include <Arduino.h>
#include <cstdio>
#include <cstring>
#include <cstdarg>

namespace managers {

DisplayManager::DisplayManager(gpio_num_t csPin, gpio_num_t dcPin, gpio_num_t rstPin,
                               gpio_num_t mosiPin, gpio_num_t sckPin, gpio_num_t backlightPin)
    : csPin_(csPin),
      dcPin_(dcPin),
      rstPin_(rstPin),
      mosiPin_(mosiPin),
      sckPin_(sckPin),
      backlightPin_(backlightPin),
      tft_(static_cast<int8_t>(csPin), static_cast<int8_t>(dcPin), static_cast<int8_t>(rstPin)),
      canvas_(128, 160),
      backlightState_(true) {}

void DisplayManager::init() {
    pinMode(static_cast<uint8_t>(backlightPin_), OUTPUT);
    digitalWrite(static_cast<uint8_t>(backlightPin_), HIGH);

    // Initialize SPI bus for ESP32-S3 (SCK, MISO=-1, MOSI, CS)
    SPI.begin(static_cast<int8_t>(sckPin_), -1, static_cast<int8_t>(mosiPin_), static_cast<int8_t>(csPin_));

    // Initialize Adafruit ST7735 driver
    tft_.initR(INITR_BLACKTAB);
    tft_.setSPISpeed(8000000);
    tft_.setRotation(0); // Portrait 128x160
    tft_.fillScreen(ST77XX_BLACK);

    utils::Logger::info("Display", "ST7735 Display initialized");
}

void DisplayManager::setBacklight(bool enable) {
    backlightState_ = enable;
    digitalWrite(static_cast<uint8_t>(backlightPin_), enable ? HIGH : LOW);
}

void DisplayManager::flushCanvas() {
    tft_.drawRGBBitmap(0, 0, canvas_.getBuffer(), 128, 160);
}

void DisplayManager::drawCentered(const char* text, int y, uint8_t size, uint16_t color) {
    if (!text) return;
    int len = strlen(text);
    int totalWidth = len * 6 * size - size;
    int x = (128 - totalWidth) / 2;
    if (x < 0) x = 0;
    canvas_.setTextSize(size);
    canvas_.setTextColor(color);
    canvas_.setCursor(x, y);
    canvas_.print(text);
}

void DisplayManager::showSplashScreen() {
    canvas_.fillScreen(0x0821); // Dark Slate
    canvas_.fillRoundRect(6, 20, 116, 120, 8, 0x10A2);
    canvas_.drawRoundRect(6, 20, 116, 120, 8, 0x07FF);
    
    drawCentered("ESP32-S3", 35, 2, ST77XX_WHITE);
    drawCentered("ENVIRONMENT", 62, 1, 0x07FF);
    drawCentered("STATION", 76, 1, 0x07FF);
    drawCentered("Starting...", 110, 1, 0x07E0);

    flushCanvas();
}

void DisplayManager::renderDashboard(const char* timeStr, const char* dateStr, bool wifiConnected, const char* ipStr,
                                     const models::TelemetryData& telemetry,
                                     const models::WeatherData& weather,
                                     const char* statusMsg) {
    // 0. Background
    canvas_.fillScreen(0x0821); // Deep Charcoal Slate

    // 1. Header Bar (0 - 18px)
    canvas_.fillRect(0, 0, 128, 18, 0x000B); // Deep Blue Navy
    canvas_.setTextColor(ST77XX_YELLOW);
    canvas_.setTextSize(1);
    canvas_.setCursor(5, 5);
    canvas_.print(timeStr ? timeStr : "00:00:00");

    // WiFi Pill Badge (X: 86, Width: 37, Height: 12)
    uint16_t wifiBg = wifiConnected ? 0x04e0 : 0x7800; // Emerald Green or Red
    canvas_.fillRoundRect(86, 3, 37, 12, 3, wifiBg);
    canvas_.setTextColor(ST77XX_WHITE);
    canvas_.setCursor(91, 5);
    canvas_.print(wifiConnected ? "WIFI" : "OFF");

    char buf[32];

    // 2. Indoor Climate Card Container (20 - 82px)
    canvas_.fillRoundRect(3, 20, 122, 62, 5, 0x10A2); // Dark Navy Blue Box
    canvas_.drawRoundRect(3, 20, 122, 62, 5, 0x07FF); // Cyan Accent Border
    
    canvas_.setTextColor(0x07FF); // Cyan Header
    canvas_.setTextSize(1);
    canvas_.setCursor(8, 23);
    canvas_.print("INDOOR CLIMATE");

    // Left Column: Large DS18B20 Temp Number (X: 8..56)
    if (telemetry.ds18b20Valid) {
        snprintf(buf, sizeof(buf), "%.1f", telemetry.ds18b20TempC);
    } else {
        snprintf(buf, sizeof(buf), "--.-");
    }
    canvas_.setTextColor(ST77XX_WHITE);
    canvas_.setTextSize(2); // Large Temp Number (4 chars = 48px wide)
    canvas_.setCursor(8, 35);
    canvas_.print(buf);

    canvas_.setTextSize(1);
    canvas_.setTextColor(0x07FF);
    canvas_.setCursor(52, 35);
    canvas_.print("C");

    canvas_.setTextColor(0x9E79); // Muted Label
    canvas_.setCursor(8, 53);
    canvas_.print("DS Probe");

    // Right Column: DHT Temp & Humidity (X: 62..120)
    canvas_.setTextColor(0xC618); // Light Grey
    if (telemetry.dhtValid) {
        snprintf(buf, sizeof(buf), "DHT:%.1fC", telemetry.dhtTempC);
        canvas_.setCursor(62, 35);
        canvas_.print(buf);

        snprintf(buf, sizeof(buf), "HUM:%d%%", (int)telemetry.dhtHumidity);
        canvas_.setCursor(62, 47);
        canvas_.print(buf);

        // Humidity Gauge Bar (Width: 54px)
        canvas_.drawRoundRect(62, 58, 56, 6, 2, 0x39E7);
        int barW = (int)((telemetry.dhtHumidity * 52.0f) / 100.0f);
        if (barW > 52) barW = 52;
        if (barW > 0) {
            canvas_.fillRoundRect(64, 60, barW, 2, 1, 0x07E0);
        }
    } else {
        canvas_.setCursor(62, 35);
        canvas_.print("DHT: ERR");
    }

    // Status message inside Indoor Card bottom line
    canvas_.setTextColor(0x07E0); // Green
    canvas_.setCursor(8, 68);
    canvas_.print(statusMsg ? statusMsg : "System Healthy");

    // 3. Outdoor Weather Card Container (85 - 141px)
    canvas_.fillRoundRect(3, 85, 122, 56, 5, 0x1800); // Warm Dark Grey Box
    canvas_.drawRoundRect(3, 85, 122, 56, 5, 0xFD20); // Warm Orange Border

    canvas_.setTextColor(0xFD20); // Orange Header
    canvas_.setCursor(8, 89);
    canvas_.print("OUTDOOR (Chennai)");

    if (weather.isValid) {
        // Left Column: Large Outdoor Temp Number (X: 8..56)
        snprintf(buf, sizeof(buf), "%.1f", weather.outdoorTempC);
        canvas_.setTextColor(ST77XX_WHITE);
        canvas_.setTextSize(2); // Large Outdoor Temp (4 chars = 48px wide)
        canvas_.setCursor(8, 101);
        canvas_.print(buf);

        canvas_.setTextSize(1);
        canvas_.setTextColor(0xFD20);
        canvas_.setCursor(52, 101);
        canvas_.print("C");

        // Right Column: Feels Like, Pressure, Wind
        canvas_.setTextColor(0xC618);
        snprintf(buf, sizeof(buf), "Feels:%.0fC", weather.feelsLikeC);
        canvas_.setCursor(62, 101);
        canvas_.print(buf);

        snprintf(buf, sizeof(buf), "P:%.0fhPa", weather.pressureHpa);
        canvas_.setCursor(62, 115);
        canvas_.print(buf);

        snprintf(buf, sizeof(buf), "W:%.1fkm/h", weather.windSpeedKmh);
        canvas_.setCursor(62, 127);
        canvas_.print(buf);
    } else {
        canvas_.setTextSize(1);
        canvas_.setTextColor(0xC618);
        canvas_.setCursor(8, 108);
        canvas_.print("Syncing Weather...");
    }

    // 4. Bottom Footer: Display envstation.local domain URL
    canvas_.fillRect(0, 143, 128, 17, 0x0000); // Black Footer
    canvas_.setTextColor(0x3660); // Bright Mint Green
    canvas_.setTextSize(1);
    if (wifiConnected) {
        drawCentered("envstation.local", 147, 1, 0x3660);
    } else {
        drawCentered("Connecting...", 147, 1, 0xC618);
    }

    // Flush frame buffer to physical SPI display (flicker-free)
    flushCanvas();
}

} // namespace managers
