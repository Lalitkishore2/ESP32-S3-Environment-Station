#include "managers/WebServerManager.hpp"
#include "utils/Logger.hpp"
#include <ArduinoJson.h>

namespace managers {

static const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32-S3 Environment Station</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        :root {
            --bg: #090d16;
            --card-bg: rgba(19, 27, 44, 0.75);
            --card-border: rgba(255, 255, 255, 0.07);
            --primary: #38bdf8;
            --accent: #6366f1;
            --text: #f8fafc;
            --muted: #64748b;
            --success: #10b981;
            --warning: #f59e0b;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Inter', sans-serif; }
        body { background: var(--bg); color: var(--text); min-height: 100vh; padding: 24px; display: flex; flex-direction: column; align-items: center; }
        .dashboard { max-width: 1050px; width: 100%; }
        
        header { display: flex; justify-content: space-between; align-items: center; background: var(--card-bg); backdrop-filter: blur(16px); border: 1px solid var(--card-border); border-radius: 20px; padding: 20px 28px; margin-bottom: 24px; box-shadow: 0 10px 30px rgba(0,0,0,0.3); }
        .header-title h1 { font-size: 1.35rem; font-weight: 700; letter-spacing: -0.02em; color: #ffffff; }
        .header-title p { font-size: 0.85rem; color: var(--muted); margin-top: 4px; font-weight: 400; }
        
        .status-badge { display: flex; align-items: center; gap: 8px; background: rgba(16, 185, 129, 0.12); border: 1px solid rgba(16, 185, 129, 0.25); color: var(--success); padding: 6px 14px; border-radius: 99px; font-size: 0.8rem; font-weight: 600; letter-spacing: 0.05em; text-transform: uppercase; }
        .pulse-dot { width: 7px; height: 7px; background: var(--success); border-radius: 50%; box-shadow: 0 0 10px var(--success); }

        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; margin-bottom: 24px; }
        .card { background: var(--card-bg); backdrop-filter: blur(16px); border: 1px solid var(--card-border); border-radius: 20px; padding: 24px; transition: border-color 0.2s, transform 0.2s; }
        .card:hover { border-color: rgba(56, 189, 248, 0.25); transform: translateY(-2px); }
        
        .card-head { display: flex; align-items: center; gap: 10px; margin-bottom: 20px; border-bottom: 1px solid rgba(255,255,255,0.05); padding-bottom: 12px; }
        .card-head svg { width: 20px; height: 20px; stroke: var(--primary); stroke-width: 2; fill: none; }
        .card-head h2 { font-size: 0.95rem; font-weight: 600; text-transform: uppercase; letter-spacing: 0.06em; color: #94a3b8; }

        .stat-group { display: flex; flex-direction: column; gap: 16px; }
        .stat-item { display: flex; justify-content: space-between; align-items: center; }
        .stat-label { font-size: 0.9rem; color: #94a3b8; font-weight: 400; }
        .stat-value { font-size: 1.4rem; font-weight: 700; color: #ffffff; letter-spacing: -0.02em; }
        .stat-unit { font-size: 0.85rem; color: var(--muted); font-weight: 500; margin-left: 2px; }

        .progress-bar { width: 100%; height: 4px; background: rgba(255,255,255,0.06); border-radius: 2px; overflow: hidden; margin-top: 6px; }
        .progress-fill { height: 100%; background: linear-gradient(90deg, #38bdf8, #6366f1); border-radius: 2px; transition: width 0.4s ease; }

        .chart-section { display: flex; flex-direction: column; gap: 20px; }
        .chart-card { background: var(--card-bg); backdrop-filter: blur(16px); border: 1px solid var(--card-border); border-radius: 20px; padding: 24px; }
        .chart-container { position: relative; width: 100%; height: 260px; margin-top: 16px; }

        footer { text-align: center; margin-top: 32px; color: var(--muted); font-size: 0.8rem; font-weight: 400; }
    </style>
</head>
<body>
    <div class="dashboard">
        <header>
            <div class="header-title">
                <h1>ESP32-S3 Environment Station</h1>
                <p id="time-display">Connecting to NTP Server...</p>
            </div>
            <div class="status-badge">
                <div class="pulse-dot"></div>
                <span id="sys-status">Connected</span>
            </div>
        </header>

        <div class="grid">
            <!-- Indoor Card -->
            <div class="card">
                <div class="card-head">
                    <svg viewBox="0 0 24 24"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/></svg>
                    <h2>Indoor Climate</h2>
                </div>
                <div class="stat-group">
                    <div>
                        <div class="stat-item">
                            <span class="stat-label">DS18B20 Temp (Probe)</span>
                            <span class="stat-value" id="ds-temp">--<span class="stat-unit">°C</span></span>
                        </div>
                    </div>
                    <div>
                        <div class="stat-item">
                            <span class="stat-label">DHT11 Temp (Ambient)</span>
                            <span class="stat-value" id="dht-temp">--<span class="stat-unit">°C</span></span>
                        </div>
                    </div>
                    <div>
                        <div class="stat-item">
                            <span class="stat-label">DHT11 Humidity</span>
                            <span class="stat-value" id="dht-hum">--<span class="stat-unit">%</span></span>
                        </div>
                        <div class="progress-bar">
                            <div class="progress-fill" id="hum-bar" style="width: 0%;"></div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Outdoor Card -->
            <div class="card">
                <div class="card-head">
                    <svg viewBox="0 0 24 24"><path d="M12 2v2M12 20v2M4.93 4.93l1.41 1.41M17.66 17.66l1.41 1.41M2 12h2M20 12h2M4.93 19.07l1.41-1.41M17.66 6.34l1.41-1.41"/></svg>
                    <h2>Outdoor Weather (Chennai)</h2>
                </div>
                <div class="stat-group">
                    <div class="stat-item">
                        <span class="stat-label">Outdoor Temperature</span>
                        <span class="stat-value" id="out-temp">--<span class="stat-unit">°C</span></span>
                    </div>
                    <div class="stat-item">
                        <span class="stat-label">Apparent (Feels Like)</span>
                        <span class="stat-value" id="feels-like">--<span class="stat-unit">°C</span></span>
                    </div>
                    <div class="stat-item">
                        <span class="stat-label">Surface Pressure</span>
                        <span class="stat-value" id="pressure">--<span class="stat-unit">hPa</span></span>
                    </div>
                    <div class="stat-item">
                        <span class="stat-label">Wind Velocity</span>
                        <span class="stat-value" id="wind">--<span class="stat-unit">km/h</span></span>
                    </div>
                </div>
            </div>

            <!-- System Info Card -->
            <div class="card">
                <div class="card-head">
                    <svg viewBox="0 0 24 24"><rect x="2" y="2" width="20" height="8" rx="2"/><rect x="2" y="14" width="20" height="8" rx="2"/><line x1="6" y1="6" x2="6.01" y2="6"/><line x1="6" y1="18" x2="6.01" y2="18"/></svg>
                    <h2>System & Database</h2>
                </div>
                <div class="stat-group">
                    <div class="stat-item">
                        <span class="stat-label">24H Database Log</span>
                        <span class="stat-value" style="font-size: 1.1rem; color: var(--primary);" id="db-records">Active</span>
                    </div>
                    <div class="stat-item">
                        <span class="stat-label">WiFi Connection</span>
                        <span class="stat-value" style="font-size: 1.1rem; color: var(--success);" id="wifi-status">Active</span>
                    </div>
                    <div class="stat-item">
                        <span class="stat-label">NTP Clock Sync</span>
                        <span class="stat-value" style="font-size: 1.1rem;" id="ntp-status">Synchronized</span>
                    </div>
                </div>
            </div>
        </div>

        <!-- 24-Hour Analytics Charts -->
        <div class="chart-section">
            <div class="chart-card">
                <div class="card-head">
                    <svg viewBox="0 0 24 24"><polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/></svg>
                    <h2>24-Hour Temperature Trends (°C)</h2>
                </div>
                <div class="chart-container">
                    <canvas id="tempChart"></canvas>
                </div>
            </div>

            <div class="chart-card">
                <div class="card-head">
                    <svg viewBox="0 0 24 24"><path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/></svg>
                    <h2>24-Hour Relative Humidity Trend (%)</h2>
                </div>
                <div class="chart-container">
                    <canvas id="humChart"></canvas>
                </div>
            </div>
        </div>

        <footer>
            ESP32-S3 Environment Station &bull; Modern C++17 Architecture &bull; 24H Circular Database Analytics
        </footer>
    </div>

    <script>
        let tempChartInst = null;
        let humChartInst = null;

        function initCharts() {
            const chartOptions = {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: { labels: { color: '#94a3b8', font: { family: 'Inter', size: 12 } } }
                },
                scales: {
                    x: { ticks: { color: '#64748b' }, grid: { color: 'rgba(255,255,255,0.05)' } },
                    y: { ticks: { color: '#64748b' }, grid: { color: 'rgba(255,255,255,0.05)' } }
                }
            };

            const ctxTemp = document.getElementById('tempChart').getContext('2d');
            tempChartInst = new Chart(ctxTemp, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [
                        { label: 'DS18B20 Temp (°C)', data: [], borderColor: '#38bdf8', backgroundColor: 'rgba(56,189,248,0.1)', tension: 0.3, fill: true },
                        { label: 'DHT11 Temp (°C)', data: [], borderColor: '#10b981', backgroundColor: 'transparent', tension: 0.3 },
                        { label: 'Outdoor Temp (°C)', data: [], borderColor: '#f59e0b', backgroundColor: 'transparent', tension: 0.3 }
                    ]
                },
                options: chartOptions
            });

            const ctxHum = document.getElementById('humChart').getContext('2d');
            humChartInst = new Chart(ctxHum, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [
                        { label: 'DHT11 Humidity (%)', data: [], borderColor: '#6366f1', backgroundColor: 'rgba(99,102,241,0.15)', tension: 0.3, fill: true }
                    ]
                },
                options: chartOptions
            });
        }

        async function fetchMetrics() {
            try {
                const res = await fetch('/api/data');
                const data = await res.json();

                document.getElementById('time-display').innerText = `${data.time} | ${data.date}`;
                document.getElementById('ds-temp').innerHTML = `${data.ds_valid ? data.ds_temp.toFixed(1) : '--'}<span class="stat-unit">°C</span>`;
                document.getElementById('dht-temp').innerHTML = `${data.dht_valid ? data.dht_temp.toFixed(1) : '--'}<span class="stat-unit">°C</span>`;
                document.getElementById('dht-hum').innerHTML = `${data.dht_valid ? data.dht_hum.toFixed(0) : '--'}<span class="stat-unit">%</span>`;

                if(data.dht_valid) {
                    document.getElementById('hum-bar').style.width = `${Math.min(100, Math.max(0, data.dht_hum))}%`;
                }

                if(data.weather_valid) {
                    document.getElementById('out-temp').innerHTML = `${data.outdoor_temp.toFixed(1)}<span class="stat-unit">°C</span>`;
                    document.getElementById('feels-like').innerHTML = `${data.feels_like.toFixed(1)}<span class="stat-unit">°C</span>`;
                    document.getElementById('pressure').innerHTML = `${data.pressure.toFixed(0)}<span class="stat-unit">hPa</span>`;
                    document.getElementById('wind').innerHTML = `${data.wind_speed.toFixed(1)}<span class="stat-unit">km/h</span>`;
                }

                document.getElementById('wifi-status').innerText = data.wifi ? 'Active' : 'Offline';
            } catch(e) {
                document.getElementById('sys-status').innerText = 'Offline';
            }
        }

        async function fetchHistory() {
            try {
                const res = await fetch('/api/history');
                const records = await res.json();
                if(!Array.isArray(records)) return;

                document.getElementById('db-records').innerText = `${records.length} Points`;

                const labels = records.map(r => r.t);
                const dsData = records.map(r => r.ds);
                const dhtData = records.map(r => r.dht);
                const humData = records.map(r => r.hum);
                const outData = records.map(r => r.out);

                if(tempChartInst) {
                    tempChartInst.data.labels = labels;
                    tempChartInst.data.datasets[0].data = dsData;
                    tempChartInst.data.datasets[1].data = dhtData;
                    tempChartInst.data.datasets[2].data = outData;
                    tempChartInst.update();
                }

                if(humChartInst) {
                    humChartInst.data.labels = labels;
                    humChartInst.data.datasets[0].data = humData;
                    humChartInst.update();
                }
            } catch(e) {}
        }

        initCharts();
        fetchMetrics();
        fetchHistory();
        setInterval(fetchMetrics, 3000);
        setInterval(fetchHistory, 10000);
    </script>
</body>
</html>
)rawliteral";

WebServerManager::WebServerManager(uint16_t port)
    : server_(port), started_(false), wifiConnected_(false), historyCount_(0), historyHead_(0), historyTimer_(120000) { // Log history point every 2 mins
    snprintf(timeStr_, sizeof(timeStr_), "00:00:00");
    snprintf(dateStr_, sizeof(dateStr_), "Sat, 25 Jul 2026");
    snprintf(ipStr_, sizeof(ipStr_), "0.0.0.0");
}

void WebServerManager::init() {
    server_.on("/", [this]() { handleRoot(); });
    server_.on("/api/data", [this]() { handleApiData(); });
    server_.on("/api/history", [this]() { handleApiHistory(); });
    server_.onNotFound([this]() { handleNotFound(); });
    
    server_.begin();
    started_ = true;
    historyTimer_.start();
    utils::Logger::info("WebSvr", "HTTP Web Server started on port 80");
}

void WebServerManager::update() {
    if (started_) {
        server_.handleClient();

        // Periodically record history point into database
        if (historyTimer_.isExpired() || historyCount_ == 0) {
            recordHistoryPoint();
        }
    }
}

void WebServerManager::recordHistoryPoint() {
    if (!wifiConnected_ || strlen(timeStr_) == 0 || strncmp(timeStr_, "00:00:00", 8) == 0) {
        return;
    }

    HistoryRecord& rec = historyDb_[historyHead_];
    // Copy HH:MM timestamp substring
    strncpy(rec.time, timeStr_, 5);
    rec.time[5] = '\0';

    rec.dsTemp = telemetry_.ds18b20Valid ? telemetry_.ds18b20TempC : 0.0f;
    rec.dhtTemp = telemetry_.dhtValid ? telemetry_.dhtTempC : 0.0f;
    rec.dhtHum = telemetry_.dhtValid ? telemetry_.dhtHumidity : 0.0f;
    rec.outdoorTemp = weather_.isValid ? weather_.outdoorTempC : 0.0f;

    historyHead_ = (historyHead_ + 1) % MAX_HISTORY;
    if (historyCount_ < MAX_HISTORY) {
        historyCount_++;
    }

    utils::Logger::debug("Database", "Recorded history point #%d at %s", historyCount_, rec.time);
}

void WebServerManager::updateData(const char* timeStr, const char* dateStr, bool wifiConnected, const char* ipStr,
                                   const models::TelemetryData& telemetry,
                                   const models::WeatherData& weather) {
    if (timeStr) strncpy(timeStr_, timeStr, sizeof(timeStr_));
    if (dateStr) strncpy(dateStr_, dateStr, sizeof(dateStr_));
    if (ipStr) strncpy(ipStr_, ipStr, sizeof(ipStr_));
    wifiConnected_ = wifiConnected;
    telemetry_ = telemetry;
    weather_ = weather;
}

void WebServerManager::handleRoot() {
    server_.send(200, "text/html", DASHBOARD_HTML);
}

void WebServerManager::handleApiData() {
    JsonDocument doc;

    doc["time"]          = timeStr_;
    doc["date"]          = dateStr_;
    doc["wifi"]          = wifiConnected_;
    doc["ip"]            = ipStr_;

    doc["ds_temp"]       = telemetry_.ds18b20TempC;
    doc["dht_temp"]      = telemetry_.dhtTempC;
    doc["dht_hum"]       = telemetry_.dhtHumidity;
    doc["ds_valid"]      = telemetry_.ds18b20Valid;
    doc["dht_valid"]     = telemetry_.dhtValid;

    doc["outdoor_temp"]  = weather_.outdoorTempC;
    doc["feels_like"]    = weather_.feelsLikeC;
    doc["pressure"]      = weather_.pressureHpa;
    doc["wind_speed"]    = weather_.windSpeedKmh;
    doc["weather_valid"] = weather_.isValid;

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    server_.send(200, "application/json", jsonResponse);
}

void WebServerManager::handleApiHistory() {
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    size_t start = (historyCount_ < MAX_HISTORY) ? 0 : historyHead_;
    for (size_t i = 0; i < historyCount_; i++) {
        size_t idx = (start + i) % MAX_HISTORY;
        const HistoryRecord& rec = historyDb_[idx];

        JsonObject obj = arr.add<JsonObject>();
        obj["t"]   = rec.time;
        obj["ds"]  = serialized(String(rec.dsTemp, 1));
        obj["dht"] = serialized(String(rec.dhtTemp, 1));
        obj["hum"] = serialized(String(rec.dhtHum, 1));
        obj["out"] = serialized(String(rec.outdoorTemp, 1));
    }

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    server_.send(200, "application/json", jsonResponse);
}

void WebServerManager::handleNotFound() {
    server_.send(404, "text/plain", "404: Not Found");
}

} // namespace managers
