#include "Seripch.h"

#include "seri/event/EventManager.h"

namespace seri::event
{
	EventHandle EventManager::SubscribeImpl(std::type_index typeId, std::shared_ptr<IEventCallback> callback)
	{
		if (!callback)
		{
			return 0;
		}

		EventManager& instance = GetInstance();

		EventHandle handle = instance._nextHandle++;
		instance._subscribers[typeId].emplace_back(Subscriber{ handle, std::move(callback) });

		return handle;
	}

	void EventManager::Unsubscribe(EventHandle handle)
	{
		if (handle == 0)
		{
			return;
		}

		for (auto& [typeId, subscribers] : GetInstance()._subscribers)
		{
			auto removed = std::erase_if(subscribers,
				[handle](const Subscriber& subscriber)
				{
					return subscriber.handle == handle;
				}
			);

			if (removed > 0)
			{
				return;
			}
		}
	}

	bool EventManager::Fire(const IEventData& data)
	{
		EventManager& instance = GetInstance();

		auto it = instance._subscribers.find(std::type_index(typeid(data)));
		if (it == instance._subscribers.end())
		{
			return false;
		}

		for (const Subscriber& subscriber : it->second)
		{
			if (subscriber.callback->FireEvent(data))
			{
				return true;
			}
		}

		return false;
	}
}
