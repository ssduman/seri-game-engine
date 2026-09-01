#pragma once

#include "seri/core/Entity.h"
#include "seri/component/Components.h"
#include "seri/script/ScriptBase.h"
#include "seri/script/ScriptRegistry.h"

#include <entt/entt.hpp>

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace seri::script
{
	struct ScriptInstance
	{
		std::unique_ptr<ScriptBase> script{ nullptr };
		bool started{ false };
		bool running{ false };
		bool faulted{ false };
	};

	class ScriptSystem
	{
	public:
		static void Init();

		static void Update(float deltaTime);
		static void LateUpdate(float deltaTime);

		static void Defer(std::function<void()> command);

		static void SetEnabled(bool enabled);
		static bool IsEnabled();

	private:
		static void OnScriptComponentDestroyed(entt::registry& registry, entt::entity entity);

		static void RebuildInstances(entt::registry& registry, entt::entity entity, component::ScriptComponent& scriptComponent);
		static void DestroyInstances(entt::entity entity);

		static void FlushDeferred();

		static inline std::unordered_map<entt::entity, std::vector<ScriptInstance>> _instances{};
		static inline std::vector<std::function<void()>> _deferred{};
		static inline bool _enabled{ true };
		static inline bool _inited{ false };

	};
}
