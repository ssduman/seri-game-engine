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

namespace seri::system
{
	struct ScriptInstance
	{
		std::unique_ptr<script::ScriptBase> script{ nullptr };
		bool started{ false };
		bool running{ false };
		bool faulted{ false };
	};

	class ScriptSystem
	{
	public:
		static void Init();

		static void FixedUpdate(float fixedDeltaTime);
		static void Update(float deltaTime);
		static void LateUpdate(float deltaTime);

		static void Sync();
		static void Reset();

		static void Dispatch(entt::entity entity, const char* stage, const std::function<void(script::ScriptBase&)>& callback);

		static void Defer(std::function<void()> command);

		static void SetEnabled(bool enabled);
		static bool IsEnabled();

		static void Rebuild(const std::string& name);

		static script::ScriptBase* FindScript(entt::entity entity, const std::string& name);

		static void OverrideFields(entt::entity entity, size_t index);
		static std::vector<script::ScriptField> GetSerializedFields(entt::entity entity, size_t index);

	private:
		static void OnScriptComponentDestroyed(entt::registry& registry, entt::entity entity);

		static void DispatchAll(const char* stage, const std::function<void(script::ScriptBase&)>& callback);

		static void RebuildInstances(entt::registry& registry, entt::entity entity, component::ScriptComponent& scriptComponent);
		static void DestroyInstances(entt::entity entity);

		static inline std::unordered_map<entt::entity, std::vector<ScriptInstance>> _scriptInstances{};
		static inline bool _enabled{ true };
		static inline bool _inited{ false };

	};
}
