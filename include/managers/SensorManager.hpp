#ifndef SENSOR_MANAGER_HPP
#define SENSOR_MANAGER_HPP

#include "drivers/DS18B20Sensor.hpp"
#include "drivers/DHTSensor.hpp"
#include "models/TelemetryData.hpp"
#include "utils/NonBlockingTimer.hpp"

namespace managers {

class SensorManager {
public:
    SensorManager(gpio_num_t ds18b20Pin, gpio_num_t dhtPin);
    void init();
    void update(); // Called in loop
    [[nodiscard]] const models::TelemetryData& getTelemetry() const;

private:
    drivers::DS18B20Sensor ds18b20_;
    drivers::DHTSensor dht11_;
    models::TelemetryData telemetry_;
    utils::NonBlockingTimer pollTimer_;
};

} // namespace managers

#endif // SENSOR_MANAGER_HPP
