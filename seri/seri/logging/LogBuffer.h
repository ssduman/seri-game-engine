#pragma once

#include "seri/logging/Logger.h"

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/frontend_requirements.hpp>
#include <boost/log/sinks/sync_frontend.hpp>

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

	class LogBufferBackend : public boost::log::sinks::basic_formatted_sink_backend<char, boost::log::sinks::synchronized_feeding>
	{
	public:
		void consume(const boost::log::record_view& record, const string_type& message);
	};

	using LogBufferSink = boost::log::sinks::synchronous_sink<LogBufferBackend>;
}
