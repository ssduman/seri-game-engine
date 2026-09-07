#include "Seripch.h"

#include "seri/logging/LogBuffer.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/expressions.hpp>

namespace seri
{
	void LogBuffer::Push(LogEntry entry)
	{
		std::lock_guard<std::mutex> lock(GetInstance()._mutex);

		if (GetInstance()._pending.size() >= GetInstance()._capacity)
		{
			GetInstance()._pending.pop_front();
		}

		GetInstance()._pending.push_back(std::move(entry));
	}

	void LogBuffer::Drain(std::vector<LogEntry>& out)
	{
		std::lock_guard<std::mutex> lock(GetInstance()._mutex);

		if (GetInstance()._pending.empty())
		{
			return;
		}

		out.insert(
			out.end(),
			std::make_move_iterator(GetInstance()._pending.begin()),
			std::make_move_iterator(GetInstance()._pending.end())
		);

		GetInstance()._pending.clear();
	}

	void LogBuffer::Clear()
	{
		std::lock_guard<std::mutex> lock(GetInstance()._mutex);

		GetInstance()._pending.clear();
	}

	void LogBuffer::SetCapacity(size_t capacity)
	{
		std::lock_guard<std::mutex> lock(GetInstance()._mutex);

		GetInstance()._capacity = capacity == 0 ? 1 : capacity;
	}

	void LogBufferBackend::consume(const boost::log::record_view& record, const string_type& message)
	{
		LogEntry entry;

		entry.message = message;

		auto severity = boost::log::extract<LogLevel>("Severity", record);
		entry.level = severity ? severity.get() : LogLevel::none;

		auto timeStamp = boost::log::extract<boost::posix_time::ptime>("TimeStamp", record);
		if (timeStamp)
		{
			boost::posix_time::time_duration timeOfDay = timeStamp.get().time_of_day();

			int milliseconds = static_cast<int>(
				timeOfDay.fractional_seconds() / (boost::posix_time::time_duration::ticks_per_second() / 1000)
			);

			char buffer[16];
			snprintf(
				buffer, sizeof(buffer), "%02d:%02d:%02d.%03d",
				static_cast<int>(timeOfDay.hours()),
				static_cast<int>(timeOfDay.minutes()),
				static_cast<int>(timeOfDay.seconds()),
				milliseconds
			);

			entry.timeStamp = buffer;
		}

		auto threadId = boost::log::extract<boost::log::attributes::current_thread_id::value_type>("ThreadID", record);
		if (threadId)
		{
			std::ostringstream stream;
			stream << threadId.get();
			entry.threadId = stream.str();
		}

		auto module = boost::log::extract<std::string>("Module", record);
		if (module)
		{
			entry.module = module.get();
		}

		auto file = boost::log::extract<std::string>("File", record);
		if (file)
		{
			entry.file = file.get();
		}

		auto function = boost::log::extract<std::string>("Function", record);
		if (function)
		{
			entry.function = function.get();
		}

		auto line = boost::log::extract<unsigned int>("Line", record);
		if (line)
		{
			entry.line = line.get();
		}

		LogBuffer::Push(std::move(entry));
	}
}
