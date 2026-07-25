#ifndef WEB_SERVER_MANAGER_HPP
#define WEB_SERVER_MANAGER_HPP

#include <WebServer.h>
#include "models/TelemetryData.hpp"
#include "models/WeatherData.hpp"
#include "utils/NonBlockingTimer.hpp"

namespace managers {

struct HistoryRecord {
    char time[12];
    float dsTemp;
    float dhtTemp;
    float dhtHum;
    float outdoorTemp;
};

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
    void handleApiHistory();
    void handleNotFound();
    void recordHistoryPoint();

    WebServer server_;
    bool started_;

    // Cached state for HTTP JSON response
    char timeStr_[16];
    char dateStr_[32];
    char ipStr_[32];
    bool wifiConnected_;
    models::TelemetryData telemetry_;
    models::WeatherData weather_;

    // 24-Hour Circular Database Storage (144 data points = 24h sampled every 10 mins)
    static constexpr size_t MAX_HISTORY = 144;
    HistoryRecord historyDb_[MAX_HISTORY];
    size_t historyCount_;
    size_t historyHead_;
    utils::NonBlockingTimer historyTimer_;
};

} // namespace managers

#endif // WEB_SERVER_MANAGER_HPP
