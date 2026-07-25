#include "managers/SensorManager.hpp"
#include "config/Config.hpp"
#include "utils/Logger.hpp"

namespace managers {

SensorManager::SensorManager(gpio_num_t ds18b20Pin, gpio_num_t dhtPin)
    : ds18b20_(ds18b20Pin),
      dht11_(dhtPin),
      telemetry_(),
      pollTimer_(config::SENSOR_POLL_INTERVAL_MS) {}

void SensorManager::init() {
    ds18b20_.init();
    dht11_.init();
    pollTimer_.start();
    update(); // Perform immediate initial read
}

void SensorManager::update() {
    if (!pollTimer_.isExpired() && telemetry_.lastReadTimestamp != 0) {
        return;
    }

    float dsTemp = 0.0f;
    bool dsOk = ds18b20_.readTemperature(dsTemp);

    float dhtTemp = 0.0f;
    float dhtHum = 0.0f;
    bool dhtOk = dht11_.readData(dhtTemp, dhtHum);

    telemetry_.ds18b20Valid = dsOk;
    if (dsOk) {
        telemetry_.ds18b20TempC = dsTemp;
    }

    telemetry_.dhtValid = dhtOk;
    if (dhtOk) {
        telemetry_.dhtTempC = dhtTemp;
        telemetry_.dhtHumidity = dhtHum;
    }

    telemetry_.lastReadTimestamp = millis();

    utils::Logger::debug("SensorMgr", "DS18B20: %.2f C (%s), DHT11: %.2f C / %.1f%% (%s)",
                         telemetry_.ds18b20TempC, dsOk ? "OK" : "ERR",
                         telemetry_.dhtTempC, telemetry_.dhtHumidity, dhtOk ? "OK" : "ERR");
}

const models::TelemetryData& SensorManager::getTelemetry() const {
    return telemetry_;
}

} // namespace managers
