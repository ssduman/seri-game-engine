#include "Seripch.h"

#include "seri/scene/SceneManager.h"
#include "seri/scene/Scene.h"
#include "seri/system/ScriptSystem.h"
#include "seri/system/PhysicsSystem.h"
#include "seri/font/FontManager.h"
#include "seri/scene/Prefab.h"
#include "seri/asset/AssetManager.h"
#include "seri/project/ProjectManager.h"

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
		RegisterComponent<seri::component::CanvasComponent>();
		RegisterComponent<seri::component::RectComponent>();
		RegisterComponent<seri::component::ButtonComponent>();
		RegisterComponent<seri::component::SpriteRendererComponent>();
		RegisterComponent<seri::component::AnimatorComponent>();
		RegisterComponent<seri::component::CameraComponent>();
		RegisterComponent<seri::component::DirectionalLightComponent>();
		RegisterComponent<seri::component::SpotLightComponent>();
		RegisterComponent<seri::component::PointLightComponent>();
		RegisterComponent<seri::component::TextComponent>();
		RegisterComponent<seri::component::AudioComponent>();
		RegisterComponent<seri::component::RigidbodyComponent>();
		RegisterComponent<seri::component::ColliderComponent>();
		RegisterComponent<seri::component::ScriptComponent>();

		SceneManager::GetInstance()._activeScene = std::make_shared<Scene>();
		SceneManager::GetInstance()._activeScene->Init();

		SceneManager::GetInstance()._activeScene->Deserialize(seri::project::ProjectManager::GetStartupScene().string());
	}

	void SceneManager::Update()
	{
		auto& instance = GetInstance();

		instance.TryReload();

		instance._activeScene->Update();
	}

	void SceneManager::ReloadScene()
	{
		GetInstance()._reloadRequested = true;
	}

	uint64_t SceneManager::InstantiatePrefab(uint64_t prefabAssetId, uint64_t parentId)
	{
		std::shared_ptr<Prefab> prefab = seri::asset::AssetManager::GetAssetByID<Prefab>(prefabAssetId);
		if (!prefab || !prefab->entities.IsSequence())
		{
			LIB_LOGGER(warning, scene) << "prefab " << prefabAssetId << " not found";
			return 0;
		}

		return GetInstance()._activeScene->InstantiateEntities(prefab->entities, parentId);
	}

	void SceneManager::Destroy(uint64_t entityId)
	{
		GetInstance()._pendingDestroyIds.push_back(entityId);
	}

	void SceneManager::FlushDestroyed()
	{
		auto& instance = GetInstance();

		if (instance._pendingDestroyIds.empty())
		{
			return;
		}

		std::vector<uint64_t> ids = std::move(instance._pendingDestroyIds);
		instance._pendingDestroyIds.clear();

		for (uint64_t id : ids)
		{
			if (instance._activeScene->HasEntity(id))
			{
				instance._activeScene->DeleteEntity(id);
			}
		}
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
			instance._pendingDestroyIds.clear();

			seri::system::ScriptSystem::Reset();
			seri::system::PhysicsSystem::Reset();

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

		if (compName == seri::component::TextComponent::kCompName)
		{
			if (std::shared_ptr<seri::font::Font> font = seri::font::FontManager::GetDefaultFont())
			{
				GetInstance().registry.get<seri::component::TextComponent>(entity).fontAssetId = font->id;
			}
		}
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

	void SceneManager::TryReload()
	{
		if (_reloadRequested)
		{
			_reloadRequested = false;
			_pendingDestroyIds.clear();

			seri::system::ScriptSystem::Reset();
			seri::system::PhysicsSystem::Reset();

			if (_hasSnapshot)
			{
				_activeScene->DeserializeFromNode(_snapshot);
			}
			else
			{
				_activeScene->Reload();
			}

			LIB_LOGGER(info, scene) << "scene reloaded";
		}
	}

}
