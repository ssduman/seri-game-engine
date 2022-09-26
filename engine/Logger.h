#pragma once

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

enum class LogLevel {
    none,
    fatal,
    error,
    warning,
    info,
    debug,
    verbose
};

struct LoggerProperties {
    LogLevel level = LogLevel::error;
};

#define LOGGER_IMPL(lvl, msg) std::cout \
    << getDateTime() \
    << " [" << toString(lvl) << "] " \
    << msg \
    << std::endl

#define LOGGER(lvl, msg) LOGGER_IMPL(LogLevel::lvl, msg)

inline std::string getDateTime() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%d.%m.%Y %H:%M:%S");
    return ss.str();
}

inline std::string toString(LogLevel lvl) {
    switch (lvl) {
        case LogLevel::none:
            return "none";
        case LogLevel::fatal:
            return "fatal";
        case LogLevel::error:
            return "error";
        case LogLevel::warning:
            return "warning";
        case LogLevel::info:
            return "info";
        case LogLevel::debug:
            return "debug";
        case LogLevel::verbose:
            return "verbose";
        default:
            return "unknown";
    }
}

//inline LoggerProperties loggerProperties;
