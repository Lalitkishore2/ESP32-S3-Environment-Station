#include "managers/WeatherManager.hpp"
#include "config/Config.hpp"
#include "utils/Logger.hpp"
#include <HTTPClient.h>
#include <ArduinoJson.h>

namespace managers {

WeatherManager::WeatherManager()
    : weatherData_(), updateTimer_(config::WEATHER_UPDATE_INTERVAL_MS) {}

void WeatherManager::init() {
    updateTimer_.start();
    weatherData_.isValid = false;
    utils::Logger::info("Weather", "Weather Manager initialized");
}

void WeatherManager::update(bool wifiConnected) {
    if (!wifiConnected) return;

    if (updateTimer_.isExpired() || !weatherData_.isValid) {
        fetchWeather();
    }
}

bool WeatherManager::fetchWeather() {
    char url[256];
    snprintf(url, sizeof(url),
             "http://api.open-meteo.com/v1/forecast?latitude=%s&longitude=%s&current=temperature_2m,relative_humidity_2m,apparent_temperature,surface_pressure,wind_speed_10m,weather_code",
             config::WEATHER_LATITUDE, config::WEATHER_LONGITUDE);

    utils::Logger::info("Weather", "Fetching weather from Open-Meteo API...");

    HTTPClient http;
    http.begin(url);
    http.setTimeout(5000);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        utils::Logger::error("Weather", "HTTP GET failed, error: %s (code: %d)", http.errorToString(httpCode).c_str(), httpCode);
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        utils::Logger::error("Weather", "JSON Deserialization failed: %s", error.c_str());
        return false;
    }

    JsonObject current = doc["current"];
    if (current.isNull()) {
        utils::Logger::error("Weather", "Invalid JSON format: missing 'current' object");
        return false;
    }

    weatherData_.outdoorTempC  = current["temperature_2m"] | 0.0f;
    weatherData_.humidity      = current["relative_humidity_2m"] | 0.0f;
    weatherData_.feelsLikeC    = current["apparent_temperature"] | 0.0f;
    weatherData_.pressureHpa   = current["surface_pressure"] | 0.0f;
    weatherData_.windSpeedKmh  = current["wind_speed_10m"] | 0.0f;
    weatherData_.weatherCode   = current["weather_code"] | 0;
    weatherData_.isValid       = true;
    weatherData_.lastFetchTimeMs = millis();

    utils::Logger::info("Weather", "Weather updated! Temp: %.1f C, Hum: %.0f%%, Press: %.0f hPa",
                         weatherData_.outdoorTempC, weatherData_.humidity, weatherData_.pressureHpa);

    return true;
}

const models::WeatherData& WeatherManager::getWeatherData() const {
    return weatherData_;
}

} // namespace managers
