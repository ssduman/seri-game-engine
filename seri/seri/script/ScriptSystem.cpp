#include "Seripch.h"

#include "seri/script/ScriptSystem.h"
#include "seri/script/scripts/RotatorScript.h"
#include "seri/scene/SceneManager.h"

#include <fmt/format.h>

namespace seri::script
{
	namespace
	{
		template<typename Fn>
		bool SafeCall(ScriptInstance& instance, const char* stage, Fn&& fn)
		{
			try
			{
				fn();
				return true;
			}
			catch (const std::exception& ex)
			{
				instance.faulted = true;
				LOGGER(error) << fmt::format("[script] '{}' threw, script disabled: {}", stage, ex.what());
			}
			catch (...)
			{
				instance.faulted = true;
				LOGGER(error) << fmt::format("[script] '{}' threw unknown exception, script disabled", stage);
			}

			return false;
		}
	}

	void ScriptSystem::Init()
	{
		if (_inited)
		{
			return;
		}

		ScriptRegistry::Register<RotatorScript>("Rotator");

		scene::SceneManager::GetRegistry()
			.on_destroy<component::ScriptComponent>()
			.connect<&ScriptSystem::OnScriptComponentDestroyed>();

		_inited = true;

		LOGGER(info) << fmt::format("[script] inited with {} registered script(s)", ScriptRegistry::GetNames().size());
	}

	void ScriptSystem::Update(float deltaTime)
	{
		if (!_enabled)
		{
			return;
		}

		auto& registry = scene::SceneManager::GetRegistry();
		auto view = registry.view<component::ScriptComponent>();

		for (entt::entity entity : view)
		{
			auto& scriptComponent = view.get<component::ScriptComponent>(entity);

			if (scriptComponent.dirty)
			{
				RebuildInstances(registry, entity, scriptComponent);
			}

			auto it = _instances.find(entity);
			if (it == _instances.end())
			{
				continue;
			}

			std::vector<ScriptInstance>& instances = it->second;

			for (size_t i = 0; i < instances.size(); i++)
			{
				ScriptInstance& instance = instances[i];
				if (!instance.script || instance.faulted)
				{
					continue;
				}

				const bool enabled = scriptComponent.entries[i].enabled;

				if (enabled != instance.running)
				{
					instance.running = enabled;
					SafeCall(instance, enabled ? "OnEnable" : "OnDisable", [&]()
						{
							if (enabled)
							{
								instance.script->OnEnable();
							}
							else
							{
								instance.script->OnDisable();
							}
						});
				}

				if (!enabled || instance.faulted)
				{
					continue;
				}

				if (!instance.started)
				{
					instance.started = true;
					if (!SafeCall(instance, "OnStart", [&]() { instance.script->OnStart(); }))
					{
						continue;
					}
				}

				SafeCall(instance, "OnUpdate", [&]() { instance.script->OnUpdate(deltaTime); });
			}
		}

		FlushDeferred();
	}

	void ScriptSystem::LateUpdate(float deltaTime)
	{
		if (!_enabled)
		{
			return;
		}

		auto& registry = scene::SceneManager::GetRegistry();
		auto view = registry.view<component::ScriptComponent>();

		for (entt::entity entity : view)
		{
			auto& scriptComponent = view.get<component::ScriptComponent>(entity);

			auto it = _instances.find(entity);
			if (it == _instances.end())
			{
				continue;
			}

			std::vector<ScriptInstance>& instances = it->second;

			for (size_t i = 0; i < instances.size(); i++)
			{
				ScriptInstance& instance = instances[i];
				if (!instance.script || instance.faulted || !instance.started || !scriptComponent.entries[i].enabled)
				{
					continue;
				}

				SafeCall(instance, "OnLateUpdate", [&]() { instance.script->OnLateUpdate(deltaTime); });
			}
		}

		FlushDeferred();
	}

	void ScriptSystem::Defer(std::function<void()> command)
	{
		_deferred.emplace_back(std::move(command));
	}

	void ScriptSystem::SetEnabled(bool enabled)
	{
		_enabled = enabled;
	}

	bool ScriptSystem::IsEnabled()
	{
		return _enabled;
	}

	void ScriptSystem::OnScriptComponentDestroyed(entt::registry& registry, entt::entity entity)
	{
		DestroyInstances(entity);
	}

	void ScriptSystem::RebuildInstances(entt::registry& registry, entt::entity entity, component::ScriptComponent& scriptComponent)
	{
		DestroyInstances(entity);

		seri::Entity handle{ entity, &registry };

		std::vector<ScriptInstance> instances{};
		instances.reserve(scriptComponent.entries.size());

		for (const auto& entry : scriptComponent.entries)
		{
			ScriptInstance instance{};
			instance.script = ScriptRegistry::Create(entry.name);

			if (!instance.script)
			{
				LOGGER(error) << fmt::format("[script] '{}' is not registered", entry.name);
			}
			else
			{
				instance.script->entity = handle;
				SafeCall(instance, "OnCreate", [&]() { instance.script->OnCreate(); });
			}

			instances.emplace_back(std::move(instance));
		}

		_instances[entity] = std::move(instances);

		scriptComponent.dirty = false;
	}

	void ScriptSystem::DestroyInstances(entt::entity entity)
	{
		auto it = _instances.find(entity);
		if (it == _instances.end())
		{
			return;
		}

		for (ScriptInstance& instance : it->second)
		{
			if (!instance.script || instance.faulted)
			{
				continue;
			}

			if (instance.running)
			{
				instance.running = false;
				SafeCall(instance, "OnDisable", [&]() { instance.script->OnDisable(); });
			}

			SafeCall(instance, "OnDestroy", [&]() { instance.script->OnDestroy(); });
		}

		_instances.erase(it);
	}

	void ScriptSystem::FlushDeferred()
	{
		if (_deferred.empty())
		{
			return;
		}

		std::vector<std::function<void()>> commands = std::move(_deferred);
		_deferred.clear();

		for (auto& command : commands)
		{
			try
			{
				command();
			}
			catch (const std::exception& ex)
			{
				LOGGER(error) << fmt::format("[script] deferred command threw: {}", ex.what());
			}
		}
	}
}
