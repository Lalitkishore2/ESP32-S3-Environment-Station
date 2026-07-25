#include "managers/TimeManager.hpp"
#include "config/Config.hpp"
#include "utils/Logger.hpp"
#include <time.h>
#include <Arduino.h>

namespace managers {

TimeManager::TimeManager() : synced_(false) {}

void TimeManager::init() {
    configTime(config::GMT_OFFSET_SEC, config::DAYLIGHT_OFFSET_SEC,
               config::NTP_SERVER_1, config::NTP_SERVER_2);
    utils::Logger::info("Time", "NTP Time sync configured");
}

bool TimeManager::syncNTP() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 1000)) { // non-blocking 1s timeout check
        synced_ = false;
        utils::Logger::warn("Time", "NTP synchronization failed or waiting for time sync");
        return false;
    }
    synced_ = true;
    utils::Logger::info("Time", "NTP synchronized successfully!");
    return true;
}

void TimeManager::getFormattedTime(char* timeBuffer, size_t bufLen) const {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10)) {
        strftime(timeBuffer, bufLen, "%H:%M:%S", &timeinfo);
    } else {
        snprintf(timeBuffer, bufLen, "--:--:--");
    }
}

void TimeManager::getFormattedDate(char* dateBuffer, size_t bufLen) const {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10)) {
        strftime(dateBuffer, bufLen, "%a, %d %b %Y", &timeinfo);
    } else {
        snprintf(dateBuffer, bufLen, "Sat, 25 Jul 2026");
    }
}

bool TimeManager::isSynced() const {
    return synced_;
}

} // namespace managers
