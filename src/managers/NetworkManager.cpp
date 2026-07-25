#include "managers/NetworkManager.hpp"
#include "config/Config.hpp"
#include "utils/Logger.hpp"

namespace managers {

NetworkManager::NetworkManager(const char* ssid, const char* password)
    : ssid_(ssid),
      password_(password),
      state_(NetworkState::DISCONNECTED),
      retryTimer_(config::WIFI_RECONNECT_INTERVAL_MS) {}

void NetworkManager::init() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    retryTimer_.start();
    state_ = NetworkState::DISCONNECTED;
    utils::Logger::info("WiFi", "WiFi Manager initialized");
}

void NetworkManager::update() {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        if (state_ != NetworkState::CONNECTED) {
            state_ = NetworkState::CONNECTED;
            utils::Logger::info("WiFi", "Connected! IP Address: %s", WiFi.localIP().toString().c_str());
        }
        return;
    }

    // Not connected
    if (state_ == NetworkState::CONNECTED) {
        state_ = NetworkState::DISCONNECTED;
        utils::Logger::warn("WiFi", "WiFi Connection Lost!");
        retryTimer_.reset();
    }

    if (state_ == NetworkState::DISCONNECTED) {
        if (retryTimer_.isExpired() || millis() < 2000) {
            utils::Logger::info("WiFi", "Attempting WiFi Connection to SSID: %s", ssid_);
            WiFi.begin(ssid_, password_);
            state_ = NetworkState::CONNECTING;
        }
    } else if (state_ == NetworkState::CONNECTING) {
        if (status == WL_CONNECT_FAILED || status == WL_NO_SSID_AVAIL || retryTimer_.isExpired()) {
            utils::Logger::warn("WiFi", "WiFi Connection Attempt Failed. Retrying in %u ms...", config::WIFI_RECONNECT_INTERVAL_MS);
            state_ = NetworkState::DISCONNECTED;
            retryTimer_.reset();
        }
    }
}

NetworkState NetworkManager::getState() const {
    return state_;
}

bool NetworkManager::isConnected() const {
    return state_ == NetworkState::CONNECTED;
}

} // namespace managers
