#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <Arduino.h>

namespace utils {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void init(uint32_t baudRate = 115200);
    static void setLogLevel(LogLevel level);
    
    static void debug(const char* tag, const char* format, ...);
    static void info(const char* tag, const char* format, ...);
    static void warn(const char* tag, const char* format, ...);
    static void error(const char* tag, const char* format, ...);

private:
    static void printLog(LogLevel level, const char* tag, const char* format, va_list args);
    static LogLevel currentLevel_;
};

} // namespace utils

#endif // LOGGER_HPP
