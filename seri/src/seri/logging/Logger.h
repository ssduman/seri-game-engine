#pragma once

#include "seri/core/Core.h"

#include <mutex>
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

	class StreamLogger
	{
	public:
		StreamLogger(LogLevel level, unsigned line, const char* file);
		~StreamLogger();

		template<typename T>
		StreamLogger& operator<<(const T& v)
		{
			_ss << v;
			return *this;
		}

		StreamLogger& operator<<(std::ostream& (*f)(std::ostream&))
		{
			_ss << f;
			return *this;
		}

	private:
		std::string GetDateTime();
		std::string ToStringLevel(LogLevel lvl);

		LogLevel _level;
		unsigned _line;
		const char* _file;
		std::ostringstream _ss;

		static std::mutex _mutex;

	};
}

#define LOGGER(lvl, msg) seri::StreamLogger(seri::LogLevel::lvl, __LINE__, __FILE__) << msg
#define LOGGER_S(lvl) seri::StreamLogger(seri::LogLevel::lvl, __LINE__, __FILE__)
