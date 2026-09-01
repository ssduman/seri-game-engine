#pragma once

#include "seri/event/EventManager.h"
#include "seri/logging/Logger.h"

#include <vector>

namespace seri::event
{
	class EventDispatcher
	{
	public:
		EventDispatcher() = default;

		~EventDispatcher()
		{
			Unregister();
		}

		void Register()
		{
			Unregister();

			_handles.push_back(EventManager::Subscribe<KeyEventData>(
				[](const KeyEventData& data) -> bool
				{
					LOGGER(verbose) << data.ToString();
					return false;
				}
			));

			_handles.push_back(EventManager::Subscribe<MouseButtonEventData>(
				[](const MouseButtonEventData& data) -> bool
				{
					LOGGER(verbose) << data.ToString();
					return false;
				}
			));

			_handles.push_back(EventManager::Subscribe<WindowDropEventData>(
				[](const WindowDropEventData& data) -> bool
				{
					LOGGER(info) << data.ToString();
					return false;
				}
			));
		}

		void Unregister()
		{
			for (EventHandle handle : _handles)
			{
				EventManager::Unsubscribe(handle);
			}

			_handles.clear();
		}

	private:
		std::vector<EventHandle> _handles{};

	};
}
