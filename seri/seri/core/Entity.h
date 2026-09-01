#pragma once

#include <entt/entt.hpp>

#include <utility>

namespace seri
{
	class Entity
	{
	public:
		Entity() = default;

		Entity(entt::entity handle, entt::registry* registry) : _handle(handle), _registry(registry) {}

		template<typename T>
		bool Has() const
		{
			return IsValid() && _registry->any_of<T>(_handle);
		}

		template<typename T>
		T& Get() const
		{
			return _registry->get<T>(_handle);
		}

		template<typename T>
		T* TryGet() const
		{
			return IsValid() ? _registry->try_get<T>(_handle) : nullptr;
		}

		template<typename T, typename... Args>
		T& Add(Args&&... args) const
		{
			return _registry->emplace_or_replace<T>(_handle, std::forward<Args>(args)...);
		}

		template<typename T>
		void Remove() const
		{
			if (Has<T>())
			{
				_registry->remove<T>(_handle);
			}
		}

		bool IsValid() const
		{
			return _registry != nullptr && _registry->valid(_handle);
		}

		entt::entity GetHandle() const
		{
			return _handle;
		}

		entt::registry* GetRegistry() const
		{
			return _registry;
		}

		operator entt::entity() const
		{
			return _handle;
		}

		explicit operator bool() const
		{
			return IsValid();
		}

		bool operator==(const Entity& other) const
		{
			return _handle == other._handle && _registry == other._registry;
		}

	private:
		entt::entity _handle{ entt::null };
		entt::registry* _registry{ nullptr };

	};
}
