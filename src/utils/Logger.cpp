#include "utils/Logger.hpp"
#include <cstdio>
#include <cstdarg>

namespace utils {

LogLevel Logger::currentLevel_ = LogLevel::INFO;

void Logger::init(uint32_t baudRate) {
    Serial.begin(baudRate);
    uint32_t start = millis();
    while (!Serial && (millis() - start < 2000)) {
        // Wait up to 2 seconds for USB serial CDC
    }
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel_ = level;
}

void Logger::debug(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    printLog(LogLevel::DEBUG, tag, format, args);
    va_end(args);
}

void Logger::info(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    printLog(LogLevel::INFO, tag, format, args);
    va_end(args);
}

void Logger::warn(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    printLog(LogLevel::WARNING, tag, format, args);
    va_end(args);
}

void Logger::error(const char* tag, const char* format, ...) {
    va_list args;
    va_start(args, format);
    printLog(LogLevel::ERROR, tag, format, args);
    va_end(args);
}

void Logger::printLog(LogLevel level, const char* tag, const char* format, va_list args) {
    if (level < currentLevel_) return;

    const char* lvlStr = "INFO";
    switch (level) {
        case LogLevel::DEBUG:   lvlStr = "DBG"; break;
        case LogLevel::INFO:    lvlStr = "INF"; break;
        case LogLevel::WARNING: lvlStr = "WRN"; break;
        case LogLevel::ERROR:   lvlStr = "ERR"; break;
    }

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);

    Serial.printf("[%10lu] [%s] [%s]: %s\n", millis(), lvlStr, tag ? tag : "SYS", buffer);
}

} // namespace utils
