#include "Seripch.h"

#include "seri/system/ScriptSystem.h"
#include "seri/scene/SceneManager.h"

#include <fmt/format.h>

namespace seri::system
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
				LIB_LOGGER(error, script_system) << fmt::format("'{}' threw, script disabled: {}", stage, ex.what());
			}
			catch (...)
			{
				instance.faulted = true;
				LIB_LOGGER(error, script_system) << fmt::format("'{}' threw unknown exception, script disabled", stage);
			}

			return false;
		}

		void ApplyFieldsTo(const component::ScriptComponent::Entry& entry, script::ScriptBase& script)
		{
			if (!entry.fields.IsMap())
			{
				return;
			}

			std::vector<script::ScriptField> fields = script.GetSerializedFields();

			for (const script::ScriptField& field : fields)
			{
				const YAML::Node& node = entry.fields[field.name];
				if (!node || field.ptr == nullptr)
				{
					continue;
				}

				switch (field.type)
				{
					case script::ScriptField::Type::boolean:
						*static_cast<bool*>(field.ptr) = node.as<bool>();
						break;
					case script::ScriptField::Type::integer:
						*static_cast<int*>(field.ptr) = node.as<int>();
						break;
					case script::ScriptField::Type::floating:
						*static_cast<float*>(field.ptr) = node.as<float>();
						break;
					case script::ScriptField::Type::vec3:
					case script::ScriptField::Type::color3:
						*static_cast<glm::vec3*>(field.ptr) = YAMLUtil::Vec3FromYAML(node);
						break;
					case script::ScriptField::Type::text:
						*static_cast<std::string*>(field.ptr) = YAMLUtil::DeepCopyYAMLString(node);
						break;
				}
			}
		}

		void OverrideFieldsFrom(component::ScriptComponent::Entry& entry, script::ScriptBase& script)
		{
			std::vector<script::ScriptField> fields = script.GetSerializedFields();

			YAML::Node node;

			for (const script::ScriptField& field : fields)
			{
				if (field.ptr == nullptr)
				{
					continue;
				}

				switch (field.type)
				{
					case script::ScriptField::Type::boolean:
						node[field.name] = *static_cast<bool*>(field.ptr);
						break;
					case script::ScriptField::Type::integer:
						node[field.name] = *static_cast<int*>(field.ptr);
						break;
					case script::ScriptField::Type::floating:
						node[field.name] = *static_cast<float*>(field.ptr);
						break;
					case script::ScriptField::Type::vec3:
					case script::ScriptField::Type::color3:
						node[field.name] = YAMLUtil::Vec3ToYAML(*static_cast<glm::vec3*>(field.ptr));
						break;
					case script::ScriptField::Type::text:
						node[field.name] = *static_cast<std::string*>(field.ptr);
						break;
				}
			}

			entry.fields = node;
		}
	}

	void ScriptSystem::Init()
	{
		if (_inited)
		{
			return;
		}

		scene::SceneManager::GetRegistry()
			.on_destroy<component::ScriptComponent>().connect<&ScriptSystem::OnScriptComponentDestroyed>();

		_inited = true;

		LIB_LOGGER(info, script_system) << "inited with " << script::ScriptRegistry::GetNames().size() << " registered script(s)";
	}

	void ScriptSystem::Sync()
	{
		auto& registry = scene::SceneManager::GetRegistry();
		auto view = registry.view<component::ScriptComponent>();
		std::vector<entt::entity> entities(view.begin(), view.end());

		for (entt::entity entity : entities)
		{
			auto* scriptComponent = registry.try_get<component::ScriptComponent>(entity);

			if (scriptComponent != nullptr && scriptComponent->dirty)
			{
				RebuildInstances(registry, entity, *scriptComponent);
			}
		}
	}

	void ScriptSystem::Update(float deltaTime)
	{
		if (!_enabled)
		{
			return;
		}

		auto& registry = scene::SceneManager::GetRegistry();
		auto view = registry.view<component::ScriptComponent>();
		std::vector<entt::entity> entities(view.begin(), view.end());

		for (entt::entity entity : entities)
		{
			auto* scriptComponentPtr = registry.try_get<component::ScriptComponent>(entity);
			if (scriptComponentPtr == nullptr)
			{
				continue;
			}

			auto& scriptComponent = *scriptComponentPtr;

			if (scriptComponent.dirty)
			{
				RebuildInstances(registry, entity, scriptComponent);
			}

			auto* transform = registry.try_get<component::TransformComponent>(entity);
			if (transform != nullptr && !transform->isActiveInHierarchy)
			{
				continue;
			}

			auto it = _scriptInstances.find(entity);
			if (it == _scriptInstances.end())
			{
				continue;
			}

			std::vector<ScriptInstance>& instances = it->second;

			auto* button = registry.try_get<component::ButtonComponent>(entity);
			bool clicked = button != nullptr && button->clicked;

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

				if (clicked && !SafeCall(instance, "OnClick", [&]() { instance.script->OnClick(); }))
				{
					continue;
				}

				SafeCall(instance, "OnUpdate", [&]() { instance.script->OnUpdate(deltaTime); });
			}
		}
	}

	void ScriptSystem::LateUpdate(float deltaTime)
	{
		if (!_enabled)
		{
			return;
		}

		auto& registry = scene::SceneManager::GetRegistry();
		auto view = registry.view<component::ScriptComponent>();
		std::vector<entt::entity> entities(view.begin(), view.end());

		for (entt::entity entity : entities)
		{
			auto* scriptComponentPtr = registry.try_get<component::ScriptComponent>(entity);
			if (scriptComponentPtr == nullptr)
			{
				continue;
			}

			auto& scriptComponent = *scriptComponentPtr;

			auto* transform = registry.try_get<component::TransformComponent>(entity);
			if (transform != nullptr && !transform->isActiveInHierarchy)
			{
				continue;
			}

			auto it = _scriptInstances.find(entity);
			if (it == _scriptInstances.end())
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
	}

	void ScriptSystem::Reset()
	{
		std::vector<entt::entity> entities{};
		entities.reserve(_scriptInstances.size());
		for (const auto& item : _scriptInstances)
		{
			entities.push_back(item.first);
		}

		for (entt::entity entity : entities)
		{
			DestroyInstances(entity);
		}

		_scriptInstances.clear();

		auto& registry = scene::SceneManager::GetRegistry();
		for (entt::entity entity : registry.view<component::ScriptComponent>())
		{
			registry.get<component::ScriptComponent>(entity).dirty = true;
		}
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
			instance.script = script::ScriptRegistry::Create(entry.name);

			if (!instance.script)
			{
				LIB_LOGGER(error, script_system) << fmt::format("'{}' is not registered", entry.name);
			}
			else
			{
				instance.script->entity = handle;
				SafeCall(instance, "OnCreate", [&]() { instance.script->OnCreate(); });
				ApplyFieldsTo(entry, *instance.script);
			}

			instances.emplace_back(std::move(instance));
		}

		_scriptInstances[entity] = std::move(instances);

		scriptComponent.dirty = false;
	}

	void ScriptSystem::DestroyInstances(entt::entity entity)
	{
		auto it = _scriptInstances.find(entity);
		if (it == _scriptInstances.end())
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

		_scriptInstances.erase(it);
	}

	void ScriptSystem::Rebuild(const std::string& name)
	{
		auto& registry = scene::SceneManager::GetRegistry();
		auto view = registry.view<component::ScriptComponent>();

		for (entt::entity entity : view)
		{
			auto& scriptComponent = view.get<component::ScriptComponent>(entity);

			for (const auto& entry : scriptComponent.entries)
			{
				if (entry.name == name)
				{
					scriptComponent.dirty = true;
					break;
				}
			}
		}
	}

	script::ScriptBase* ScriptSystem::FindScript(entt::entity entity, const std::string& name)
	{
		auto& registry = scene::SceneManager::GetRegistry();
		auto* scriptComponent = registry.try_get<component::ScriptComponent>(entity);

		auto it = _scriptInstances.find(entity);
		if (!scriptComponent || it == _scriptInstances.end())
		{
			return nullptr;
		}

		for (size_t i = 0; i < it->second.size() && i < scriptComponent->entries.size(); i++)
		{
			ScriptInstance& instance = it->second[i];
			if (scriptComponent->entries[i].name == name && instance.script && !instance.faulted)
			{
				return instance.script.get();
			}
		}

		return nullptr;
	}

	void ScriptSystem::OverrideFields(entt::entity entity, size_t index)
	{
		auto it = _scriptInstances.find(entity);
		if (it == _scriptInstances.end() || index >= it->second.size())
		{
			return;
		}

		ScriptInstance& instance = it->second[index];
		if (!instance.script || instance.faulted)
		{
			return;
		}

		auto& registry = scene::SceneManager::GetRegistry();
		auto* scriptComponent = registry.try_get<component::ScriptComponent>(entity);
		if (!scriptComponent || index >= scriptComponent->entries.size())
		{
			return;
		}

		OverrideFieldsFrom(scriptComponent->entries[index], *instance.script);
	}

	std::vector<script::ScriptField> ScriptSystem::GetSerializedFields(entt::entity entity, size_t index)
	{
		std::vector<script::ScriptField> fields{};

		auto it = _scriptInstances.find(entity);
		if (it == _scriptInstances.end() || index >= it->second.size())
		{
			return {};
		}

		ScriptInstance& instance = it->second[index];
		if (!instance.script || instance.faulted)
		{
			return {};
		}

		return instance.script->GetSerializedFields();
	}
}
