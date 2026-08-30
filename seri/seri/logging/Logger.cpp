#include "Seripch.h"

#include "seri/logging/Logger.h"

namespace seri
{
	std::mutex StreamLogger::_mutex{};

	StreamLogger::StreamLogger(LogLevel level, unsigned line, const char* file) : _level(level), _line(line), _file(file)
	{
	}

	StreamLogger::~StreamLogger()
	{
#if SERI_MULTITHREADED
		std::lock_guard<std::mutex> lock(_mutex);
#endif

		bool isError = _level == LogLevel::error;
		std::ostream& out = isError ? std::cerr : std::cout;

		out <<
			GetDateTime() << " " <<
			"[" << ToStringLevel(_level) << "] " <<
			_ss.str();

		if (isError)
		{
			out << " (" << _file << ":" << _line << ")";
		}

		out << std::endl;
	}

	std::string StreamLogger::GetDateTime()
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);

		std::tm tm{};

#ifdef SERI_WINDOWS
		localtime_s(&tm, &time);
#elif defined SERI_LINUX
		localtime_r(&time, &tm);
#endif

		std::stringstream ss;
		ss << std::put_time(&tm, "%d.%m.%Y %H:%M:%S");
		return ss.str();
	}

	std::string StreamLogger::ToStringLevel(LogLevel lvl)
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
