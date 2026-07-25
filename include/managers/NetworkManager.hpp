#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <WiFi.h>
#include "utils/NonBlockingTimer.hpp"

namespace managers {

enum class NetworkState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};

class NetworkManager {
public:
    NetworkManager(const char* ssid, const char* password);
    void init();
    void update(); // Non-blocking state update called in loop
    
    [[nodiscard]] NetworkState getState() const;
    [[nodiscard]] bool isConnected() const;

private:
    const char* ssid_;
    const char* password_;
    NetworkState state_;
    utils::NonBlockingTimer retryTimer_;
};

} // namespace managers

#endif // NETWORK_MANAGER_HPP
