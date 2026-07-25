#ifndef WEB_SERVER_MANAGER_HPP
#define WEB_SERVER_MANAGER_HPP

#include <WebServer.h>
#include "models/TelemetryData.hpp"
#include "models/WeatherData.hpp"

namespace managers {

class WebServerManager {
public:
    explicit WebServerManager(uint16_t port = 80);
    void init();
    void update(); // Called non-blockingly in main loop
    void updateData(const char* timeStr, const char* dateStr, bool wifiConnected, const char* ipStr,
                    const models::TelemetryData& telemetry,
                    const models::WeatherData& weather);

private:
    void handleRoot();
    void handleApiData();
    void handleNotFound();

    WebServer server_;
    bool started_;

    // Cached state for HTTP JSON response
    char timeStr_[16];
    char dateStr_[32];
    char ipStr_[32];
    bool wifiConnected_;
    models::TelemetryData telemetry_;
    models::WeatherData weather_;
};

} // namespace managers

#endif // WEB_SERVER_MANAGER_HPP
