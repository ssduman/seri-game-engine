#include "Seripch.h"

#include "seri/logging/LogBuffer.h"

#include <spdlog/pattern_formatter.h>

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

	LogBufferSink::LogBufferSink(spdlog::pattern_time_type timeType)
		: spdlog::sinks::base_sink<std::mutex>(std::make_unique<spdlog::pattern_formatter>("%H:%M:%S.%e", timeType, ""))
	{
	}

	void LogBufferSink::sink_it_(const spdlog::details::log_msg& msg)
	{
		spdlog::memory_buf_t timeStamp;
		formatter_->format(msg, timeStamp);

		LogEntry entry;

		entry.level = Logger::FromSpdlogLevel(msg.level);
		entry.line = static_cast<unsigned int>(msg.source.line);
		entry.timeStamp.assign(timeStamp.data(), timeStamp.size());
		entry.threadId = std::to_string(msg.thread_id);
		entry.module.assign(msg.logger_name.data(), msg.logger_name.size());
		entry.file = msg.source.filename ? msg.source.filename : "";
		entry.function = msg.source.funcname ? msg.source.funcname : "";
		entry.message.assign(msg.payload.data(), msg.payload.size());

		LogBuffer::Push(std::move(entry));
	}

	void LogBufferSink::flush_()
	{
	}
}
