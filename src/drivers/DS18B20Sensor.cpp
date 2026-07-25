#include "drivers/DS18B20Sensor.hpp"
#include "utils/Logger.hpp"

namespace drivers {

DS18B20Sensor::DS18B20Sensor(gpio_num_t pin)
    : pin_(pin), oneWire_(static_cast<uint8_t>(pin)), sensors_(&oneWire_), initialized_(false) {}

bool DS18B20Sensor::init() {
    sensors_.begin();
    int deviceCount = sensors_.getDeviceCount();
    if (deviceCount > 0) {
        sensors_.setWaitForConversion(false); // Non-blocking temperature requests
        initialized_ = true;
        utils::Logger::info("DS18B20", "Found %d DS18B20 device(s) on GPIO%d", deviceCount, static_cast<int>(pin_));
    } else {
        initialized_ = false;
        utils::Logger::error("DS18B20", "No DS18B20 sensors found on GPIO%d!", static_cast<int>(pin_));
    }
    return initialized_;
}

bool DS18B20Sensor::readTemperature(float& outTempC) {
    if (!initialized_) {
        return false;
    }

    sensors_.requestTemperatures();
    float tempC = sensors_.getTempCByIndex(0);

    if (tempC == DEVICE_DISCONNECTED_C || tempC == -127.0f) {
        utils::Logger::warn("DS18B20", "CRC error or sensor disconnected");
        return false;
    }

    outTempC = tempC;
    return true;
}

} // namespace drivers
