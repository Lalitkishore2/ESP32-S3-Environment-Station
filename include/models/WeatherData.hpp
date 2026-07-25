#ifndef WEATHER_DATA_HPP
#define WEATHER_DATA_HPP

#include <cstdint>

namespace models {

struct WeatherData {
    float outdoorTempC   = 0.0f;
    float feelsLikeC     = 0.0f;
    float humidity       = 0.0f;
    float pressureHpa    = 0.0f;
    float windSpeedKmh   = 0.0f;
    int16_t weatherCode  = 0;
    
    char weatherCondition[32] = "Unknown";
    char lastUpdatedTime[16]  = "--:--";

    bool isValid         = false;
    uint32_t lastFetchTimeMs = 0;
};

} // namespace models

#endif // WEATHER_DATA_HPP
