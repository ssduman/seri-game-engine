#pragma once

#include "seri/event/EventData.h"

#include <memory>

namespace seri::event
{
	class IEventCallback
	{
	public:
		IEventCallback() = default;

		virtual ~IEventCallback() = default;

		virtual void FireEvent(IEventData& data) = 0;

	};

	template <typename F>
	class EventCallback : public IEventCallback
	{
	public:
		EventCallback(F f) : _f(std::move(f)) {};

		~EventCallback() override = default;

		void FireEvent(IEventData& data) override
		{
			_f(data);
		}

	private:
		F _f;

	};

	template <typename F>
	inline std::shared_ptr<IEventCallback> MakeEventCallback(F f)
	{
		return std::make_shared<EventCallback<F>>(std::move(f));
	}
}
