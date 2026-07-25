#include "drivers/LedDriver.hpp"
#include <Arduino.h>

namespace drivers {

LedDriver::LedDriver(gpio_num_t pin)
    : pin_(pin), mode_(LedMode::OFF), state_(false), lastToggleMs_(0), blinkStep_(0) {}

void LedDriver::init() {
    pinMode(static_cast<uint8_t>(pin_), OUTPUT);
    digitalWrite(static_cast<uint8_t>(pin_), LOW);
    state_ = false;
}

void LedDriver::setMode(LedMode mode) {
    if (mode_ == mode) return;
    mode_ = mode;
    lastToggleMs_ = millis();
    blinkStep_ = 0;

    if (mode_ == LedMode::OFF) {
        state_ = false;
        digitalWrite(static_cast<uint8_t>(pin_), LOW);
    } else if (mode_ == LedMode::ON) {
        state_ = true;
        digitalWrite(static_cast<uint8_t>(pin_), HIGH);
    }
}

void LedDriver::update() {
    if (mode_ == LedMode::OFF || mode_ == LedMode::ON) return;

    uint32_t now = millis();

    if (mode_ == LedMode::BLINK_FAST) {
        if (now - lastToggleMs_ >= 100) {
            lastToggleMs_ = now;
            state_ = !state_;
            digitalWrite(static_cast<uint8_t>(pin_), state_ ? HIGH : LOW);
        }
    } else if (mode_ == LedMode::BLINK_SLOW) {
        if (now - lastToggleMs_ >= 500) {
            lastToggleMs_ = now;
            state_ = !state_;
            digitalWrite(static_cast<uint8_t>(pin_), state_ ? HIGH : LOW);
        }
    } else if (mode_ == LedMode::DOUBLE_BLINK) {
        // Step 0: ON 100ms, Step 1: OFF 100ms, Step 2: ON 100ms, Step 3: OFF 700ms
        uint32_t stepDuration = (blinkStep_ == 3) ? 700 : 100;
        if (now - lastToggleMs_ >= stepDuration) {
            lastToggleMs_ = now;
            blinkStep_ = (blinkStep_ + 1) % 4;
            state_ = (blinkStep_ == 0 || blinkStep_ == 2);
            digitalWrite(static_cast<uint8_t>(pin_), state_ ? HIGH : LOW);
        }
    }
}

} // namespace drivers
