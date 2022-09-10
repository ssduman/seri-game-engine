#pragma once

#include "Logger.h"

class LogManager {
public:
    LogManager(LoggerProperties loggerProperties) : _loggerProperties(loggerProperties) {}

    ~LogManager() = default;

private:
    LoggerProperties _loggerProperties;

};
