#ifndef BUZZER_DRIVER_HPP
#define BUZZER_DRIVER_HPP

#include <driver/gpio.h>
#include <cstdint>

namespace drivers {

enum class BeepPattern {
    NONE,
    SHORT_BEEP,
    LONG_BEEP,
    ALERT_PULSE
};

class BuzzerDriver {
public:
    explicit BuzzerDriver(gpio_num_t pin);
    void init();
    void triggerPattern(BeepPattern pattern);
    void update(); // Non-blocking tick in main loop
    void stop();

private:
    void toneOn(uint32_t freqHz);
    void toneOff();

    gpio_num_t pin_;
    BeepPattern pattern_;
    uint32_t startTimeMs_;
    uint8_t step_;
    bool active_;
};

} // namespace drivers

#endif // BUZZER_DRIVER_HPP
