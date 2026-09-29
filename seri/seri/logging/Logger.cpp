#include "Seripch.h"

#include "seri/logging/LogBuffer.h"
#include "seri/logging/Logger.h"

#include <spdlog/details/fmt_helper.h>
#include <spdlog/logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/version.h>

namespace seri
{
	class SeriLogger : public spdlog::logger
	{
	public:
		using spdlog::logger::logger;

		void Log(const char* module, spdlog::source_loc source, spdlog::level::level_enum level, std::string_view message)
		{
			spdlog::details::log_msg msg(source, module, level, message);
			log_it_(msg, should_log(level), tracer_.enabled());
		}
	};

	class LogLevelFlag : public spdlog::custom_flag_formatter
	{
	public:
		void format(const spdlog::details::log_msg& msg, const std::tm& time, spdlog::memory_buf_t& dest) override
		{
			spdlog::details::fmt_helper::append_string_view(Logger::ToString3(Logger::FromSpdlogLevel(msg.level)), dest);
		}

		std::unique_ptr<spdlog::custom_flag_formatter> clone() const override
		{
			return std::make_unique<LogLevelFlag>();
		}
	};

	class LogModuleFlag : public spdlog::custom_flag_formatter
	{
	public:
		void format(const spdlog::details::log_msg& msg, const std::tm& time, spdlog::memory_buf_t& dest) override
		{
			if (msg.logger_name.size() == 0)
			{
				return;
			}

			dest.push_back('[');
			spdlog::details::fmt_helper::append_string_view(msg.logger_name, dest);
			spdlog::details::fmt_helper::append_string_view("] ", dest);
		}

		std::unique_ptr<spdlog::custom_flag_formatter> clone() const override
		{
			return std::make_unique<LogModuleFlag>();
		}
	};

	class LogSourceFlag : public spdlog::custom_flag_formatter
	{
	public:
		void format(const spdlog::details::log_msg& msg, const std::tm& time, spdlog::memory_buf_t& dest) override
		{
			if (msg.level != spdlog::level::err || msg.source.empty())
			{
				return;
			}

			dest.push_back('(');
			spdlog::details::fmt_helper::append_string_view(msg.source.filename, dest);
			dest.push_back(':');
			spdlog::details::fmt_helper::append_int(msg.source.line, dest);
			spdlog::details::fmt_helper::append_string_view(") ", dest);
		}

		std::unique_ptr<spdlog::custom_flag_formatter> clone() const override
		{
			return std::make_unique<LogSourceFlag>();
		}
	};

	void Logger::Init(const LoggerConfig& config)
	{
		GetInstance()._config = config;

		bool isUTC = GetInstance()._config.clock == LogClock::utc;
		spdlog::pattern_time_type timeType = isUTC ? spdlog::pattern_time_type::utc : spdlog::pattern_time_type::local;

		std::string pattern = GetInstance()._config.timeStampFormat + " ";
		if (GetInstance()._config.showThreadId)
		{
			pattern += "[%t] ";
		}
		pattern += "%^[%q]%$ %k%w%v";

		auto formatter = std::make_unique<spdlog::pattern_formatter>(timeType);
		formatter->add_flag<LogLevelFlag>('q').add_flag<LogModuleFlag>('k').add_flag<LogSourceFlag>('w').set_pattern(pattern);

		auto consoleSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
		consoleSink->set_formatter(std::move(formatter));

		std::vector<spdlog::sink_ptr> sinks{ consoleSink };

		if (GetInstance()._config.bufferLogs)
		{
			LogBuffer::SetCapacity(GetInstance()._config.bufferCapacity);
			sinks.push_back(std::make_shared<LogBufferSink>(timeType));
		}

		GetInstance()._logger = std::make_shared<SeriLogger>("seri", sinks.begin(), sinks.end());
		GetInstance()._logger->flush_on(GetInstance()._config.autoFlush ? spdlog::level::trace : spdlog::level::off);

		SetLogLevel(GetInstance()._config.level);

		GetInstance()._inited = true;

		LIB_LOGGER(info, logger) << "inited, spdlog version: " << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR << "." << SPDLOG_VER_PATCH
			<< ", level: " << ToString(GetInstance()._config.level)
			<< ", clock: " << (isUTC ? "utc" : "local");
	}

	void Logger::Shutdown()
	{
		if (!GetInstance()._inited)
		{
			return;
		}

		LIB_LOGGER(info, logger) << "shutting down";

		GetInstance()._logger->flush();
		GetInstance()._logger->set_level(spdlog::level::off);

		GetInstance()._inited = false;
	}

	void Logger::SetLogLevel(LogLevel level)
	{
		GetInstance()._config.level = level;

		if (GetInstance()._logger)
		{
			GetInstance()._logger->set_level(ToSpdlogLevel(level));
		}
	}

	bool Logger::ShouldLog(LogLevel level)
	{
		return GetInstance()._logger && GetInstance()._logger->should_log(ToSpdlogLevel(level));
	}

	void Logger::Log(LogLevel level, const char* module, const char* file, int line, const char* function, std::string_view message)
	{
		if (GetInstance()._logger)
		{
			GetInstance()._logger->Log(module, spdlog::source_loc{ file, line, function }, ToSpdlogLevel(level), message);
		}
	}

	LogLevel Logger::FromString(const char* str)
	{
		if (str)
		{
			if (strcmp(str, "error") == 0)
			{
				return LogLevel::error;
			}
			if (strcmp(str, "warning") == 0)
			{
				return LogLevel::warning;
			}
			if (strcmp(str, "info") == 0)
			{
				return LogLevel::info;
			}
			if (strcmp(str, "verbose") == 0)
			{
				return LogLevel::verbose;
			}
		}
		return LogLevel::none;
	}

	const char* Logger::ToString(LogLevel level)
	{
		switch (level)
		{
			case LogLevel::none:
				return "none";
			case LogLevel::error:
				return "error";
			case LogLevel::warning:
				return "warning";
			case LogLevel::info:
				return "info";
			case LogLevel::verbose:
				return "verbose";
			default:
				return "unknown";
		}
	}

	const char* Logger::ToString3(LogLevel level)
	{
		switch (level)
		{
			case LogLevel::none:
				return "NONE";
			case LogLevel::error:
				return "ERR";
			case LogLevel::warning:
				return "WRN";
			case LogLevel::info:
				return "INF";
			case LogLevel::verbose:
				return "VER";
			default:
				return "UNK";
		}
	}

	spdlog::level::level_enum Logger::ToSpdlogLevel(LogLevel level)
	{
		switch (level)
		{
			case LogLevel::error:
				return spdlog::level::err;
			case LogLevel::warning:
				return spdlog::level::warn;
			case LogLevel::info:
				return spdlog::level::info;
			case LogLevel::verbose:
				return spdlog::level::debug;
			default:
				return spdlog::level::off;
		}
	}

	LogLevel Logger::FromSpdlogLevel(spdlog::level::level_enum level)
	{
		switch (level)
		{
			case spdlog::level::err:
			case spdlog::level::critical:
				return LogLevel::error;
			case spdlog::level::warn:
				return LogLevel::warning;
			case spdlog::level::info:
				return LogLevel::info;
			case spdlog::level::debug:
			case spdlog::level::trace:
				return LogLevel::verbose;
			default:
				return LogLevel::none;
		}
	}

	LogStream::LogStream(LogLevel level, const char* module, const char* file, int line, const char* function)
		: _level(level), _module(module), _file(file), _line(line), _function(function)
	{
	}

	LogStream::~LogStream()
	{
		Logger::Log(_level, _module, _file, _line, _function, _stream.view());
	}

	std::ostream& LogStream::Stream()
	{
		return _stream;
	}
}
