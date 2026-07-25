#ifndef ALARM_MANAGER_HPP
#define ALARM_MANAGER_HPP

#include "drivers/LedDriver.hpp"
#include "drivers/BuzzerDriver.hpp"
#include "models/TelemetryData.hpp"

namespace managers {

class AlarmManager {
public:
    AlarmManager(drivers::LedDriver& greenLed, drivers::LedDriver& whiteLed, drivers::BuzzerDriver& buzzer);
    void init();
    void evaluateSystemState(bool wifiConnected, bool weatherUpdated, const models::TelemetryData& telemetry);

private:
    drivers::LedDriver& greenLed_;
    drivers::LedDriver& whiteLed_;
    drivers::BuzzerDriver& buzzer_;
    
    bool alertActive_;
    uint32_t lastAlertTimeMs_;
};

} // namespace managers

#endif // ALARM_MANAGER_HPP
