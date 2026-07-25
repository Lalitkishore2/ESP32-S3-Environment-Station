#ifndef WEATHER_MANAGER_HPP
#define WEATHER_MANAGER_HPP

#include "models/WeatherData.hpp"
#include "utils/NonBlockingTimer.hpp"

namespace managers {

class WeatherManager {
public:
    WeatherManager();
    void init();
    bool fetchWeather();
    void update(bool wifiConnected); // Periodically check fetch timer
    [[nodiscard]] const models::WeatherData& getWeatherData() const;

private:
    models::WeatherData weatherData_;
    utils::NonBlockingTimer updateTimer_;
};

} // namespace managers

#endif // WEATHER_MANAGER_HPP
