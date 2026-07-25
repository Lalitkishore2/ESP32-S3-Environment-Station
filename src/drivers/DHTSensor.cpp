#include "drivers/DHTSensor.hpp"
#include "utils/Logger.hpp"
#include <cmath>

namespace drivers {

DHTSensor::DHTSensor(gpio_num_t pin)
    : pin_(pin), dht_(static_cast<uint8_t>(pin), DHT11), initialized_(false) {}

bool DHTSensor::init() {
    dht_.begin();
    initialized_ = true;
    utils::Logger::info("DHT11", "Initialized DHT11 sensor on GPIO%d", static_cast<int>(pin_));
    return true;
}

bool DHTSensor::readData(float& outTempC, float& outHumidity) {
    if (!initialized_) return false;

    float h = dht_.readHumidity();
    float t = dht_.readTemperature();

    if (std::isnan(h) || std::isnan(t)) {
        utils::Logger::warn("DHT11", "Failed to read valid data from DHT11 sensor!");
        return false;
    }

    outTempC = t;
    outHumidity = h;
    return true;
}

} // namespace drivers
