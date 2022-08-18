#pragma once

#include "Logger.h"

class LogManager {
public:
    LogManager(LoggerProperties loggerProperties) : _loggerProperties(loggerProperties) {}

    ~LogManager() {

    }

private:
    LoggerProperties _loggerProperties;

};
