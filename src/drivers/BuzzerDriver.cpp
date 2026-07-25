#include "drivers/BuzzerDriver.hpp"
#include "config/Config.hpp"
#include <Arduino.h>

namespace drivers {

BuzzerDriver::BuzzerDriver(gpio_num_t pin)
    : pin_(pin), pattern_(BeepPattern::NONE), startTimeMs_(0), step_(0), active_(false) {}

void BuzzerDriver::init() {
    pinMode(static_cast<uint8_t>(pin_), OUTPUT);
    stop();
}

void BuzzerDriver::toneOn(uint32_t freqHz) {
    tone(static_cast<uint8_t>(pin_), freqHz);
}

void BuzzerDriver::toneOff() {
    noTone(static_cast<uint8_t>(pin_));
}

void BuzzerDriver::stop() {
    toneOff();
    active_ = false;
    pattern_ = BeepPattern::NONE;
    step_ = 0;
}

void BuzzerDriver::triggerPattern(BeepPattern pattern) {
    pattern_ = pattern;
    startTimeMs_ = millis();
    step_ = 0;
    active_ = true;

    if (pattern_ == BeepPattern::SHORT_BEEP || pattern_ == BeepPattern::LONG_BEEP || pattern_ == BeepPattern::ALERT_PULSE) {
        toneOn(config::ALERT_TONE_FREQ_HZ);
    }
}

void BuzzerDriver::update() {
    if (!active_ || pattern_ == BeepPattern::NONE) return;

    uint32_t elapsed = millis() - startTimeMs_;

    if (pattern_ == BeepPattern::SHORT_BEEP) {
        if (elapsed >= 100) {
            stop();
        }
    } else if (pattern_ == BeepPattern::LONG_BEEP) {
        if (elapsed >= 500) {
            stop();
        }
    } else if (pattern_ == BeepPattern::ALERT_PULSE) {
        if (elapsed >= 150) {
            startTimeMs_ = millis();
            step_++;
            if (step_ >= 4) {
                stop();
            } else {
                if (step_ % 2 == 0) {
                    toneOn(config::ALERT_TONE_FREQ_HZ);
                } else {
                    toneOff();
                }
            }
        }
    }
}

} // namespace drivers
