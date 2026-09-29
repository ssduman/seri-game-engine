#pragma once

#include "seri/logging/Logger.h"

#include <spdlog/sinks/base_sink.h>

#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace seri
{
	struct LogEntry
	{
		LogLevel level{ LogLevel::none };
		unsigned int line{ 0 };
		std::string timeStamp;
		std::string threadId;
		std::string module;
		std::string file;
		std::string function;
		std::string message;
	};

	class LogBuffer
	{
	public:
		static void Push(LogEntry entry);

		static void Drain(std::vector<LogEntry>& out);

		static void Clear();

		static void SetCapacity(size_t capacity);

	private:
		LogBuffer() = default;
		~LogBuffer() = default;

		LogBuffer(LogBuffer&& other) = delete;
		LogBuffer(const LogBuffer& other) = delete;
		LogBuffer& operator=(LogBuffer&& other) = delete;
		LogBuffer& operator=(const LogBuffer& other) = delete;

		static LogBuffer& GetInstance()
		{
			static LogBuffer instance;
			return instance;
		}

		std::mutex _mutex;
		std::deque<LogEntry> _pending;
		size_t _capacity{ 4096 };
	};

	class LogBufferSink : public spdlog::sinks::base_sink<std::mutex>
	{
	public:
		explicit LogBufferSink(spdlog::pattern_time_type timeType);

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override;
		void flush_() override;
	};
}
