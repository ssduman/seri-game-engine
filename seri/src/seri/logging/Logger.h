#pragma once

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace seri
{
	enum class LogLevel
	{
		none,
		error,
		warning,
		info,
		debug,
		verbose,
	};

	struct LoggerProperties
	{
		LogLevel level = LogLevel::info;
	};

	inline std::string GetDateTime()
	{
		std::time_t t = std::time(nullptr);
		std::tm tm{};
		localtime_s(&tm, &t);
		std::stringstream ss;
		ss << std::put_time(&tm, "%d.%m.%Y %H:%M:%S");
		return ss.str();
	}

	inline std::string ToString(LogLevel lvl)
	{
		switch (lvl)
		{
		case LogLevel::none:
			return "none";
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
}

#define SERI_LOGGER_IMPL(lvl, msg) std::cout \
    << seri::GetDateTime() \
    << " [" << seri::ToString(lvl) << "] " \
    << msg \
    << std::endl

#define LOGGER(lvl, msg) SERI_LOGGER_IMPL(seri::LogLevel::lvl, msg)
#define SERI_LOGGER(lvl, msg) SERI_LOGGER_IMPL(seri::LogLevel::lvl, msg)
