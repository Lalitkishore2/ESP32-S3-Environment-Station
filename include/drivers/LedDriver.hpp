#ifndef LED_DRIVER_HPP
#define LED_DRIVER_HPP

#include <driver/gpio.h>
#include <cstdint>

namespace drivers {

enum class LedMode {
    OFF,
    ON,
    BLINK_FAST,  // 100ms on / 100ms off
    BLINK_SLOW,  // 500ms on / 500ms off
    DOUBLE_BLINK // double pulse
};

class LedDriver {
public:
    explicit LedDriver(gpio_num_t pin);
    void init();
    void setMode(LedMode mode);
    void update(); // Non-blocking update called in loop

private:
    gpio_num_t pin_;
    LedMode mode_;
    bool state_;
    uint32_t lastToggleMs_;
    uint8_t blinkStep_;
};

} // namespace drivers

#endif // LED_DRIVER_HPP
