#ifndef APP_CONTROLLER_HPP
#define APP_CONTROLLER_HPP

#include "config/PinDefinitions.hpp"
#include "drivers/LedDriver.hpp"
#include "drivers/BuzzerDriver.hpp"
#include "managers/SensorManager.hpp"
#include "managers/DisplayManager.hpp"
#include "managers/NetworkManager.hpp"
#include "managers/TimeManager.hpp"
#include "managers/WeatherManager.hpp"
#include "managers/AlarmManager.hpp"
#include "managers/WebServerManager.hpp"
#include "utils/NonBlockingTimer.hpp"

class AppController {
public:
    AppController();
    void setup();
    void loop();

private:
    drivers::LedDriver greenLed_;
    drivers::LedDriver whiteLed_;
    drivers::BuzzerDriver buzzer_;

    managers::SensorManager sensorMgr_;
    managers::DisplayManager displayMgr_;
    managers::NetworkManager networkMgr_;
    managers::TimeManager timeMgr_;
    managers::WeatherManager weatherMgr_;
    managers::AlarmManager alarmMgr_;
    managers::WebServerManager webServerMgr_;

    utils::NonBlockingTimer displayTimer_;
    utils::NonBlockingTimer alarmTimer_;
};

#endif // APP_CONTROLLER_HPP
