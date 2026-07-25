#include "managers/AlarmManager.hpp"
#include "config/Config.hpp"
#include "utils/Logger.hpp"
#include <Arduino.h>

namespace managers {

AlarmManager::AlarmManager(drivers::LedDriver& greenLed, drivers::LedDriver& whiteLed, drivers::BuzzerDriver& buzzer)
    : greenLed_(greenLed), whiteLed_(whiteLed), buzzer_(buzzer), alertActive_(false), lastAlertTimeMs_(0) {}

void AlarmManager::init() {
    greenLed_.init();
    whiteLed_.init();
    buzzer_.init();
    whiteLed_.setMode(drivers::LedMode::ON); // Display Enabled
    utils::Logger::info("Alarm", "Alarm Manager initialized");
}

void AlarmManager::evaluateSystemState(bool wifiConnected, bool weatherUpdated, const models::TelemetryData& telemetry) {
    // 1. Green LED logic
    if (wifiConnected && weatherUpdated) {
        greenLed_.setMode(drivers::LedMode::ON); // Healthy system
    } else if (!wifiConnected) {
        greenLed_.setMode(drivers::LedMode::BLINK_FAST); // Connecting/Retrying
    } else {
        greenLed_.setMode(drivers::LedMode::BLINK_SLOW);
    }

    // 2. White LED & Sensor Error logic
    bool sensorError = !telemetry.ds18b20Valid || !telemetry.dhtValid;
    if (sensorError) {
        whiteLed_.setMode(drivers::LedMode::DOUBLE_BLINK);
    } else {
        whiteLed_.setMode(drivers::LedMode::ON);
    }

    // 3. Buzzer Alert logic for out-of-bound conditions (non-continuous)
    uint32_t now = millis();
    bool outOfBounds = false;

    if (telemetry.ds18b20Valid) {
        if (telemetry.ds18b20TempC > config::HIGH_TEMP_ALERT_C || telemetry.ds18b20TempC < config::LOW_TEMP_ALERT_C) {
            outOfBounds = true;
        }
    }
    if (telemetry.dhtValid) {
        if (telemetry.dhtHumidity > config::HIGH_HUMIDITY_ALERT || telemetry.dhtHumidity < config::LOW_HUMIDITY_ALERT) {
            outOfBounds = true;
        }
    }

    if ((sensorError || outOfBounds) && (now - lastAlertTimeMs_ >= 30000)) { // Alarm max once per 30s
        lastAlertTimeMs_ = now;
        buzzer_.triggerPattern(drivers::BeepPattern::ALERT_PULSE);
        utils::Logger::warn("Alarm", "Triggered Audio Alert! SensorErr: %d, OutOfBounds: %d", sensorError, outOfBounds);
    }
}

} // namespace managers
