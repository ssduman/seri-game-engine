#include "Seripch.h"

#include "seri/scene/SceneManager.h"
#include "seri/scene/Scene.h"
#include "seri/script/ScriptSystem.h"

#include <entt/entt.hpp>

#include <memory>
#include <vector>

namespace seri::scene
{
	void SceneManager::Init()
	{
		RegisterComponent<seri::component::IDComponent>();
		RegisterComponent<seri::component::TransformComponent>();
		RegisterComponent<seri::component::SceneComponent>();
		RegisterComponent<seri::component::MeshComponent>();
		RegisterComponent<seri::component::MeshRendererComponent>();
		RegisterComponent<seri::component::SkinnedMeshRendererComponent>();
		RegisterComponent<seri::component::AnimatorComponent>();
		RegisterComponent<seri::component::CameraComponent>();
		RegisterComponent<seri::component::DirectionalLightComponent>();
		RegisterComponent<seri::component::SpotLightComponent>();
		RegisterComponent<seri::component::PointLightComponent>();
		RegisterComponent<seri::component::TextComponent>();
		RegisterComponent<seri::component::ScriptComponent>();

		SceneManager::GetInstance()._activeScene = std::make_shared<Scene>();
		SceneManager::GetInstance()._activeScene->Init();

		SceneManager::GetInstance()._activeScene->Deserialize("assets/scenes/main.sscene");
	}

	void SceneManager::Update()
	{
		SceneManager::GetInstance()._activeScene->Update();
	}

	entt::registry& SceneManager::GetRegistry()
	{
		return SceneManager::GetInstance().registry;
	}

	entt::entity SceneManager::CreateEntity()
	{
		return GetRegistry().create();
	}

	void SceneManager::DestroyEntity(entt::entity entity)
	{
		GetRegistry().destroy(entity);
	}

	SceneState SceneManager::GetState()
	{
		return GetInstance()._state;
	}

	void SceneManager::SetState(SceneState newState)
	{
		auto& instance = GetInstance();

		if (instance._state == newState)
		{
			return;
		}

		SceneState prevState = instance._state;

		instance._state = newState;

		if (prevState == SceneState::edit)
		{
			instance._snapshot = instance._activeScene->SerializeToNode();
			instance._snapshotDirty = instance._activeScene->IsDirty();
			instance._hasSnapshot = true;
		}

		if (newState == SceneState::edit)
		{
			seri::script::ScriptSystem::Reset();

			if (instance._hasSnapshot)
			{
				instance._activeScene->DeserializeFromNode(instance._snapshot);

				if (instance._snapshotDirty)
				{
					instance._activeScene->SetAsDirty();
				}

				instance._snapshot = YAML::Node{};
				instance._hasSnapshot = false;
			}
		}
	}

	std::shared_ptr<Scene> SceneManager::GetActiveScene()
	{
		return GetInstance()._activeScene;
	}

	const std::vector<seri::scene::ComponentIO>& SceneManager::GetCompIO()
	{
		return GetInstance()._componentIOs;
	}

	bool SceneManager::HasComponent(entt::entity entity, std::string_view compName)
	{
		auto& comps = GetInstance()._componentIOmaps;
		if (!comps.contains(compName))
		{
			return false;
		}

		return comps[compName].Has(GetInstance().registry, entity);
	}

	void SceneManager::AddComponent(entt::entity entity, std::string_view compName)
	{
		auto& comps = GetInstance()._componentIOmaps;
		if (!comps.contains(compName))
		{
			return;
		}

		GetInstance()._activeScene->SetAsDirty();
		comps[compName].Add(GetInstance().registry, entity);
	}

	void SceneManager::RemoveComponent(entt::entity entity, std::string_view compName)
	{
		auto& comps = GetInstance()._componentIOmaps;
		if (!comps.contains(compName))
		{
			return;
		}

		GetInstance()._activeScene->SetAsDirty();
		comps[compName].Remove(GetInstance().registry, entity);
	}

	void SceneManager::SerializeComponent(entt::entity entity, YAML::Node& node, std::string_view compName)
	{
		auto& comps = GetInstance()._componentIOmaps;
		if (!comps.contains(compName))
		{
			return;
		}

		GetInstance()._activeScene->SetAsDirty();
		comps[compName].Serialize(GetInstance().registry, entity, node);
	}

	void SceneManager::DeserializeComponent(entt::entity entity, const YAML::Node& node, std::string_view compName)
	{
		auto& comps = GetInstance()._componentIOmaps;
		if (!comps.contains(compName))
		{
			return;
		}

		GetInstance()._activeScene->SetAsDirty();
		comps[compName].Deserialize(GetInstance().registry, entity, node);
	}

}
