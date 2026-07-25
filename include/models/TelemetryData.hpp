#ifndef TELEMETRY_DATA_HPP
#define TELEMETRY_DATA_HPP

#include <cstdint>

namespace models {

struct TelemetryData {
    float ds18b20TempC = 0.0f;
    float dhtTempC     = 0.0f;
    float dhtHumidity  = 0.0f;

    bool ds18b20Valid  = false;
    bool dhtValid      = false;

    uint32_t lastReadTimestamp = 0;
};

} // namespace models

#endif // TELEMETRY_DATA_HPP
