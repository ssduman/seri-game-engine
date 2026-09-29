#pragma once

#include <spdlog/common.h>

#include <memory>
#include <sstream>
#include <string>
#include <string_view>

namespace seri
{
	enum class LogLevel
	{
		none = 0,
		error = 1,
		warning = 2,
		info = 3,
		verbose = 4,
	};

	enum class LogClock
	{
		utc,
		local,
	};

	struct LoggerConfig
	{
		LogLevel level{ LogLevel::info };

		LogClock clock{ LogClock::local };
		std::string timeStampFormat{ "%H:%M:%S.%f %d.%m.%Y" };

		bool autoFlush{ true };

		bool showThreadId{ true };

		bool bufferLogs{ true };
		size_t bufferCapacity{ 4096 };
	};

	class SeriLogger;

	class Logger
	{
	public:
		static void Init(const LoggerConfig& config);
		static void Shutdown();

		static void SetLogLevel(LogLevel level);

		static bool ShouldLog(LogLevel level);
		static void Log(LogLevel level, const char* module, const char* file, int line, const char* function, std::string_view message);

		static LogLevel FromString(const char*);
		static const char* ToString(LogLevel level);
		static const char* ToString3(LogLevel level);

		static spdlog::level::level_enum ToSpdlogLevel(LogLevel level);
		static LogLevel FromSpdlogLevel(spdlog::level::level_enum level);

		static constexpr const char* TrimPath(const char* path)
		{
			const char* name = path;
			for (const char* it = path; *it != '\0'; it++)
			{
				if (*it == '/' || *it == '\\')
				{
					name = it + 1;
				}
			}
			return name;
		}

	private:
		Logger() = default;
		~Logger() = default;

		Logger(Logger&& other) = delete;
		Logger(const Logger& other) = delete;
		Logger& operator=(Logger&& other) = default;
		Logger& operator=(const Logger& other) = delete;

		static Logger& GetInstance()
		{
			static Logger instance;
			return instance;
		}

		LoggerConfig _config{};
		std::shared_ptr<SeriLogger> _logger;
		bool _inited{ false };
	};

	class LogStream
	{
	public:
		LogStream(LogLevel level, const char* module, const char* file, int line, const char* function);
		~LogStream();

		std::ostream& Stream();

	private:
		LogStream(LogStream&& other) = delete;
		LogStream(const LogStream& other) = delete;
		LogStream& operator=(LogStream&& other) = delete;
		LogStream& operator=(const LogStream& other) = delete;

		LogLevel _level{ LogLevel::none };
		const char* _module{ "" };
		const char* _file{ "" };
		int _line{ 0 };
		const char* _function{ "" };
		std::ostringstream _stream;
	};
}

#define LOGGER(level) \
	if (!::seri::Logger::ShouldLog(::seri::LogLevel::level)) {} else \
		::seri::LogStream(::seri::LogLevel::level, "", ::seri::Logger::TrimPath(__FILE__), __LINE__, __FUNCTION__).Stream()

#define LIB_LOGGER(level, module) \
	if (!::seri::Logger::ShouldLog(::seri::LogLevel::level)) {} else \
		::seri::LogStream(::seri::LogLevel::level, #module, ::seri::Logger::TrimPath(__FILE__), __LINE__, __FUNCTION__).Stream()
