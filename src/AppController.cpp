#include "AppController.hpp"
#include "config/Config.hpp"
#include "utils/Logger.hpp"
#include <WiFi.h>

AppController::AppController()
    : greenLed_(config::pins::GREEN_LED_PIN),
      whiteLed_(config::pins::WHITE_LED_PIN),
      buzzer_(config::pins::BUZZER_PIN),
      sensorMgr_(config::pins::DS18B20_PIN, config::pins::DHT11_PIN),
      displayMgr_(config::pins::TFT_CS_PIN, config::pins::TFT_DC_PIN, config::pins::TFT_RST_PIN,
                  config::pins::TFT_MOSI_PIN, config::pins::TFT_SCK_PIN, config::pins::TFT_BL_PIN),
      networkMgr_(config::DEFAULT_WIFI_SSID, config::DEFAULT_WIFI_PASS),
      timeMgr_(),
      weatherMgr_(),
      alarmMgr_(greenLed_, whiteLed_, buzzer_),
      webServerMgr_(config::WEB_SERVER_PORT),
      displayTimer_(config::DISPLAY_REFRESH_INTERVAL_MS),
      alarmTimer_(1000) {}

void AppController::setup() {
    utils::Logger::init(115200);
    utils::Logger::info("AppCtrl", "--- ESP32-S3 ENVIRONMENT MONITORING STATION ---");

    // Initialize display and splash screen
    displayMgr_.init();
    displayMgr_.showSplashScreen();

    // Initialize hardware drivers & alarm manager
    alarmMgr_.init();

    // Initialize sensors
    sensorMgr_.init();

    // Initialize Network, Time, Weather, Web Server
    networkMgr_.init();
    timeMgr_.init();
    weatherMgr_.init();
    webServerMgr_.init();

    displayTimer_.start();
    alarmTimer_.start();

    utils::Logger::info("AppCtrl", "Setup phase complete. Entering main execution loop.");
}

void AppController::loop() {
    // 1. Update non-blocking drivers
    greenLed_.update();
    whiteLed_.update();
    buzzer_.update();

    // 2. Poll sensors
    sensorMgr_.update();

    // 3. Update WiFi network state machine
    networkMgr_.update();

    // 4. Sync time if WiFi is connected and time is not synced
    bool wifiConnected = networkMgr_.isConnected();
    if (wifiConnected && !timeMgr_.isSynced()) {
        timeMgr_.syncNTP();
    }

    // 5. Update weather data
    weatherMgr_.update(wifiConnected);

    // 6. Update Web Server HTTP requests
    webServerMgr_.update();

    // 7. Evaluate alarm thresholds periodically
    if (alarmTimer_.isExpired()) {
        const auto& telemetry = sensorMgr_.getTelemetry();
        const auto& weather = weatherMgr_.getWeatherData();
        alarmMgr_.evaluateSystemState(wifiConnected, weather.isValid, telemetry);
    }

    // 8. Refresh UI dashboard & Web Server data
    if (displayTimer_.isExpired()) {
        char timeStr[16];
        char dateStr[32];
        timeMgr_.getFormattedTime(timeStr, sizeof(timeStr));
        timeMgr_.getFormattedDate(dateStr, sizeof(dateStr));

        const auto& telemetry = sensorMgr_.getTelemetry();
        const auto& weather = weatherMgr_.getWeatherData();
        String localIp = wifiConnected ? WiFi.localIP().toString() : "0.0.0.0";

        // Update web server data cache
        webServerMgr_.updateData(timeStr, dateStr, wifiConnected, localIp.c_str(), telemetry, weather);

        const char* statusMsg = "System Healthy";
        if (!telemetry.ds18b20Valid || !telemetry.dhtValid) {
            statusMsg = "WARN: Sensor Err";
        } else if (!wifiConnected) {
            statusMsg = "WARN: Offline";
        }

        // Render TFT screen
        displayMgr_.renderDashboard(timeStr, dateStr, wifiConnected, localIp.c_str(), telemetry, weather, statusMsg);
    }
}
