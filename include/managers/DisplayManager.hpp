#ifndef DISPLAY_MANAGER_HPP
#define DISPLAY_MANAGER_HPP

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "models/TelemetryData.hpp"
#include "models/WeatherData.hpp"
#include "driver/gpio.h"

namespace managers {

class DisplayManager {
public:
    DisplayManager(gpio_num_t csPin, gpio_num_t dcPin, gpio_num_t rstPin,
                   gpio_num_t mosiPin, gpio_num_t sckPin, gpio_num_t backlightPin);
    void init();
    void setBacklight(bool enable);
    void showSplashScreen();
    void renderDashboard(const char* timeStr, const char* dateStr, bool wifiConnected, const char* ipStr,
                         const models::TelemetryData& telemetry,
                         const models::WeatherData& weather,
                         const char* statusMsg);

private:
    void drawCentered(const char* text, int y, uint8_t size, uint16_t color);
    void flushCanvas();

    gpio_num_t csPin_;
    gpio_num_t dcPin_;
    gpio_num_t rstPin_;
    gpio_num_t mosiPin_;
    gpio_num_t sckPin_;
    gpio_num_t backlightPin_;

    Adafruit_ST7735 tft_;
    GFXcanvas16 canvas_;
    bool backlightState_;
};

} // namespace managers

#endif // DISPLAY_MANAGER_HPP
