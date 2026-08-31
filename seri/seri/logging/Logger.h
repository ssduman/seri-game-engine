#pragma once

#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#include <string>

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
	};

	using SeriLogger = boost::log::sources::severity_logger_mt<LogLevel>;

	BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(GlobalLogger, SeriLogger);

	class Logger
	{
	public:
		static void Init(const LoggerConfig& config);
		static void Shutdown();

		static void SetLogLevel(LogLevel level);

		static LogLevel FromString(const char*);
		static const char* ToString(LogLevel level);
		static const char* ToString3(LogLevel level);

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

		static void FormatRecord(const boost::log::record_view& record, boost::log::formatting_ostream& stream);

		static void EnableColor();
		static const char* ToColor(LogLevel level);

		static Logger& GetInstance()
		{
			static Logger instance;
			return instance;
		}

		LoggerConfig _config{};
		std::locale _timeStampLocale{};
		bool _inited{ false };
		bool _colorEnabled{ false };
	};
}

#define LOGGER(lvl) \
	BOOST_LOG_STREAM_SEV(::seri::GlobalLogger::get(), ::seri::LogLevel::lvl) \
		<< ::boost::log::add_value("File", ::seri::Logger::TrimPath(__FILE__)) \
		<< ::boost::log::add_value("Line", static_cast<unsigned int>(__LINE__)) \
		<< ::boost::log::add_value("Function", static_cast<const char*>(__FUNCTION__))
