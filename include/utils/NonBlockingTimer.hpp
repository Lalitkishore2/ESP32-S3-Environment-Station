#ifndef NON_BLOCKING_TIMER_HPP
#define NON_BLOCKING_TIMER_HPP

#include <Arduino.h>

namespace utils {

class NonBlockingTimer {
public:
    explicit NonBlockingTimer(uint32_t intervalMs = 1000)
        : intervalMs_(intervalMs), lastTickMs_(0), running_(false) {}

    void start() {
        lastTickMs_ = millis();
        running_ = true;
    }

    void stop() {
        running_ = false;
    }

    void setInterval(uint32_t intervalMs) {
        intervalMs_ = intervalMs;
    }

    [[nodiscard]] uint32_t getInterval() const {
        return intervalMs_;
    }

    bool isExpired() {
        if (!running_) return false;
        uint32_t currentMs = millis();
        if (currentMs - lastTickMs_ >= intervalMs_) {
            lastTickMs_ = currentMs;
            return true;
        }
        return false;
    }

    void reset() {
        lastTickMs_ = millis();
    }

    [[nodiscard]] bool isRunning() const {
        return running_;
    }

private:
    uint32_t intervalMs_;
    uint32_t lastTickMs_;
    bool     running_;
};

} // namespace utils

#endif // NON_BLOCKING_TIMER_HPP
