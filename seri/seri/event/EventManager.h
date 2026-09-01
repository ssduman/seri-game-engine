#pragma once

#include "seri/event/EventCallback.h"
#include "seri/event/EventData.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

namespace seri::event
{
	using EventHandle = uint64_t;

	class EventManager
	{
	public:
		EventManager(EventManager const&) = delete;

		void operator=(EventManager const&) = delete;

		static EventManager& GetInstance()
		{
			static EventManager instance;
			return instance;
		}

		template<typename T, typename F>
		static EventHandle Subscribe(F callback)
		{
			static_assert(std::is_base_of_v<IEventData, T>, "event type must derive from IEventData");

			return SubscribeImpl(std::type_index(typeid(T)), std::move(MakeEventCallback(
				[fn = std::move(callback)](const IEventData& data) mutable -> bool
				{
					return fn(static_cast<const T&>(data));
				}
			)));
		}

		static void Unsubscribe(EventHandle handle);

		static bool Fire(const IEventData& data);

	private:
		EventManager() = default;
		~EventManager() = default;

		static EventHandle SubscribeImpl(std::type_index typeId, std::shared_ptr<IEventCallback> callback);

		struct Subscriber
		{
			EventHandle handle{ 0 };
			std::shared_ptr<IEventCallback> callback{ nullptr };
		};

		std::atomic<EventHandle> _nextHandle{ 1 };
		std::unordered_map<std::type_index, std::vector<Subscriber>> _subscribers{};

	};
}
