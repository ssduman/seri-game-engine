#pragma once

#include "seri/logging/Logger.h"

class LogManager
{
public:
	LogManager(LogLevel currLogLevel) : _currLogLevel{ currLogLevel } {}

	LogManager(LogLevel currLogLevel, std::string currMsg) : _currLogLevel{ currLogLevel }, _currMsg{ currMsg } {}

	~LogManager()
	{
		if (!_currMsg.empty() && _currLogLevel >= _loggerProperties.level)
		{
			std::cout << _currMsg;
		}
	}

	void setMinimumLogLevel(LogLevel lvl)
	{
		_loggerProperties.level = lvl;
	}

	friend std::ostream& operator<< (std::ostream& out, const LogManager& mng);

	friend std::ostream& operator<< (std::ostream& out, const char* msg);

	friend std::ostream& operator<< (std::ostream& out, const std::string& msg);

	LogLevel _currLogLevel;
	std::string _currMsg;
	static inline LoggerProperties _loggerProperties{ LogLevel::info };

};

inline std::ostream& operator<<(std::ostream& out, const LogManager& mng)
{
	out << mng._currMsg;
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const char* msg)
{
	out << msg;
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const std::string& msg)
{
	out << msg;
	return out;
}

#define SERI_LOGGER(lvl) std::cout << LogManager{ lvl }

void test()
{
	std::cout << LogManager{ LogLevel::info, "test" };
	SERI_LOGGER(LogLevel::info) << "seri logger" << " test 1";
	SERI_LOGGER(LogLevel::info) << LogManager{ LogLevel::info };
	std::cout << LogManager{ LogLevel::info } << LogManager{ LogLevel::info } << "test";
}

