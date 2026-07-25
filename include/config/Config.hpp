#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdint>

namespace config {

// WiFi Configuration
constexpr const char* DEFAULT_WIFI_SSID = "POCO";
constexpr const char* DEFAULT_WIFI_PASS = "lalitkishore27";
constexpr const char* MDNS_HOSTNAME     = "envstation"; // Access via http://envstation.local
constexpr uint32_t    WIFI_RECONNECT_INTERVAL_MS = 15000; // 15 seconds

// Web Server Configuration
constexpr uint16_t    WEB_SERVER_PORT = 80;

// NTP Configuration
constexpr const char* NTP_SERVER_1 = "pool.ntp.org";
constexpr const char* NTP_SERVER_2 = "time.nist.gov";
constexpr int32_t     GMT_OFFSET_SEC = 19800; // IST (+5:30)
constexpr int32_t     DAYLIGHT_OFFSET_SEC = 0;

// Weather API Configuration (Open-Meteo)
// Coordinates: 13°01'28.4"N 80°11'14.3"E (Chennai, Tamil Nadu)
constexpr const char* WEATHER_LATITUDE  = "13.0246";
constexpr const char* WEATHER_LONGITUDE = "80.1873";
constexpr uint32_t    WEATHER_UPDATE_INTERVAL_MS = 600000; // 10 minutes (600s)

// Sensor Thresholds & Intervals
constexpr uint32_t    SENSOR_POLL_INTERVAL_MS = 2000; // 2 seconds
constexpr float       HIGH_TEMP_ALERT_C  = 35.0f;
constexpr float       LOW_TEMP_ALERT_C   = 10.0f;
constexpr float       HIGH_HUMIDITY_ALERT = 85.0f;
constexpr float       LOW_HUMIDITY_ALERT  = 20.0f;

// Display Options
constexpr uint32_t DISPLAY_REFRESH_INTERVAL_MS = 1000; // 1 second tick
constexpr uint8_t DEFAULT_BRIGHTNESS = 255;

// Buzzer PWM Settings
constexpr uint32_t BUZZER_LEDC_CHANNEL = 0;
constexpr uint32_t BUZZER_LEDC_RES_BITS = 8;
constexpr uint32_t ALERT_TONE_FREQ_HZ = 2000;

} // namespace config

#endif // CONFIG_HPP
