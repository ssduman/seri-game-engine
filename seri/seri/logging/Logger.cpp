#include "Seripch.h"

#include "seri/core/Core.h"
#include "seri/logging/Logger.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes/attribute_set.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/counter.hpp>
#include <boost/log/attributes/current_process_id.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#ifdef SERI_WINDOWS
#include <windows.h>
#endif

namespace seri
{
	void Logger::Init(const LoggerConfig& config)
	{
		namespace attrs = boost::log::attributes;

		GetInstance()._config = config;

		GetInstance()._timeStampLocale = std::locale(
			std::locale::classic(),
			new boost::posix_time::time_facet(GetInstance()._config.timeStampFormat.c_str())
		);

		bool isUTC = GetInstance()._config.clock == LogClock::utc;

		auto core = boost::log::core::get();

		core->remove_all_sinks();

		boost::log::attribute_set attributes;
		attributes.insert("RecordID", attrs::counter<unsigned int>(1));
		attributes.insert("TimeStamp", isUTC ? boost::log::attribute(attrs::utc_clock()) : boost::log::attribute(attrs::local_clock()));
		attributes.insert("ProcessID", attrs::current_process_id());
		attributes.insert("ThreadID", attrs::current_thread_id());
		core->set_global_attributes(attributes);

		EnableColor();

		auto sink = boost::log::add_console_log(std::clog);
		sink->set_formatter(&FormatRecord);
		sink->locked_backend()->auto_flush(GetInstance()._config.autoFlush);

		SetLogLevel(GetInstance()._config.level);

		GetInstance()._inited = true;

		LIB_LOGGER(info, logger) << "inited, level: " << ToString(GetInstance()._config.level)
			<< ", clock: " << (isUTC ? "utc" : "local")
			<< ", timestamp format: " << GetInstance()._config.timeStampFormat;
	}

	void Logger::Shutdown()
	{
		if (!GetInstance()._inited)
		{
			return;
		}

		LIB_LOGGER(info, logger) << "shutting down";

		auto core = boost::log::core::get();
		core->flush();
		core->set_logging_enabled(false);
		core->remove_all_sinks();
		core->reset_filter();

		GetInstance()._inited = false;
	}

	void Logger::SetLogLevel(LogLevel level)
	{
		GetInstance()._config.level = level;

		auto core = boost::log::core::get();

		if (level == LogLevel::none)
		{
			core->set_logging_enabled(false);
			return;
		}

		core->set_logging_enabled(true);
		core->set_filter(boost::log::expressions::attr<LogLevel>("Severity") <= level);
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

	void Logger::EnableColor()
	{
		GetInstance()._colorEnabled = false;

#ifdef SERI_WINDOWS
		HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
		if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		DWORD mode = 0;
		if (!GetConsoleMode(handle, &mode))
		{
			return;
		}

		if (!SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
		{
			return;
		}

		GetInstance()._colorEnabled = true;
#endif
	}

	const char* Logger::ToColor(LogLevel level)
	{
		switch (level)
		{
			case LogLevel::error:
				return "\033[91m";
			case LogLevel::warning:
				return "\033[93m";
			case LogLevel::info:
				return "\033[92m";
			case LogLevel::verbose:
				return "\033[90m";
			default:
				return "\033[0m";
		}
	}

	void Logger::FormatRecord(const boost::log::record_view& record, boost::log::formatting_ostream& stream)
	{
		auto severity = boost::log::extract<LogLevel>("Severity", record);

		auto timeStamp = boost::log::extract<boost::posix_time::ptime>("TimeStamp", record);
		stream.imbue(GetInstance()._timeStampLocale);
		stream << timeStamp.get() << " ";

		if (GetInstance()._config.showThreadId)
		{
			auto threadId = boost::log::extract<boost::log::attributes::current_thread_id::value_type>("ThreadID", record);
			if (threadId)
			{
				stream << "[" << threadId.get() << "] ";
			}
		}

		LogLevel level = severity ? severity.get() : LogLevel::none;

		if (GetInstance()._colorEnabled)
		{
			stream << ToColor(level) << "[" << ToString3(level) << "]" << "\033[0m" << " ";
		}
		else
		{
			stream << "[" << ToString3(level) << "] ";
		}

		auto module = boost::log::extract<std::string>("Module", record);
		if (module)
		{
			stream << "[" << module.get() << "] ";
		}

		if (severity && severity.get() == LogLevel::error)
		{
			auto file = boost::log::extract<std::string>("File", record);
			auto line = boost::log::extract<unsigned int>("Line", record);
			if (file && line)
			{
				stream << "(" << file.get() << ":" << line.get() << ") ";
			}
		}

		stream << record[boost::log::expressions::smessage];
	}

}
